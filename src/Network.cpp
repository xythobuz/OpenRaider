/*!
 * \file src/Network.cpp
 * \brief Networking Singleton class
 *
 * \author Mongoose
 */

#include <Network.h>

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <MatMath.h> // Random Number

//#define LOCAL_BCAST
#define MAX_CLIENTS 32

typedef struct client_s {
    unsigned int uid;
    char active;
    unsigned int seq;
    unsigned int frameExpected;
} client_t;

#ifdef USING_PTHREADS
#include <pthread.h>
pthread_t gPThreadId[3];
#endif

unsigned int gUID;
client_t gClients[MAX_CLIENTS];
unsigned int gNumClients = 0;
network_frame_t gPiggyBack;


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Network *Network::mInstance = 0x0;


Network *Network::Instance()
{
    if (mInstance == 0x0)
    {
        mInstance = new Network();
    }

    return mInstance;
}


void killNetworkSingleton()
{
    printf("Shutting down Network...\n");

    // Requires public deconstructor
    delete Network::Instance();
}


Network::Network()
{
    strncpy(mRemoteHost, "localhost", REMOTE_HOST_STR_SZ);
    memset(mBindHost, 0, BIND_HOST_STR_SZ);
    setPort(8080);

    mPiggyBack = true;
    mNetworkReliable = true;
    mSpawnedClient = false;
    mSpawnedServer = false;
    mKillClient = false;
    mKillServer = false;
    mDebug = false;

    gUID = getUID();

    printf("UID %u\n", gUID);

    for (gNumClients = MAX_CLIENTS; gNumClients > 0;)
    {
        --gNumClients;

        gClients[gNumClients].active = 0;
        gClients[gNumClients].uid = 0;
        gClients[gNumClients].seq = 0;
    }
}


