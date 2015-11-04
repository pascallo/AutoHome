/* Example of a Actor Node inside the Mesh Network
 *  - turns LED on and off
 * */

#include "msp430g2553.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common/mesh.h"
#include "../common/misc.h"



void ProcessReceivedData(void);


UINT8 send_current_state = 0x01;
UINT8 typeRequest = 0x00;

int main(void)
{
	uint16_t i;

	WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT

	initMesh(ProcessReceivedData);

	while(1)
	{

		for(i = 0; i < RECEIVE_LOOP_CYCLES; i++) //wait for 20s and check for received data
		{
			if(send_current_state != 0x00)
			{
				UINT8 resp[2];

				resp[0] = 0x01; //led status response
				resp[1] = LED_STATUS;

				sendReliable(send_current_state, 2, resp);
				send_current_state=0x00;

			}

			if(typeRequest != 0x00)
			{
				sendDeviceInfoReponse(typeRequest);
				typeRequest = 0x00;
			}

			RECEIVE_LOOP_DELAY();	//Do not Receive too often
			//receivePacket();
		}


		refreshRoutingTable();
	}
}




void ProcessReceivedData()
{

	UINT8 cmd = rxBuffer[MSG_BYTE_DATA_START];
	UINT8 value = rxBuffer[MSG_BYTE_DATA_START+1];

	if(MSG_TYPE(rxBuffer[MSG_BYTE_LENGTH_AND_TYPE]) == MSG_TYPE_ASK_FOR_DEVICE_INFO) //request for device info
	{
		typeRequest = rxBuffer[MSG_BYTE_SENDER_ADRESS];
	}

	if(cmd == 0x01)	// LED control cmd
	{
		LED(value); //turn LED on or off
	}

	send_current_state = rxBuffer[MSG_BYTE_SENDER_ADRESS];

}
