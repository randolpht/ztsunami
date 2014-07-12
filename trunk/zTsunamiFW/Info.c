#include "UART.h"
#include "Info.h"



/*******************************************************************************
InfoBanner

Displays program name and version to UART0
 *******************************************************************************/
void InfoBanner(void)
{
    /* Clear the Terminal */
    UARTTxMessageBuild(27);
    UARTTxMessageBuild('[');
    UARTTxMessageBuild('2');
    UARTTxMessageBuild('J');
    UARTTxMessageBuild(27);
    UARTTxMessageBuild('[');
    UARTTxMessageBuild('H');
    UART0Tx();

    /* Program Name and Version */
    UARTTxMessageBuild('z');
    UARTTxMessageBuild('T');
    UARTTxMessageBuild('s');
    UARTTxMessageBuild('u');
    UARTTxMessageBuild('n');
    UARTTxMessageBuild('a');
    UARTTxMessageBuild('m');
    UARTTxMessageBuild('i');
    UARTTxMessageBuild(' ');
    UARTTxMessageBuild('v');
    UARTTxMessageBuild(VERSION_MAJOR);
    UARTTxMessageBuild('.');
    UARTTxMessageBuild(VERSION_MINOR);
    UARTTxMessageBuild('\r');
    UARTTxMessageBuild('\n');
    UART0Tx();

    /* Creator */
    UARTTxMessageBuild('J');
    UARTTxMessageBuild('e');
    UARTTxMessageBuild('a');
    UARTTxMessageBuild('n');
    UARTTxMessageBuild('-');
    UARTTxMessageBuild('L');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('u');
    UARTTxMessageBuild('i');
    UARTTxMessageBuild('s');
    UARTTxMessageBuild(' ');
    UARTTxMessageBuild('B');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('u');
    UARTTxMessageBuild('r');
    UARTTxMessageBuild('d');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('n');
    UARTTxMessageBuild('\r');
    UARTTxMessageBuild('\n');
    UART0Tx();

    /* Contact Details */
    UARTTxMessageBuild('j');
    UARTTxMessageBuild('n');
    UARTTxMessageBuild('l');
    UARTTxMessageBuild('s');
    UARTTxMessageBuild('.');
    UARTTxMessageBuild('b');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('u');
    UARTTxMessageBuild('r');
    UARTTxMessageBuild('d');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('n');
    UARTTxMessageBuild('@');
    UARTTxMessageBuild('g');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('g');
    UARTTxMessageBuild('l');
    UARTTxMessageBuild('e');
    UARTTxMessageBuild('m');
    UARTTxMessageBuild('a');
    UARTTxMessageBuild('i');
    UARTTxMessageBuild('l');
    UARTTxMessageBuild('.');
    UARTTxMessageBuild('c');
    UARTTxMessageBuild('o');
    UARTTxMessageBuild('m');
    UARTTxMessageBuild('\r');
    UARTTxMessageBuild('\n');
    UART0Tx();
}
