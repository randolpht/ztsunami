#ifndef __INCCC1110Supplement_H
#define __INCCC1110Supplement_H

/* CLKCON bits */
#define OSC          0x40

/* UxCSR bits */
#define MODE        0x80
#define RE          0x40
#define SLAVE       0x20
#define FE          0x10
#define ERR         0x08
#define RX_BYTE     0x04
#define TX_BYTE     0x02
#define ACTIVE      0x01

/* UxGCR bits */
#define CPOL       0x80
#define CPHA       0x40
#define ORDER      0x20

/* PERCFG bits */
#define T1CFG      0x40
#define T3CFG      0x20
#define T4CFG      0x10
#define U1CFG      0x02
#define U0CFG      0x01

#define PX_7_PERIPHERAL 0x80
#define PX_6_PERIPHERAL 0x40
#define PX_5_PERIPHERAL 0x20
#define PX_4_PERIPHERAL 0x10
#define PX_3_PERIPHERAL 0x08
#define PX_2_PERIPHERAL 0x04
#define PX_1_PERIPHERAL 0x02
#define PX_0_PERIPHERAL 0x01

#define PX_7_OUT 0x80
#define PX_6_OUT 0x40
#define PX_5_OUT 0x20
#define PX_4_OUT 0x10
#define PX_3_OUT 0x08
#define PX_2_OUT 0x04
#define PX_1_OUT 0x02
#define PX_0_OUT 0x01

#define  TEST0  XREG( 0xDF25 )  /* Various Test Settings */
#define  TEST1  XREG( 0xDF24 )  /* Various Test Settings */
#define  TEST2  XREG( 0xDF23 )  /* Various Test Settings */

#else /* __INCCC1110Supplement_H */
#error "Nested/Repeated include file: CC1110Supplement.h"
#endif /* __INCCC1110Supplement_H */