Network::~Network()
{
    killServerThread();
    killClientThread();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

network_frame_t &Network::getPiggyBack()
{
    return gPiggyBack;
}


unsigned int Network::getUID()
{
    struct timeval tv;
    struct timezone tz;


    gettimeofday(&tv, &tz);

    srand(tv.tv_usec);

    return ((unsigned int)(tv.tv_sec * helRandomNum(2.0f, 3.3f) -
                tv.tv_sec * helRandomNum(1.0f, 2.0f)) +
            (unsigned int)(tv.tv_usec * helRandomNum(2.0f, 3.3f) -
                tv.tv_usec * helRandomNum(1.0f, 2.0f)) +
            (unsigned int)helRandomNum(666.0f, 5000.0f));
}


int Network::getPort()
{
    return mPort;
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void *client_thread(void *v)
{
    Network &network = *Network::Instance();
    network.runClient();

    return NULL;
}


void *server_thread(void *v)
{
    Network &network = *Network::Instance();
    network.runServer();

    return NULL;
}


void Network::setBindHost(char *s)
{
    if (!s || !s[0])
        return;

    strncpy(mBindHost, s, BIND_HOST_STR_SZ);
}


void Network::setRemoteHost(char *s)
{
    if (!s || !s[0])
        return;

    strncpy(mRemoteHost, s, REMOTE_HOST_STR_SZ);
}


void Network::setDebug(bool toggle)
{
    mDebug = toggle;
}


void Network::setPort(unsigned int port)
{
    mPort = port;
}


void Network::killServerThread()
{
    mKillServer = true;

    // Remember for mutex
    //  while (mKillServer)
    //  {
    //  }

    mSpawnedServer = false;
}


void Network::killClientThread()
{
    mKillClient = true;

    // Remember for mutex
    //  while (mKillClient)
    //  {
    //  }

    mSpawnedClient = false;
}


void Network::spawnServerThread()
{
    // For now don't handle shutting down server to start client and vv
    if (!mSpawnedServer && !mSpawnedClient)
    {
#ifdef USING_PTHREADS
        pthread_create(gPThreadId + 0, 0, server_thread, NULL);
#else
        printf("Network::spawnServerThread> Build doesn't support threads\n");
#endif

        mSpawnedServer = true;
    }
}


void Network::spawnClientThread()
{
    // For now don't handle shutting down server to start client and vv
    if (!mSpawnedServer && !mSpawnedClient)
    {
#ifdef USING_PTHREADS
        pthread_create(gPThreadId + 1, 0, client_thread, NULL);
#else
        printf("Network::spawnClientThread> Build doesn't support threads\n");
#endif

        mSpawnedClient = true;
    }
}


////////////////////////////////////////////////////////////
// Protected Mutators
////////////////////////////////////////////////////////////

int Network::runServer()
{
    unsigned int fsize;
    int socket_fd, cc, cip;
    struct sockaddr_in s_in, from;
    char hostid[64];
    network_frame_t f;
    unsigned int i;
    unsigned int packetsRecieved = 0;


    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_fd < 0)
    {
        perror("recv_udp:socket");
        return -1;
    }

    if (mBindHost[0])
    {
        strncpy(hostid, mBindHost, 64);
    }
    else
    {
        if (gethostname(hostid, 64) < 0)
        {
            perror("Server: recv_udp:gethostname");
            return -1;
        }

        printf("Server: gethostname returned '%s'\n", hostid);
        fflush(stdout);
    }

    // Setup for port binding
    memset(&s_in, 0, sizeof(s_in));
    s_in.sin_family = AF_INET;
#ifdef LOCAL_BCAST
    // Replace deprecated gethostbyname() with getaddrinfo()
    //struct hostent *hostptr;
    //if ((hostptr = gethostbyname(hostid)) == NULL)
    //{
    //    fprintf(stderr, "Server: recv_udp, Invalid host name '%s'\n", hostid);
    //    return -1;
    //}
    //memcpy((void *)(&s_in.sin_addr), hostptr->h_addr, hostptr->h_length);
    struct addrinfo *result, *res;
    bool found = false;
    int error = getaddrinfo(hostid, NULL, NULL, &result);
    if (error != 0) {
        fprintf(stderr, "Server: %s\n", gai_strerror(error));
        return -1;
    }
    for (res = result; res != NULL; res = res->ai_next) {
        if (res->ss_family == AF_INET) {
            found = true;
            memcpy((void *)&s_in, res->ai_addr, res->ai_addrlen);
            break; // Found something suitable
        }
    }
    freeaddrinfo(result);
    if (!found) {
        fprintf(stderr, "Server: Can't bind to %s\n", hostid);
        return -1;
    }
#else
    s_in.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    int port = getPort();
    s_in.sin_port = htons(port); // htons new

    fflush(stdout);

    // Bind
    while (bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in)) < 0)
    {
        if (s_in.sin_port++ > (port + 10))
        {
            perror("Server: recv_udp:bind exhausted");
            return -1;
        }
    }

    cip = ntohl(s_in.sin_addr.s_addr);

    printf("Server: Started on ( %i.%i.%i.%i:%i )\n",
            cip >> 24, cip << 8 >> 24,
            cip << 16 >> 24, cip << 24 >> 24, s_in.sin_port);

    for (; !mKillClient;)
    {
        fsize = sizeof(from);

        // 1. Wait for event
        // 2. Get inbound frame
        cc = recvfrom(socket_fd, &f, sizeof(network_frame_t), 0,
                (struct sockaddr *)&from, &fsize);

        if (cc < 0)
        {
            perror("Server: recv_udp:recvfrom");
            continue;
        }

        ++packetsRecieved;

        if (mDebug)
        {
            printf("=====================================================\n");
            printf("Packet %u\n", packetsRecieved);
            printf("Server: Recieved packet from %u\n",
                    f.uid);
        }

        // A. Look and see if this client has connected before
        for (i = 0; i < gNumClients; ++i)
        {
            if (gClients[i].uid == f.uid)
            {
                break;
            }
        }

        // B. Collect client data if it's a new connection
        if (!gClients[i].active)
        {
            for (i = 0; i < gNumClients+1; ++i)
            {
                if ((i + 1) < MAX_CLIENTS && !gClients[i].active)
                {
                    gClients[i].uid = f.uid;
                    gClients[i].active = 1;
                    gClients[i].frameExpected = 0;
                    ++gNumClients;

                    printf("Server: %u made connection, as client %u\n",
                            gClients[i].uid, i);
                    break;
                }
            }

            if (i == MAX_CLIENTS || !gClients[i].active)
            {
                if (mDebug)
                {
                    printf("Server: Handshake packet from %u failed?\n",
                            f.uid);
                }
                continue;
            }
        }

        cip = ntohl(from.sin_addr.s_addr);

        if (mDebug)
        {
            printf("Server: Client (Famliy %d, Address %i.%i.%i.%i:%d)\n",
                    ntohs(from.sin_family), cip >> 24, cip << 8 >> 24,
                    cip << 16 >> 24, cip << 24 >> 24,
                    ntohs(from.sin_port));

            printf("Server: Datalink layer recieved: packet seq %u\n",
                    f.seq);
        }

        if (mNetworkReliable)
        {
            if (f.seq == gClients[i].seq)
            {
                if (mDebug)
                {
                    printf("SERVER> Msg from %u\n", f.uid);
                }

                to_network_layer(f.data);
                gClients[i].seq = f.seq;
            }
            else
            {
                continue;
            }
        }

        //! \fixme Combine with above, duh
        // 3. Send to network layer
        if (gClients[i].frameExpected == f.header)
        {
            f.data.cid = i;

            to_network_layer(f.data);
            gClients[i].frameExpected = !gClients[i].frameExpected;
        }

        fflush(stdout);

#ifdef UNIT_TEST_NETWORK
        if ((rand() % 10 == 0))
        {
            printf("Server: Simulating a lost ack %u\n", f.seq);
            continue;
        }
#endif

        // 4. Send ACK, w/ piggyback if requested
        if (mPiggyBack)
        {
            gPiggyBack.header = 0;
            gPiggyBack.seq = f.seq;
            gPiggyBack.uid = gUID;

            if (mDebug)
            {
                printf("SERVER> Sending data by piggyback\n");
            }

            cc = sendto(socket_fd, &gPiggyBack, sizeof(gPiggyBack), 0,
                    (struct sockaddr *)&from, sizeof(from));
        }
        else
        {
            f.header = 0;
            f.seq = 0;
            f.uid = gUID;

            cc = sendto(socket_fd, &f, sizeof(f), 0,
                    (struct sockaddr *)&from, sizeof(from));
        }

        if (cc < 0)
        {
            perror("Server: send_udp:sendto");
        }
        else
        {
            if (mDebug)
            {
                printf("Server: Ack sent to %u\n", gClients[i].uid);
            }
        }
    }

    mKillClient = false;

    return 0;
}


