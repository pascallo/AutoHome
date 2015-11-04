//Add MASTER_NODE to global defines!



#include "msp430g2553.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/RFM73.h"

#include "../common/mesh.h"
#include "../common/misc.h"


void WriteToFHEM();
void initTimer();

char intStr[5];


void printReceivedPackets()
{
	char msgbf[40];
	char intStr[10];

	strcpy(msgbf, "RP: ");
	ItoA(receivedPacketCounter, intStr);
	strcpy(msgbf+4, intStr);
	strcpy(msgbf+4+strlen(intStr), "\n");
	UARTSendArray(msgbf, strlen(msgbf));

}




int main(void)
{
	 char *line;
	 uint16_t i;

	 WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT

	 initMesh(WriteToFHEM);
	 UARTInit();


	 LED(1);

	 while(1)
	 {

		 for(i = 0; i < RECEIVE_LOOP_CYCLES; i++) //wait for 20s and check for received data
		 {

			 if(i%SENSOR_SEND_INTERVAL == 0) //print routing table every five seconds
			 {
				 printRoutingTable();
			 }

			 line = UARTReadLine();

			 if(line[0] == 'T') // print Routing Table
			 {
				 printRoutingTable();
			 }
			 else if(line[0] == 'S') // Send Packet
			 {

				 UINT8 receiverAddr = line[1];
				 UINT8 length = line[2];

				 if(length+3 < UART_TX_BUFFER_SIZE)
				 {
					 if(sendReliable(receiverAddr, length, (UINT8 *)&line[3]) == 1)
						 UARTSendArray("S! ", 3);
					 else
						 UARTSendArray("F! ", 3);

					 ItoA(receiverAddr, intStr);
					 UARTSendArray(intStr, strlen(intStr));  //indicate which receiver failed for easy asynchronous processing

					 UARTSendArray("\n", 1);
				 }
			 }
			 else if(line[0] == 'I') // get Info for Device
			 {
				 sendDeviceInfoRequest(line[1]);
				 UARTSendArray("S!\n", 3);
			 }

			 RECEIVE_LOOP_DELAY();	//Do not Receive too often
			// receivePacket();
		 }


		 refreshRoutingTable();

	 }
}







void WriteToFHEM()
{
	UINT8 i;


	UINT8 senderAddress 	= rxBuffer[MSG_BYTE_SENDER_ADRESS];
	UINT8 msgType 			= rxBuffer[MSG_BYTE_LENGTH_AND_TYPE] & 0xE0;
	UINT8 viaAddress 		= rxBuffer[MSG_BYTE_VIA_ADRESS];
	UINT8 msgLength 		= rxBuffer[MSG_BYTE_LENGTH_AND_TYPE] & 0x1F;
	UINT8 hops 				= rxBuffer[MSG_BYTE_HOPS];
	UINT8 id 				= rxBuffer[MSG_BYTE_ID];



	// Print Format: Sender Via Type Length Hops ID : Data
	UARTSendArray("P ", 2);
	ItoA(senderAddress, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(" ", 1);

	ItoA(viaAddress, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(" ", 1);

	ItoA(msgType, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(" ", 1);

	ItoA(msgLength, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(" ", 1);

	ItoA(hops, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(" ", 1);

	ItoA(id, intStr);
	UARTSendArray(intStr, strlen(intStr));
	UARTSendArray(":", 1);


	for(i=0; i<msgLength; i++)
	{
		ItoA(rxBuffer[MSG_BYTE_DATA_START+i], intStr);
		UARTSendArray(intStr, strlen(intStr));
		UARTSendArray(" ", 1);
	}
	UARTSendArray("\n", 1);


}
