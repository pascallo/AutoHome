#ifndef MESH_H
	#define MESH_H

	//defines
	#define MAX_ADRESS 						0x50
	#define BROADCAST_ADRESS				0x00
	#define MAX_HOP_COUNT					0x18
	#define RECEIVE_LOOP_DELAY()			DelayMs(1)
	#define RECEIVE_LOOP_CYCLES				10000
	#define SENSOR_SEND_INTERVAL			5000

	//message byte index
	#define MSG_BYTE_RECEIVER_ADRESS		0
	#define MSG_BYTE_SENDER_ADRESS			1
	#define MSG_BYTE_LENGTH_AND_TYPE		2
	#define MSG_BYTE_VIA_ADRESS				3
	#define MSG_BYTE_LAST_HOP				4
	#define MSG_BYTE_HOPS					5
	#define MSG_BYTE_ID						6
	#define MSG_BYTE_DATA_START				7
	#define MSG_HEADER_SIZE MSG_BYTE_DATA_START

	//message type
	#define MSG_TYPE_INIT_HELLO 			0x20
	#define MSG_TYPE_ROUTING_TABLE			0x40
	#define MSG_TYPE_DATA					0x60
	#define MSG_TYPE_ASK_FOR_DEVICE_INFO	0x80
	#define MSG_TYPE_DEVICE_INFO			0xA0
	#define MSG_TYPE_ACK					0xC0
	#define MSG_TYPE_RELIABLE_DATA			0xE0

	#define MSG_TYPE(typelength)	(typelength & 0xE0)

	//includes
	#include "RFM73.h"
	#include "config.h"

	//variables
	UINT8 rxBuffer[MAX_PACKET_LEN];



	//routing table
	UINT8 routingTable[MAX_ADRESS][2];		//  routingTable[targetAdress][viaAdress/hops]
	#define TABLE_VIA_ADRESS	0
	#define TABLE_HOPS			1

	//functions
	UINT8 sendDeviceInfoRequest(UINT8 receiverAdress);
	UINT8 sendDeviceInfoReponse(UINT8 receiverAdress);
	void send(UINT8 receiverAdress, UINT8 msgLength, UINT8* msgData);
	void sendPacket(UINT8 type, UINT8 receiverAdress, UINT8 senderAdress, UINT8 msgType, UINT8 msgLength, UINT8 viaAdress, UINT8* msgData);
	void sendPacketForward(UINT8 type, UINT8 receiverAdress, UINT8 senderAdress, UINT8 msgType, UINT8 msgLength, UINT8 viaAdress, UINT8* msgData, UINT8 hops, UINT8 id);
	UINT8 sendReliable(UINT8 receiverAdress, UINT8 msgLength, UINT8* msgData);
	void refreshRoutingTable(void);

	inline void receivePacket(void);
	void processReceivedData(UINT8 length);

	void initMesh(void (*callBack)(void));
	void updateTableFromRxBuffer();
	void sendRoutingTable();

	extern UINT8 receivedPacketCounter;
	extern UINT8 sentPacketCounter;




#endif
