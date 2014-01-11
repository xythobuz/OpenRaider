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
 *-- History ------------------------------------------------
 *
 * 2002.06.21:
 * Mongoose - Created
 ================================================================*/


#ifndef _NETWORK_H_
#define _NETWORK_H_

/*----------------------------------------------
 * The packet holds the data for the network
 *  layer.  This is encapsulated in the frame.
 *
 * data  - a short (32 char) string
 ---------------------------------------------*/
typedef struct network_packet_s
{
	char send;
	unsigned int port;
	unsigned int cid;
	char  data[32];
	float pos[3];
	float pitch;
	float yaw;
	int aframe;
	int bframe;
	int view_model;

	char gurantee; // TCP like

} network_packet_t;


/*----------------------------------------------
 * The frame holds the data for the datalink
 *  layer.  This goes over the line.
 *
 * header - The header flags
 * seq    - The sequence number
 * data   - The packet for the NL
 ---------------------------------------------*/
typedef struct network_frame_s
{
	unsigned int header;
	unsigned int seq;
	unsigned int uid;

	network_packet_t data;

} network_frame_t;


void from_network_layer(network_packet_t *p, unsigned int *last_id);
void to_network_layer(network_packet_t p);


void killNetworkSingleton();

#define REMOTE_HOST_STR_SZ 64
#define BIND_HOST_STR_SZ 64


class Network
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors ( Singleton )
	////////////////////////////////////////////////////////////

	static Network *Instance();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs the object of Network or
	 *        returns pointer to it if previously allocated
	 *
	 *-- History ------------------------------------------
	 *
	 * 2001.08.24:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~Network();
	/*------------------------------------------------------
	 * Pre  : Network object is allocated
	 * Post : Deconstructs an object of Network
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	network_frame_t &getPiggyBack();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	unsigned int getUID();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	float getRandom(float from, float to);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int getPort();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void setDebug(bool toggle);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.12.15:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setPort(unsigned int port);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setBindHost(char *s);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.10:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void setRemoteHost(char *s);
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void killServerThread();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void killClientThread();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void spawnServerThread();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void spawnClientThread();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	int runServer();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	void runClient();
	/*------------------------------------------------------
	 * Pre  :
	 * Post :
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.09.03:
	 * Mongoose - Created
	 ------------------------------------------------------*/

 protected:

	////////////////////////////////////////////////////////////
	// Constructors ( Singleton )
	////////////////////////////////////////////////////////////

	Network();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of Network
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.06.21:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Protected Mutators
	////////////////////////////////////////////////////////////

 private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	static Network *mInstance;      /* Singleton use */

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
