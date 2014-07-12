#include "CC1110.h"            /* Provided by Keil */
#include "CC1110Supplement.h"
#include "CC1110DKMini.h"
#include "Radio.h"
#include "UART.h"
#include "Util.h"
#include "Info.h"
#include "Types.h"

//TODO Circular buffer for RadioIn, UART0out data
//TODO Watchdog
//TODO use IRQ or DMA
//TODO display RSSI



/*******************************************************************************
main

Software entry point

Purpose of this software is to listen & inject Z-Wave packet
Is essential a bidirectional Serial to Z-Wave bridge
Can be used to either be part of a home automation system (my case),
or create havoc. USE RESPONSIBLY.

Requires a CC1110DK
Compiled with Keil c51 v952

 *******************************************************************************/
void main(void)
{
    unsigned char Idx = 0; /* General For Loop Counter */

    CLKCON = 0x00; /* Select the High speed crystal oscillator */
    while((CLKCON & OSC) != 0)
    {
        /* Wait for change to take effect */
    }

    /* Configure Pins */
    P0SEL = (PX_5_PERIPHERAL | PX_4_PERIPHERAL | PX_3_PERIPHERAL | PX_2_PERIPHERAL); /* Schematics of the mini devkit user guide p25 shows the serial port uses: P0_3 as Tx, P0_2 as Rx */
    P1DIR = (PX_1_OUT | PX_0_OUT); /* Schematics of the mini devkit user guide p25 show Green LED uses P1_0 and the Red LED P1_1 */

    UART0Init(BAUD_115200); /* Configures UART0 */
    InfoBanner();

    RadioInit(); /* Configure the TI chip Radio */

    /* UART RX Test */
    //UARTRxByteCount = 5; //wait for 5 bytes
    //UART0Rx();
    //for(Idx = 0; Idx < UARTRxByteCount ; Idx++)
    //{
    //    UARTTxMessageBuild(UARTRxBytes[Idx] + 1); /* Print the next char for a laugth */
    //}
    //UART0Tx();

    /* Dummy Data to test Radio Tx */
    RadioTxMessageBuild(0x01);
    RadioTxMessageBuild(0x23);
    RadioTxMessageBuild(0x45);
    RadioTxMessageBuild(0x67);
    RadioTxMessageBuild(0x89);
    /*
    RadioTxMessageBuild(0xAB);
    RadioTxMessageBuild(0xCD);
    RadioTxMessageBuild(0xEF);
    RadioTxMessageBuild(0x01);
    RadioTxMessageBuild(0x23);
    RadioTxMessageBuild(0x45);
    RadioTxMessageBuild(0x67);
    RadioTxMessageBuild(0x89);
    RadioTxMessageBuild(0xAB);
    RadioTxMessageBuild(0xCD);
    RadioTxMessageBuild(0xEF);
    RadioTxMessageBuild(0x01);
    RadioTxMessageBuild(0x23);
    RadioTxMessageBuild(0x45);
    RadioTxMessageBuild(0x67);
    RadioTxMessageBuild(0x89);
    */

    while(1) /* Forever Loop */
    {
        MARCSTATECurrent = MARCSTATE;

        /* LED Controls */
        PinLEDGreen = (MARCSTATECurrent == RX) ? 1 : 0 ; /* When radio receives, Green LED should be lit */
        PinLEDRed =   (MARCSTATECurrent == TX) ? 1 : 0 ; /* When radio trasmits,   Red LED should be lit */

        switch(MARCSTATECurrent)
        {
            case IDLE:
                if(RadioRxBytesCount > 0)
                {
                    /* Print the number of radio bytes received */
                    UARTTxMessageBuild('R');
                    UARTTxMessageBuild('x');
                    UARTTxMessageBuild('B');
                    UARTTxMessageBuild(' ');
                    UARTTxMessageBuild('=');
                    UARTTxMessageBuild(' ');
                    UART0Tx();

                    UtilU8ToHumanDec(RadioRxBytesCount);

                    UARTTxMessageBuild('\r');
                    UARTTxMessageBuild('\n');
                    UART0Tx();

                    /* Print the received radio bytes */
                    UARTTxMessageBuild('R');
                    UARTTxMessageBuild('x');
                    UARTTxMessageBuild('D');
                    UARTTxMessageBuild(' ');
                    UARTTxMessageBuild('=');
                    UARTTxMessageBuild(' ');
                    UART0Tx();

                    for(Idx = 0; Idx < RadioRxBytesCount; Idx++)
                    {
                        UtilU8ToHumanHex(RadioRxBytes[Idx]);
                    }

                    RadioRxBytesCount = 0; /* Reset Radio Data IN counter */
                    UARTTxMessageBuild('\r');
                    UARTTxMessageBuild('\n');
                    UART0Tx();
                }

                RadioTxBytesCountOut = 0; /* Reset Radio Data OUT counter */

                /* Always go back to Rx Mode by Default, unless master switch is pushed */
                if(PinSwitchMaster == 0)
                {
                    PKTLEN = RadioTxBytesCountIn;
                    RFST = STX; /* Enter Radio TX state by issuing an SRX strobe command */
                }
                else
                {
                    PKTLEN = 25;
                    RFST = SRX; /* Enter Radio RX state by issuing an SRX strobe command */
                }

                break;

            case RX:
                if(RFTXRXIF != 0)
                {
                    RFTXRXIF = 0; /* Specs say to clear fag  BEFORE reading (p188) */
                    RadioRxBytes[RadioRxBytesCount] = ~RFD;
                    RadioRxBytesCount++;
                }
                break;

            case RX_OVERFLOW:
                RFST = SIDLE; /* Return to Idle state to continue */
                break;

            case TX:
                /* Print Radio State Info */
                UARTTxMessageBuild('S');
                UARTTxMessageBuild('t');
                UARTTxMessageBuild('M');
                UARTTxMessageBuild(' ');
                UARTTxMessageBuild('=');
                UARTTxMessageBuild(' ');
                UART0Tx();

                UtilU8ToHumanHex(MARCSTATECurrent);

                UARTTxMessageBuild('\r');
                UARTTxMessageBuild('\n');
                UART0Tx();

                while(RadioTxBytesCountOut < RadioTxBytesCountIn)
                {
                    if(RFTXRXIF != 0)
                    {
                        RFD = ~RadioTxBytes[RadioTxBytesCountOut];
                        RFTXRXIF = 0;
                        RadioTxBytesCountOut++;
                    }
                }
                //RadioTxBytesCountIn = 0; /* Reset the Radio TX Data IN index */
                RFST = SIDLE; /* Return to Idle state to continue */
                break;

            case TX_UNDERFLOW:
                    UARTTxMessageBuild('S');
                    UARTTxMessageBuild('t');
                    UARTTxMessageBuild('M');
                    UARTTxMessageBuild(' ');
                    UARTTxMessageBuild('=');
                    UARTTxMessageBuild(' ');
                    UART0Tx();

                    UtilU8ToHumanHex(MARCSTATECurrent);

                    UARTTxMessageBuild('\r');
                    UARTTxMessageBuild('\n');
                    UART0Tx();
                RFST = SIDLE; /* Return to Idle state to continue */
                break;

            default:
                /* Push both buttons at the same time to leave a locked unknown current state without a reset */
                if((PinSwitchMaster == 0) && (PinSwitchSlave == 0))
                {
                    /* Print Radio State Info */
                    UARTTxMessageBuild('S');
                    UARTTxMessageBuild('t');
                    UARTTxMessageBuild('M');
                    UARTTxMessageBuild(' ');
                    UARTTxMessageBuild('=');
                    UARTTxMessageBuild(' ');
                    UART0Tx();

                    UtilU8ToHumanHex(MARCSTATECurrent);

                    UARTTxMessageBuild('\r');
                    UARTTxMessageBuild('\n');
                    UART0Tx();
                    RFST = SIDLE; /* Return to Idle state to continue */
                }
                break;
        }

        MARCSTATEPrevious = MARCSTATECurrent;
    }
}