void Network::runClient()
{
    unsigned int fsize, last_frame_sent = 0;
    int socket_fd, cc, done;
    struct sockaddr_in dest;
    struct addrinfo *addr;
    network_frame_t f;
    struct timeval timeout;
    fd_set readfds;
    unsigned int packetsSent = 0;
    unsigned int seq = 0;
    char timedOut = 1;

    if (!mRemoteHost || !mRemoteHost[0])
    {
        return;
    }

    memset((char*) &timeout, 0, sizeof(timeout));
    timeout.tv_sec = 5;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd == -1)
    {
        perror("Client: send_udp: socket");
        exit(0);
    }

    //if ((hostptr = gethostbyname(mRemoteHost)) == NULL)
    //{
    //    fprintf(stderr, "Client: send_udp: invalid host name, %s\n",
    //            mRemoteHost);
    //    exit(0);
    //}

    int error = getaddrinfo(mRemoteHost, NULL, NULL, &addr);
    if (error != 0) {
        fprintf(stderr, "Client: %s\n", gai_strerror(error));
        exit(0);
    }

    // Setup connection
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    int port = getPort();
    dest.sin_port = htons(port);
#ifdef LOCAL_BCAST
    //memcpy(hostptr->h_addr, (char *) &dest.sin_addr, hostptr->h_length);
    bool found = false;
    for (struct addrinfo *res = addr; res != NULL; res = res->ai_next) {
        if (res->ss_family == AF_INET) {
            found = true;
            memcpy((void *)&dest.sin_addr, res->ai_addr.sin_addr, sizeof(res->ai_addr.sin_addr));
            break; // Found something suitable
        }
    }
    freeaddrinfo(addr);
    if (!found) {
        fprintf(stderr, "Client: Can't connect to %s\n", hostid);
        return;
    }
