#include "Util.h"
#include "UART.h"
#include "Types.h"



/*******************************************************************************
UtilU8ToHumanHex

 *******************************************************************************/
void UtilU8ToHumanHex(unsigned char U8in)
{
    unsigned char LowerHalfByte = 0;
    unsigned char UpperHalfByte = 0;

    UARTTxMessageBuild('0');
    UARTTxMessageBuild('x');

    UpperHalfByte = (U8in >> 4) & 0x0F;
    if(UpperHalfByte < 10)
    {
        UARTTxMessageBuild(UpperHalfByte + ASCII_0);
    }
    else
    {
        UARTTxMessageBuild(UpperHalfByte - 10 + ASCII_A);
    }

    LowerHalfByte = U8in & 0x0F;
    if(LowerHalfByte < 10)
    {
        UARTTxMessageBuild(LowerHalfByte + ASCII_0);
    }
    else
    {
        UARTTxMessageBuild(LowerHalfByte - 10 + ASCII_A);
    }

    UARTTxMessageBuild(' ');
    UART0Tx();
}



/*******************************************************************************
UtilU8ToHumanDec

 *******************************************************************************/
void UtilU8ToHumanDec(unsigned char U8in) //TODO bit to zero fill or not
{
    unsigned char Temp = 0;

    Temp = U8in;
    if((Temp / 100) > 0)
    {
        UARTTxMessageBuild((Temp / 100) + ASCII_0);
        Temp = Temp - ((Temp / 100) * 100);
    }
    else
    {
        UARTTxMessageBuild('0');
    }

    if((Temp / 10) > 0)
    {
        UARTTxMessageBuild((Temp / 10) + ASCII_0);
        Temp = Temp - ((Temp / 10) * 10);
    }
    else
    {
        UARTTxMessageBuild('0');
    }

    UARTTxMessageBuild(Temp + ASCII_0);
    UART0Tx();
}

//TODO HumanHexToU8
