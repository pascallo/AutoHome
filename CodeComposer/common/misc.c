#include "msp430g2553.h"
#include <stdint.h>
#include "../common/mesh.h"

#include "misc.h"

char uartTxBuffer[UART_TX_BUFFER_SIZE];
uint8_t uartTxBufferPos = 0;

void UARTInit()
{
	 //set the internal oscillator at 1 MHz
	 BCSCTL1 = CALBC1_1MHZ; 	// Select lowest DCOx and MODx settings
	 DCOCTL = CALDCO_1MHZ;		// Set DCO to 1MHz

	 /* Configure hardware UART */
	 P1SEL = BIT1 + BIT2;		// P1.1 = RXD, P1.2=TXD
	 P1SEL2 = BIT1 + BIT2;		// P1.1 = RXD, P1.2=TXD
	 UCA0CTL1 |= UCSSEL_2;		// Use SMCLK
	 UCA0BR0 = 104;				// Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
	 UCA0BR1 = 0;				// Set baud rate to 9600 with 1MHz clock
	 UCA0MCTL = UCBRS0;			// Modulation UCBRSx = 1
	 UCA0CTL1 &= ~UCSWRST;		// Initialize USCI state machine
	 IE2 |= UCA0RXIE;			// Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled
}


void UARTSendArray(char *TxArray, char ArrayLength)
{
	while(ArrayLength--)
	{ // Loop until StringLength == 0 and post decrement
		 while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
		 UCA0TXBUF = *TxArray; //Write the character at the location specified py the pointer
		 TxArray++; //Increment the TxString pointer to point to the next character
	 }
}


UINT8 inline checkCompleteLineReceived()
{

	if(uartTxBufferPos != 0)
	{
		if(uartTxBuffer[uartTxBufferPos-1] == '\n' ) // received new data after newline -> overwrite old line
		{
			if( !(uartTxBuffer[0] == 'S' && (uartTxBufferPos < 2 || uartTxBufferPos < (uartTxBuffer[2]+3))) && // special cases for non-ascii data, "S": ignore \n bytes if position is less than length
				!(uartTxBuffer[0] == 'I' && uartTxBufferPos < 2 ) ) 					//  "I": min length 3
			{
				uartTxBufferPos = 0;
				return 1;
			}
		}
	}
	return 0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	checkCompleteLineReceived();

    uartTxBuffer[uartTxBufferPos] = UCA0RXBUF;
    uartTxBufferPos++;

    if(uartTxBufferPos >= UART_TX_BUFFER_SIZE) uartTxBufferPos = 0;
}


inline char *UARTReadLine()
{
	uartTxBuffer[UART_TX_BUFFER_SIZE-1] = 0;

	if(checkCompleteLineReceived())
		return uartTxBuffer;
	else return (char *)0;
}



void ItoA( uint8_t z, char* Buffer )
{
  UINT8 i = 0;
  UINT8 j;
  char tmp;
  unsigned u;    // In u bearbeiten wir den Absolutbetrag von z.

    // ist die Zahl negativ?
    // gleich mal ein - hinterlassen und die Zahl positiv machen
   /* if( z < 0 ) {
      Buffer[0] = '-';
      Buffer++;
      // -INT_MIN ist idR. größer als INT_MAX und nicht mehr
      // als int darstellbar! Man muss daher bei der Bildung
      // des Absolutbetrages aufpassen.
      u = ( (unsigned)-(z+1) ) + 1;
    }
    else {*/
      u = (unsigned)z;
  //  }
    // die einzelnen Stellen der Zahl berechnen
    do {
      Buffer[i++] = '0' + u % 10;
      u /= 10;
    } while( u > 0 );

    // den String in sich spiegeln
    for( j = 0; j < i / 2; ++j ) {
      tmp = Buffer[j];
      Buffer[j] = Buffer[i-j-1];
      Buffer[i-j-1] = tmp;
    }
    Buffer[i] = '\0';
}


void printRoutingTable()
{
	UARTSendArray("RT:", 3);

	char intStr[5];

	UINT8 i = 0;
	UINT8 currentAdress = 0;
	//Go throu all adresses in your routing table
	for(currentAdress = 0; currentAdress < MAX_ADRESS; currentAdress++)
	{
		//if adress is know, add to txBuffer
		if(routingTable[currentAdress][TABLE_HOPS] != 0xFF)
		{

			ItoA(currentAdress, intStr);
			UARTSendArray(intStr, strlen(intStr));
			UARTSendArray(" ", 1);

			ItoA(routingTable[currentAdress][TABLE_VIA_ADRESS], intStr);
			UARTSendArray(intStr, strlen(intStr));
			UARTSendArray(" ", 1);

			ItoA(routingTable[currentAdress][TABLE_HOPS], intStr);
			UARTSendArray(intStr, strlen(intStr));
			UARTSendArray("\t", 1);


			i = i + 2;
		}
	}
	UARTSendArray("\n", 1);
}


int stringEqual(char *str1, char *str2)
{
	UINT8 i;

	if(strlen(str1) != strlen(str2)) return 0;

	for(i=0; i<strlen(str1); i++)
	{
		if(str1[i] != str2[i]) return 0;
	}

	return 1;
}

