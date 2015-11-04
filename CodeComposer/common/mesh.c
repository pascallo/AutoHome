//includes
#include "mesh.h"
#include "stdlib.h"

UINT8 receivedPacketCounter = 0;
UINT8 sentPacketCounter = 1;
UINT8 lastAck = 0;
void (*callBackDataReceived)(void) = NULL;



/**************************************************
Function: sendDeviceInfoResponse
Description:
	sends a packet containing the type of the device
Parameter:
	receiverAdress
Return:
**************************************************/

UINT8 sendDeviceInfoReponse(UINT8 receiverAdress)
{
	if(receiverAdress > MAX_ADRESS) return 0;					//makes sure that we stay inside routingTable

	UINT8 i, d;
	UINT8 packetID = sentPacketCounter;
	lastAck = 0;
	unsigned char msgData = MY_TYPE;

	for(i = 0; i < 25; i++) //retry 25 times
	{

		sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
				receiverAdress,
				MY_ADRESS,
				MSG_TYPE_DEVICE_INFO,
				1,
				routingTable[receiverAdress][TABLE_VIA_ADRESS],
				&msgData,
				0, packetID);


		for(d = 0; d < 200; d++) //delay 200ms
		{
			RECEIVE_LOOP_DELAY();	// give ACK time to arrive
			if(!USE_INTERRUPT) receivePacket();
		}

		if(lastAck == packetID) return 1;
		DelayMs(rand()%101);	// wait 0-100ms before retry to avoid collisions
	}

	return 0;
}

/**************************************************
Function: sendDeviceInfoRequest
Description:
	sends a packet, requesting the type of the device
Parameter:
	receiverAdress
Return:
**************************************************/

UINT8 sendDeviceInfoRequest(UINT8 receiverAdress)
{
	if(receiverAdress > MAX_ADRESS) return 0;					//makes sure that we stay inside routingTable

	UINT8 i, d;
	UINT8 packetID = sentPacketCounter;
	lastAck = 0;

	for(i = 0; i < 25; i++) //retry 50 times
	{

		sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
				receiverAdress,
				MY_ADRESS,
				MSG_TYPE_ASK_FOR_DEVICE_INFO,
				0,
				routingTable[receiverAdress][TABLE_VIA_ADRESS],
				0,
				0, packetID);


		for(d = 0; d < 200; d++) //delay 200ms
		{
			RECEIVE_LOOP_DELAY();	// give ACK time to arrive
			if(!USE_INTERRUPT) receivePacket();
		}

		if(lastAck == packetID) return 1;
		DelayMs(rand()%101);	// wait 0-100ms before retry to avoid collisions
	}

	return 0;
}

/**************************************************
Function: sendReliable
Description:
	checks if packet reaches the receiver
	takes hop address from routing table and sends payload
Parameter:
	receiverAdress
	msgType
	msgLength: max 25 (byte)
	msgData: pointer to buffer (max 25 bytes)
Return:
	-1 if receiver couldn't be reached
	0 if packet was delivered
**************************************************/

UINT8 sendReliable(UINT8 receiverAdress,
		UINT8 msgLength,
		UINT8* msgData)

{
	UINT8 i, d;
	UINT8 packetID = sentPacketCounter;
	lastAck = 0;

	for(i = 0; i < 100; i++) //retry 100 times
	{

		sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
				receiverAdress,
				MY_ADRESS,
				MSG_TYPE_RELIABLE_DATA,
				msgLength,
				routingTable[receiverAdress][TABLE_VIA_ADRESS],
				msgData,
				0, packetID);


		for(d = 0; d < 200; d++) //delay 200ms
		{
			RECEIVE_LOOP_DELAY();	// give ACK time to arrive
			if(!USE_INTERRUPT) receivePacket();
		}

		if(lastAck == packetID) return 1;
		DelayMs(rand()%101);	// wait 0-100ms before retry to avoid collisions

	}

	return 0;
}