#else
    if (inet_pton(AF_INET, mRemoteHost, &dest.sin_addr) < 0)
    {
        perror("inet_pton");
        return;
    }
#endif


    // init
    f.data.send = 0;
    f.seq = 0;

    for (; !mKillServer;)
    {
        ++packetsSent;

        if (mDebug)
        {
            printf("=====================================================\n");
            printf("Packet %u\n", packetsSent);
        }

        // 1. Get packet to send over wire
        if (mNetworkReliable && timedOut && f.seq != seq)
        {
            if (mDebug)
            {
                printf("Client: Resending packet\n");
            }
        }
        else
        {
            from_network_layer(&f.data, &last_frame_sent);

            if (!f.data.send)
            {
                const struct timespec tmp = {.tv_sec = 0, .tv_nsec = 20000};
                nanosleep(&tmp, NULL);
                continue;
            }
        }

        // 2. Copy to frame
        f.seq = 0;//seq;  // 0 forces all packets to check out
        f.uid = gUID;

        // 3. Send over the wire
        done = 0;
        timedOut = 0;

        while (!done)
        {
            if (mDebug)
            {
                printf("Client: Sending packet %u\n", f.seq);
            }

            cc = sendto(socket_fd, &f, sizeof(f), 0,
                    (struct sockaddr *)&dest, sizeof(dest));

            if (cc < 0)
            {
                perror("Client: send_udp:sendto");

                if (errno == EMSGSIZE)
                {
                    printf("Client: packet was too large\n");
                }
            }
            else
            {
                f.data.send = 0;
            }

            // Comment out this to enable more reliable service
            done = 1;
        }

        // 4. Wait for +ack or resend
        FD_ZERO(&readfds);

        // Setup socket to listen on here
        FD_SET(socket_fd, &readfds);

        // Set timeout in milliseconds
        timeout.tv_usec = 850;

        cc = select(socket_fd + 1, &readfds, NULL, NULL, &timeout);

        if ((cc < 0) && (errno != EINTR))
        {
            // there was an local error with select
        }

        if (cc == 0)
        {
            if (mDebug)
            {
                printf("Client: Timeout detected on packet %u\n", f.seq);
            }
            timedOut = 1;
            continue;
        }

        // Clear header for recv use
        f.header = 0;

        fsize = sizeof(dest);
        cc = recvfrom(socket_fd, &f, sizeof(f), 0,
                (struct sockaddr *)&dest, &fsize);

        if (cc < 0)
        {
            perror("Client: recv_udp:recvfrom");
        }
        else
        {
            if (mDebug)
            {
                printf("Client: Datalink layer recieved: packet seq %u\n", f.seq);
                printf("CLIENT> Msg from %u\n", f.uid);
            }

            to_network_layer(f.data);
        }

        if (seq == f.seq)
        {
            if (mDebug)
            {
                printf("Client: Recieved ack %u\n", f.seq);
            }

            ++seq;
        }
    }

    mKillServer = false;
}

