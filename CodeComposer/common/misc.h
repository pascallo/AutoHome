#ifndef MISC_H
	#define MISC_H


	#include <stdint.h>

	#define UART_TX_BUFFER_SIZE 29

	void UARTInit(void);
	void UARTSendArray(char *TxArray, char ArrayLength);
	void ItoA( uint8_t z, char* Buffer );
	inline char *UARTReadLine();
	void printRoutingTable();
	int stringEqual(char *str1, char *str2);

#endif