/**************************************************
Function: Send
Description:
	takes hop address from routing table and sends payload
Parameter:
	receiverAdress
	msgType
	msgLength: max 25 (byte)
	msgData: pointer to buffer (max 25 bytes)
Return:
	None
**************************************************/
void send(UINT8 receiverAdress,
		UINT8 msgLength,
		UINT8* msgData)

{

	sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
			receiverAdress,
			MY_ADRESS,
			MSG_TYPE_DATA,
			msgLength,
			routingTable[receiverAdress][TABLE_VIA_ADRESS],
			msgData,
			0, sentPacketCounter);
}


/**************************************************
Function: SendPacket
Description:
	fill FIFO to send a packet
Parameter:
	type: WR_TX_PLOAD or  W_TX_PAYLOAD_NOACK_CMD
	receiverAdress
	senderAdress
	msgType
	msgLength: max 25 (byte)
	viaAdress
	msgData: pointer to buffer (max 25 bytes)
Return:
	None
**************************************************/
void sendPacket(
		UINT8 type,
		UINT8 receiverAdress,
		UINT8 senderAdress,
		UINT8 msgType,
		UINT8 msgLength,
		UINT8 viaAdress,
		UINT8* msgData)
{


	sendPacketForward(type, receiverAdress, senderAdress, msgType, msgLength, viaAdress, msgData, 0, sentPacketCounter);
}

/**************************************************
Function: sendPacketForward
Description:
	fill FIFO to send a packet
Parameter:
	type: WR_TX_PLOAD or  W_TX_PAYLOAD_NOACK_CMD
	receiverAdress
	senderAdress
	msgType
	msgLength: max 25 (byte)
	viaAdress
	msgData: pointer to buffer (max 25 bytes)
	hops: number of previous hops
	id: unique identifier
Return:
	None
**************************************************/

void sendPacketForward(
		UINT8 type,
		UINT8 receiverAdress,
		UINT8 senderAdress,
		UINT8 msgType,
		UINT8 msgLength,
		UINT8 viaAdress,
		UINT8* msgData,
		UINT8 hops,
		UINT8 id)
{

	UINT8 msgLengthAndType;

	msgLength &= 0x1F;
	msgType &= 0xE0;
	//msgLengthAndType
	msgLengthAndType = (msgLength | msgType);  //first 3 bits define message type, last 5 bits define payload length

	//create message
	UINT8 buffer[MAX_PACKET_LEN];

	buffer[MSG_BYTE_RECEIVER_ADRESS] 	= receiverAdress;
	buffer[MSG_BYTE_SENDER_ADRESS] 		= senderAdress;
	buffer[MSG_BYTE_LENGTH_AND_TYPE] 	= msgLengthAndType;
	buffer[MSG_BYTE_VIA_ADRESS] 		= viaAdress;
	buffer[MSG_BYTE_ID] 				= id;
	buffer[MSG_BYTE_HOPS] 				= hops;


	int i;
	for(i = 0; i < msgLength; i++)
	{
		buffer[MSG_BYTE_DATA_START + i] = msgData[i];
	}

	buffer[MSG_BYTE_LAST_HOP] 	= MY_ADRESS;

	if(USE_INTERRUPT) DISABLE_INTERRUPT();
	SwitchToTxMode();

	// read register FIFO_STATUS's value
	UINT8 fifoStatus;
	fifoStatus = SPI_Read_Reg(FIFO_STATUS);

	//if not full, send data (write buff)
	if((fifoStatus & FIFO_STATUS_TX_FULL) == 0)
	{
		// write data to buffer
		SPI_Write_Buf(type, buffer, msgLength+MSG_BYTE_DATA_START);

		sentPacketCounter++;
		if(sentPacketCounter == 0) sentPacketCounter++; // skip zero (uninitialized value)
	}

	SwitchToRxMode();
	if(USE_INTERRUPT) ENABLE_INTERRUPT();
}


