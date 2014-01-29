/*!
 * \file include/Network.h
 * \brief Networking Singleton class
 *
 * \author Mongoose
 */

#ifndef _NETWORK_H_
#define _NETWORK_H_

/*!
 * \brief The packet holds the data for the network.
 *
 * This is encapsulated in the frame.
 */
typedef struct network_packet_s {
    char send;
    unsigned int port;
    unsigned int cid;
    char  data[32];    //!< A short (32 char) string
    float pos[3];
    float pitch;
    float yaw;
    int aframe;
    int bframe;
    int view_model;
    char gurantee;     //!< TCP like
} network_packet_t;

/*!
 * \brief The frame holds the data for the datalink layer.
 *
 * This goes over the line.
 */
typedef struct network_frame_s {
    unsigned int header;   //!< The header flags
    unsigned int seq;      //!< The sequence number
    unsigned int uid;
    network_packet_t data; //!< The packet for the NL
} network_frame_t;

void from_network_layer(network_packet_t *p, unsigned int *last_id);
void to_network_layer(network_packet_t p);

void killNetworkSingleton();

#define REMOTE_HOST_STR_SZ 64
#define BIND_HOST_STR_SZ 64

/*!
 * \brief Networking Singleton class
 */
class Network {
public:
    /*!
     * \brief Get the Singleton or create it on first use
     * \returns Singleton of type Network
     */
    static Network *Instance();

    /*!
     * \brief Deconstructs an object of Network
     */
    ~Network();

    network_frame_t &getPiggyBack();

    unsigned int getUID();

    int getPort();

    void setDebug(bool toggle);

    void setPort(unsigned int port);

    void setBindHost(char *s);

    void setRemoteHost(char *s);

    void killServerThread();

    void killClientThread();

    void spawnServerThread();

    void spawnClientThread();

    int runServer();

    void runClient();

protected:
    /*!
     * \brief Constructs an object of Network
     */
    Network();

private:
    static Network *mInstance;            //!< Singleton use
    bool mSpawnedClient;
    bool mSpawnedServer;
    int mPort;
    char mRemoteHost[REMOTE_HOST_STR_SZ];
    char mBindHost[BIND_HOST_STR_SZ];
    bool mPiggyBack;
    bool mNetworkReliable;
    bool mKillClient;
    bool mKillServer;
    bool mDebug;
};

#endif
