/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : UnRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Network
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments:
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.06.21:
 * Mongoose - Created
 =================================================================*/

#include <Network.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

extern network_frame_t gPiggyBack;

void from_network_layer(network_packet_t *p, unsigned int *last_id)
{
    static unsigned int i = 0;


    if (!p)
    {
        return;
    }

    *last_id = i++;

    sleep(1);

    p->send = 1;
    p->pos[0] = i*3;
    p->pos[1] = i*3+1;
    p->pos[2] = i*3+2;

    printf("<S>ending { %f %f %f }\n", p->pos[0], p->pos[1], p->pos[2]);
}


void to_network_layer(network_packet_t p)
{
    printf("<R>ecieved { %f %f %f }\n", p.pos[0], p.pos[1], p.pos[2]);

    gPiggyBack.data.pos[0] = gPiggyBack.seq*4;
    gPiggyBack.data.pos[1] = gPiggyBack.seq*4+1;
    gPiggyBack.data.pos[2] = gPiggyBack.seq*4+2;
    gPiggyBack.data.send = 1;
    gPiggyBack.data.yaw = 90.0f;
}


int main(int argc, char *argv[])
{
    printf("\n\n[Network class test]\n");
    Network &test = *Network::Instance();


    if (argc > 3)
    {
        if (argv[1][1] == 'v')
        {
            test.setDebug(true);
        }

        switch (argv[1][0])
        {
            case 'c':
                test.setRemoteHost(argv[2]);
                test.setPort(atoi(argv[3]));
                test.runClient();
                break;
            case 's':
                test.setBindHost(argv[2]);
                test.setPort(atoi(argv[3]));
                test.runServer();
                break;
            default:
                printf("Error in command line, run %s for help\n", argv[0]);
        }
    }
    else if (argc > 2)
    {
        test.setPort(atoi(argv[2]));
        test.runServer();
    }
    else
    {
        printf("Server: %s s [bind_host_name] port\n", argv[0]);
        printf("Client: %s c remote_host_name remote_host_port\n", argv[0]);
        printf("Append 'v' behind c/s option for verbose. eg cv\n");
    }

    killNetworkSingleton();

    return 0;
}