/**************************************************
Function: ReceivePacket
Description:
	read FIFO to read a packet
Parameter:
	None
Return:
	None
**************************************************/
inline void receivePacket(void)
{
	UINT8 length, status, fifoStatus;
	//UINT8 rxBuffer[MAX_PACKET_LEN];

	status = SPI_Read_Reg(STATUS);							// read register STATUS's value

	if((STATUS_RX_DR & status) == 0x40)						// if receive data ready (RX_DR) interrupt
	{

		if(USE_INTERRUPT) DISABLE_INTERRUPT();
		do
		{
			length = SPI_Read_Reg(R_RX_PL_WID_CMD);			// read len


			if(length <= MAX_PACKET_LEN)

 			{

 				SPI_Read_Buf(RD_RX_PLOAD, rxBuffer, length);// read receive payload from RX_FIFO buffer
 			}
 			else
 			{
 				SPI_Write_Reg(FLUSH_RX, 0);					//flush Rx
			}


			fifoStatus = SPI_Read_Reg(FIFO_STATUS);			// read register FIFO_STATUS's value

		}
		while((fifoStatus & FIFO_STATUS_RX_EMPTY) == 0); 	//while not empty


		processReceivedData(length);
		if(USE_INTERRUPT) ENABLE_INTERRUPT();

	}
	SPI_Write_Reg(WRITE_REG|STATUS, status);				// clear RX_DR or TX_DS or MAX_RT interrupt flag
}


/**************************************************
Function: initMesh
Description:
	clears the routing table; says hello to neighors; SwitchToRxMode
Parameter:
	callBack function if data packet received for this node
Return:
	None
**************************************************/
void initMesh(void (*callBack)(void))
{
	//Step 0: init rfm
	CE(0);
	CSN(1);
	SCK(0);
	MOSI(0);
	RFM73_Initialize();
	int i;

	//set callback function, if data is received
	callBackDataReceived = callBack;


	for(i = 0; i < MAX_ADRESS; i++)
	{
		routingTable[i][TABLE_VIA_ADRESS] 	= 0x00;
		routingTable[i][TABLE_HOPS] = 0xFF;
	}


	if(USE_INTERRUPT)
	{
	 ENABLE_INTERRUPT();
	 CLEAR_INTERRUPT();

	  __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled
	}


	//Step 1: Say Hello to neighbors
	sendPacket(W_TX_PAYLOAD_NOACK_CMD, BROADCAST_ADRESS, MY_ADRESS, MSG_TYPE_INIT_HELLO, 0x00, BROADCAST_ADRESS, 0x00);

	//wait for response
	SwitchToRxMode();

	if(!USE_INTERRUPT) receivePacket();

}

/**************************************************
Function: refreshRoutingTable
Description:
	clears the routing table; says hello to neighors; SwitchToRxMode
Parameter:
	callBack function if data packet received for this node
Return:
	None
**************************************************/

void refreshRoutingTable(void)
{

	sendRoutingTable();

	//init routing table
	if(USE_INTERRUPT) DISABLE_INTERRUPT();
	int i;
	for(i = 0; i < MAX_ADRESS; i++)
	{
		if(routingTable[i][TABLE_HOPS] != 0xFF)
			routingTable[i][TABLE_HOPS]++;			//delay invalidation of route -> more robust against lost packets

		if(routingTable[i][TABLE_HOPS] > MAX_HOP_COUNT)
		{
			routingTable[i][TABLE_VIA_ADRESS] 	= 0x00;
			routingTable[i][TABLE_HOPS] = 0xFF;
		}

	}
	if(USE_INTERRUPT) ENABLE_INTERRUPT();
}


