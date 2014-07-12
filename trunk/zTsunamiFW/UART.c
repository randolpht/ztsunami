#include  "CC1110.h"            /* Provided by Keil */
#include  "CC1110Supplement.h"
#include "UART.h"


/* Commonly used Baud Rate Settings for 26 MHz System Clock */
#define BAUD_M_26MHZ_2400    131
#define BAUD_E_26MHZ_2400      6
#define BAUD_M_26MHZ_9600    131
#define BAUD_E_26MHZ_9600      8
#define BAUD_M_26MHZ_57600    34
#define BAUD_E_26MHZ_57600    11
#define BAUD_M_26MHZ_115200   34
#define BAUD_E_26MHZ_115200   12
#define BAUD_M_26MHZ_230400   34
#define BAUD_E_26MHZ_230400   13


static unsigned char idata UARTTxBytes[UART_TX_BYTES_MAX];
static unsigned char idata UARTTxByteCount = 0;

unsigned char idata UARTRxBytes[UART_RX_BYTES_MAX];
unsigned char idata UARTRxByteCount = 0;



/*******************************************************************************
UART0Init

Setup the UART0
UART Docs in swra222b.pdf
// TODO drive UART by interrupt (p20 21)
 *******************************************************************************/
void UART0Init(unsigned char BaudRate)
{
    U0CSR = MODE | RE;
    switch(BaudRate)
    {
        case BAUD_2400:
            U0GCR = BAUD_E_26MHZ_2400;
            U0BAUD = BAUD_M_26MHZ_2400;
            break;
        case BAUD_9600:
            U0GCR = BAUD_E_26MHZ_9600;
            U0BAUD = BAUD_M_26MHZ_9600;
            break;
        case BAUD_57600:
            U0GCR = BAUD_E_26MHZ_57600;
            U0BAUD = BAUD_M_26MHZ_57600;
            break;
        case BAUD_115200:
            U0GCR = BAUD_E_26MHZ_115200;
            U0BAUD = BAUD_M_26MHZ_115200;
            break;
        case BAUD_230400:
            U0GCR = BAUD_E_26MHZ_230400;
            U0BAUD = BAUD_M_26MHZ_230400;
            break;
        default:
            break;
    }
}



/*******************************************************************************
UART0Tx

Sends byte by byte an array of byte
 *******************************************************************************/
void UART0Tx(void)
{
    unsigned char UARTTxIndex;
    UTX0IF = 0; /* Clear USART0 TX interrupt flag */
    for(UARTTxIndex = 0; UARTTxIndex < UARTTxByteCount; UARTTxIndex++)
    {
        U0DBUF = UARTTxBytes[UARTTxIndex];
        UARTTxBytes[UARTTxIndex] = 0; /* Erase Sent char */
        while(!UTX0IF)
        {
        }
        UTX0IF = 0; /* Clear USART0 TX interrupt flag */
    }
    UARTTxByteCount = 0; /* Reset counter */
}



/*******************************************************************************
UART0Rx

Receives bytes and store them in an array
 *******************************************************************************/
void UART0Rx(void)
{
    unsigned char UARTRxIndex;
    URX0IF = 0; /* Clear USART0 RX interrupt flag */
    for(UARTRxIndex = 0; UARTRxIndex < UARTRxByteCount; UARTRxIndex++)
    {
        while(!URX0IF)
        {
        }
        UARTRxBytes[UARTRxIndex] = U0DBUF;
        URX0IF = 0; /* Clear USART0 RX interrupt flag */
    }
}



/*******************************************************************************
UARTTxMessageBuild

 *******************************************************************************/
void UARTTxMessageBuild(unsigned char CharIn)
{
    UARTTxBytes[UARTTxByteCount] = CharIn;
    UARTTxByteCount++;
}

//TODO message clear