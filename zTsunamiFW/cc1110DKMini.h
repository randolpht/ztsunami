#ifndef __INCCC1110DKMini
#define __INCCC1110DKMini

sbit PinLEDGreen     = P1^0; /* Schematics of the mini devkit user guide p25 show Green LED     uses P1_0 */
sbit PinLEDRed       = P1^1; /* Schematics of the mini devkit user guide p25 show   Red LED     uses P1_1 */
sbit PinSwitchMaster = P1^2; /* Schematics of the mini devkit user guide p25 show Master Switch uses P1_2 */
sbit PinSwitchSlave  = P1^3; /* Schematics of the mini devkit user guide p25 show Slave  Switch uses P1_3 */

bit SwitchMasterStatus;
bit SwitchSlaveStatus;

#else /* __INCCC1110DKMini */
#error "Nested/Repeated include file: CC1110DKMini.h"
#endif /* __INCCC1110DKMini */