/**************************************************
Function: processReceivedData
Description:
	decides what happens with received data. If receiver is this node, callback will be called
Parameter:
	length: length of rxBuffer (includes adresses, type, .. , data)
Return:
	None
**************************************************/
void processReceivedData(UINT8 length)
{
	UINT8 receiverAdress 	= rxBuffer[MSG_BYTE_RECEIVER_ADRESS];
	UINT8 msgType 			= rxBuffer[MSG_BYTE_LENGTH_AND_TYPE] & 0xE0;
	UINT8 viaAdress 		= rxBuffer[MSG_BYTE_VIA_ADRESS];

	receivedPacketCounter++;

	//response to 'Hello'
	if(msgType == MSG_TYPE_INIT_HELLO)
	{
		DelayMs(rand()%10);
		sendRoutingTable();
	}

	//resfresh routing table
	else if(msgType == MSG_TYPE_ROUTING_TABLE)
	{
		updateTableFromRxBuffer();
	}

	//you got a data packet
	else if(receiverAdress == MY_ADRESS)
	{

	/*	if(msgType == MSG_TYPE_ASK_FOR_DEVICE_INFO)
		{
			if(rxBuffer[MSG_BYTE_SENDER_ADRESS] > MAX_ADRESS) return;					//makes sure that we stay inside routingTable

			rxBuffer[MSG_BYTE_DATA_START] = MY_TYPE;									//reuse buffer

			sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
					rxBuffer[MSG_BYTE_SENDER_ADRESS],									//send to sender address
					MY_ADRESS,															//set own address as sender adress
					MSG_TYPE_DEVICE_INFO,												//device info type
					1,																	//length is one byte
					routingTable[rxBuffer[MSG_BYTE_SENDER_ADRESS]][TABLE_VIA_ADRESS],	//lookup route in routingTable
					&rxBuffer[MSG_BYTE_DATA_START],										//send back type of device
					0,																	//set number of hops to zero
					sentPacketCounter);													//use sentPacketCounter as ID

		}
		else*/
		if(msgType == MSG_TYPE_ACK) //got an ACK
		{
			lastAck = rxBuffer[MSG_BYTE_DATA_START]; 		//save id of acknowledged packet
		}
		else	//got Data
		{
			if(msgType == MSG_TYPE_RELIABLE_DATA || msgType == MSG_TYPE_ASK_FOR_DEVICE_INFO || msgType == MSG_TYPE_DEVICE_INFO) // check if ACK is needed
			{

				if(rxBuffer[MSG_BYTE_SENDER_ADRESS] > MAX_ADRESS) return;					//makes sure that we stay inside routingTable

				sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
						rxBuffer[MSG_BYTE_SENDER_ADRESS],									//send to sender address
						MY_ADRESS,															//set own address as sender adress
						MSG_TYPE_ACK,														//ACK Type
						1,																	//length is one byte
						routingTable[rxBuffer[MSG_BYTE_SENDER_ADRESS]][TABLE_VIA_ADRESS],	//lookup route in routingTable
						rxBuffer + MSG_BYTE_ID,												//send back original packet's id as payload
						0,																	//set number of hops to zero
						sentPacketCounter);													//use sentPacketCounter as ID
			}

			callBackDataReceived();															//process data in both cases (ACK, NOACK)
		}
	}

	//forward a packet
	else if(viaAdress == MY_ADRESS)
	{
		if(rxBuffer[MSG_BYTE_RECEIVER_ADRESS] > MAX_ADRESS) return;						//makes sure that we stay inside routingTable
		if(rxBuffer[MSG_BYTE_HOPS] > MAX_HOP_COUNT) return;										//prevent loops
		//if(rxBuffer[MSG_BYTE_LAST_HOP] == routingTable[rxBuffer[MSG_BYTE_RECEIVER_ADRESS]][TABLE_VIA_ADRESS]) return; //prevent loops

		rxBuffer[MSG_BYTE_HOPS]++;

		sendPacketForward(W_TX_PAYLOAD_NOACK_CMD,
				rxBuffer[MSG_BYTE_RECEIVER_ADRESS],										//don't change receiver adress
				rxBuffer[MSG_BYTE_SENDER_ADRESS],										//don't change sender adress
				rxBuffer[MSG_BYTE_LENGTH_AND_TYPE],										//don't change type
				rxBuffer[MSG_BYTE_LENGTH_AND_TYPE],										//don't change length
				routingTable[rxBuffer[MSG_BYTE_RECEIVER_ADRESS]][TABLE_VIA_ADRESS],		//take via adress from your own routing table
				rxBuffer + MSG_BYTE_DATA_START,											//don't change payload (it's the rxBuffer without the first 5 bytes)
				rxBuffer[MSG_BYTE_HOPS],												//increment number of hops
				rxBuffer[MSG_BYTE_ID]);													//keep original ID

	}
}


