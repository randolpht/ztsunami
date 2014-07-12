#ifndef __INCuart
#define __INCuart

#define UART_RX_BYTES_MAX 40
#define UART_TX_BYTES_MAX 40

#define BAUD_2400    1
#define BAUD_9600    2
#define BAUD_57600   3
#define BAUD_115200  4
#define BAUD_230400  5


extern unsigned char idata UARTRxBytes[UART_RX_BYTES_MAX];
extern unsigned char idata UARTRxByteCount;

extern void UART0Init(unsigned char BaudRate);
extern void UART0Tx(void);
extern void UART0Rx(void);

extern void UARTTxMessageBuild(unsigned char CharIn);

#else /* __INCuart */
#error "Nested/Repeated include file: UART.h"
#endif /* __INCuart */