#ifndef __INCradio
#define __INCradio

#define RADIO_RX_BYTES_MAX 40
#define RADIO_TX_BYTES_MAX 40

/* Radio Command Strobes */
#define SFSTXON      0x00
#define SCAL         0x01
#define SRX          0x02
#define STX          0x03
#define SIDLE        0x04
#define SNOP         0x05

/* Radio States */
#define SLEEP        0x00
#define IDLE         0x01
#define VCOON_MC     0x03
#define REGON_MC     0x04
#define MANCAL       0x05
#define VCOON        0x06
#define REGON        0x07
#define STARTCAL     0x08
#define BWBOOST      0x09
#define FS_LOCK      0x0A
#define IFADCON      0x0B
#define ENDCAL       0x0C
#define RX           0x0D
#define RX_END       0x0E
#define RX_RS        0x0F
#define TXRX_SWITCH  0x10
#define RX_OVERFLOW  0x11
#define FSTXON       0x12
#define TX           0x13
#define TX_END       0x14
#define RXTX_SWITCH  0x15
#define TX_UNDERFLOW 0x16

extern unsigned char idata RadioRxBytes[RADIO_RX_BYTES_MAX];
extern unsigned char idata RadioRxBytesCount;

extern unsigned char idata RadioTxBytes[RADIO_TX_BYTES_MAX];
extern unsigned char idata RadioTxBytesCountIn;
extern unsigned char idata RadioTxBytesCountOut;

extern unsigned char idata MARCSTATEPrevious;
extern unsigned char idata MARCSTATECurrent;

extern void RadioTxMessageBuild(unsigned char CharIn);

extern void RadioInit(void);

#else /* __INCradio */
#error "Nested/Repeated include file: Radio.h"
#endif /* __INCradio */