/**************************************************
Function: updateTableFromRxBuffer
Description:
	updates routing table
Parameter:
	None
Return:
	None
**************************************************/
void updateTableFromRxBuffer()
{
	UINT8 senderAdress = rxBuffer[MSG_BYTE_SENDER_ADRESS];
	UINT8 msgLength = rxBuffer[MSG_BYTE_LENGTH_AND_TYPE] & 0x1F;

	//first add the sender of the routing table as neighbor with distance/hops 1
	routingTable[senderAdress][TABLE_VIA_ADRESS] = senderAdress;
	routingTable[senderAdress][TABLE_HOPS] = 1;

	//then process the entries of the routing table
	int i;
	for(i = 0; i < msgLength; i = i + 2)
	{
		UINT8 targetNode = rxBuffer[MSG_BYTE_DATA_START + i];			//even elements are targets
		UINT8 targetHops = rxBuffer[MSG_BYTE_DATA_START + i + 1] + 1;	//odd elements are hops to target. +1 because of via

		if(targetNode == MY_ADRESS) continue;
		if(targetHops > MAX_HOP_COUNT) continue;
		if(targetNode <= MAX_ADRESS)	//valid adress?!
		{
			if(routingTable[targetNode][TABLE_HOPS] > targetHops)	//new and shorter rout
			{
				routingTable[targetNode][TABLE_VIA_ADRESS] 	= senderAdress;
				routingTable[targetNode][TABLE_HOPS] 		= targetHops;
			}
		}
	}
}


/**************************************************
Function: sendRoutingTable
Description:
	sends a routing table
Parameter:
	None
Return:
	None
**************************************************/
void sendRoutingTable()
{
	UINT8 i = 0;
	UINT8 currentAdress = 0;
	UINT8 txBuffer[MAX_PACKET_LEN];

	//Go throu all adresses in your routing table
	for(currentAdress = 1; currentAdress < MAX_ADRESS; currentAdress++) //skip broadcast
	{
		//if adress is know, add to txBuffer
		if(routingTable[currentAdress][TABLE_HOPS] != 0xFF)
		{
			txBuffer[i] = currentAdress; //target
			txBuffer[i + 1] = routingTable[currentAdress][TABLE_HOPS]; //hops
			i = i + 2;
			//if buffer is full, send packet
			if(i >= MAX_PACKET_LEN - MSG_HEADER_SIZE)
			{
				sendPacket(W_TX_PAYLOAD_NOACK_CMD, BROADCAST_ADRESS, MY_ADRESS, MSG_TYPE_ROUTING_TABLE, i, 0x00, txBuffer);
				i = 0;
			}
		}
	}
	sendPacket(W_TX_PAYLOAD_NOACK_CMD, BROADCAST_ADRESS, MY_ADRESS, MSG_TYPE_ROUTING_TABLE, i, 0x00, txBuffer);
}





#ifdef INTERRUPT_VECTOR

	// interrupt service routine
	#pragma vector=INTERRUPT_VECTOR
	__interrupt void receivePacketInterrupt(void)
	{
		 receivePacket();
		 CLEAR_INTERRUPT();
	}

#endif


