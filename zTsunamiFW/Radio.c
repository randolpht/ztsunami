#include  "CC1110.h"            /* Provided by Keil */
#include  "CC1110Supplement.h"
#include "Types.h"
#include "Radio.h"


unsigned char idata RadioRxBytes[RADIO_RX_BYTES_MAX];
unsigned char idata RadioRxBytesCount = 0;

unsigned char idata RadioTxBytes[RADIO_TX_BYTES_MAX];
unsigned char idata RadioTxBytesCountIn = 0;
unsigned char idata RadioTxBytesCountOut = 0;

unsigned char idata MARCSTATEPrevious = 0xFF; /* Invalid on purpose so first valid will be displayed */
unsigned char idata MARCSTATECurrent = 0xFE; /* Invalid on purpose so first valid will be displayed */



/*******************************************************************************
RadioInit

ZForce calculated values
The values we have used are as listed below
Note that PKTLEN (packet length) is variable in Z-Wave protocol
and marked as 0 in the list below.
This gets changed throughout the program depending on the state the receiver/transmitter is in.
If you just want to see if you capture something,
you should change it to some other value - maybe 10-20 - and start from there.
These are the values for EU 40 kbps frequency:

If a byte is received in the RFD register,
and it is not read before the next byte is received,
the radio will enter RX_OVERFLOW state and the RFIF.IRQ_RXOVF flag will be set together with RFIF.IRQ_DONE.

 *******************************************************************************/
void RadioInit(void)
{
    AGCCTRL2  = 0x43; // 0xDF17: AGCCTRL2 – AGC Control (0100 0011)
                      // 7:6 MAX_DVGA_GAIN[1:0]  01 The highest gain setting can not be used
                      // 5:3 MAX_LNA_GAIN[2:0]  000 Maximum possible LNA + LNA 2 gain
                      // 2:0 MAGN_TARGET[2:0]   011 These bits set the target value for the averaged amplitude from the digital channel filter (1 LSB = 0 dB). (33 dB)

    DEVIATN   = 0x35; // 0xDF11: DEVIATN – Modem Deviation Setting (0011 0101)
                      //   7 Not used
                      // 6:4 DEVIATION_E[2:0] 011 Deviation exponent
                      //   3 Not used
                      // 2:0 DEVIATION_M[2:0] 101 Sets fraction of symbol period used for phase change.

    FOCCFG    = 0x16; // 0xDF15: FOCCFG – Frequency Offset Compensation Configuration (0001 0110)
                      //   7 Not used
                      //   6 Reserved. Always write 0
                      //   5 FOC_BS_CS_GATE  0 If set, the demodulator freezes the frequency offset compensation and clock recovery feedback loops until the CARRIER_SENSE signal goes high
                      // 4:3 FOC_PRE_K[1:0] 10 The frequency compensation loop gain to be used before a sync word is detected. (3K)
                      //   2 FOC_POST_K      1 The frequency compensation loop gain to be used after a sync word is detected. (K/2)
                      // 1:0 FOC_LIMIT[1:0] 10 The saturation point for the frequency offset compensation algorithm (±BW CHAN / 4)

    FREQ0     = 0x66; // 0xDF0B: FREQ0 – Frequency Control Word, Low Byte

    FREQ1     = 0x66; // 0xDF0A: FREQ1 – Frequency Control Word, Middle Byte

    FREQ2     = 0x21; // 0xDF09: FREQ2 – Frequency Control Word, High Byte (0010 0001)
                      // 7:6 FREQ[23:22] FREQ[23:22]
                      // 5:0 FREQ[21:16] FREQ[23:0] is the base frequency for the frequency synthesizer in increments of fRef /216.

    FSCAL0    = 0x1F; // 0xDF1F: FSCAL0 – Frequency Synthesizer Calibration (0001 1111)
                      //   7 Not used
                      // 6:0 FSCAL0[6:0] 0011111 Frequency synthesizer calibration control. The value to use in this register is given by the SmartRF® Studio software

    FSCAL1    = 0x00; // 0xDF1E: FSCAL1 – Frequency Synthesizer Calibration
                      // 7:6 Not used
                      // 5:0 FSCAL1[5:0] Frequency synthesizer calibration result register.

    FSCAL2    = 0x2A; // 0xDF1D: FSCAL2 – Frequency Synthesizer Calibration (0010 1010)
                      // 7:6 Not used
                      //   5 VCO_CORE_H_EN     1 Select VCO (high)
                      // 4:0 FSCAL2[4:0]   01010 Frequency synthesizer calibration result register

    FSCAL3    = 0xE9; // 0xDF1C: FSCAL3 – Frequency Synthesizer Calibration (1110 1001)
                      // 7:6 FSCAL3[7:6]            11 Frequency synthesizer calibration configuration. The value to write in this register before calibration is given by the SmartRF® Studio software
                      // 5:4 CHP_CURR_CAL_EN[1:0]   10 Disable charge pump calibration stage when 0
                      // 3:0 FSCAL3[3:0]          1001 Frequency synthesizer calibration result register

    FSCTRL1   = 0x06; // 0xDF07: FSCTRL1 – Frequency Synthesizer Control (0000 0110)
                      // 7:6 Not used
                      //   5 Reserved
                      // 4:0 FREQ_IF[4:0] 0110 The desired IF frequency to employ in RX. Subtracted from FS base frequency in RX and controls the digital complex mixer in the demodulator.

    LQI       = 0x7F; // 0xDF39: LQI – Demodulator Estimate for Link Quality (0111 1111)
                      //   7 CRC_OK             0
                      // 6:0 LQI_EST[6:0] 1111111 The Link Quality Indicator estimates how easily a received signal can be demodulated. Calculated over the 64 symbols following the sync word.

    MCSM0     = 0x18; // 0xDF14: MCSM0 – Main Radio Control State Machine Configuration (0001 1000)
                      // 7:6 Not used
                      // 5:4 FS_AUTOCAL[1:0]   01 Select calibration mode, when to calibrate (When going from IDLE to RX or TX (or FSTXON))
                      //   3 Reserved.          1 Refer to SmartRF® Studio software [9] for settings
                      //   2 Reserved.          0 Refer to SmartRF® Studio software [9] for settings
                      // 1:0 CLOSE_IN_RX[1:0] 0 Sets RX attenuation. Used in order to avoid saturation in RX when two or more chips are close (within ~3 m). (0dB)

    MDMCFG1   = 0x72; // 0xDF0F: MDMCFG1 – Modem Configuration (0111 0010)
                      //   7 FEC_EN              0 Enable Forward Error Correction (FEC) with interleaving for packet payload. FEC is only supported for fixed packet length mode, i.e. PKTCTRL0.LENGTH_CONFIG=0
                      // 6:4 NUM_PREAMBLE[2:0] 111 Sets the minimum number of preamble bytes to be transmitted (24)
                      // 3:2 Not used
                      // 1:0 CHANSPC_E[1:0]     10 2 bit exponent of channel spacing

    MDMCFG2   = 0x06; // 0xDF0E: MDMCFG2 – Modem Configuration (0000 0110)
                      //   7 DEM_DCFILT_OFF    0 Disable digital DC blocking filter before demodulator. (Enabled)
                      // 6:4 MOD_FORMAT[2:0] 000 The modulation format of the radio signal (2-FSK)
                      //   3 MANCHESTER_EN     0 Manchester encoding/decoding enable (Disable)
                      // 2:0 SYNC_MODE[2:0]  110 Sync-word qualifier mode. (16/16 + carrier-sense above threshold)

    MDMCFG3   = 0x93; // 0xDF0D: MDMCFG3 – Modem Configuration: The mantissa of the user specified symbol rate.

    MDMCFG4   = 0xCA; // 0xDF0C: MDMCFG4 – Modem configuration (1100 1010)
                      // 7:6 CHANBW_E[1:0]
                      // 5:4 CHANBW_M[1:0] Sets the decimation ratio for the delta-sigma ADC input stream and thus the channel bandwidth.
                      // 3:0 DRATE_E[3:0] The exponent of the user specified symbol rate.

    PA_TABLE0 = 0x50; // 0xDF2E: PA_TABLE0 – PA Power Setting 0: Power amplifier output power setting 0

    PKTCTRL0  = 0x00; // 0xDF04: PKTCTRL0 – Packet Automation Control (0000 0000)
                      //   7 Not used            0
                      //   6 WHITE_DATA          0 Whitening enable. (Disabled)
                      // 5:4 PKT_FORMAT[1:0]    00 Packet format of RX and TX data (Normal mode)
                      //   3 CC2400_EN           0 CC2400 support enable. (Disabled)
                      //   2 CRC_EN              0 CRC calculation in TX and CRC check in RX enable (Disabled)
                      // 1:0 LENGTH_CONFIG[1:0] 00 Packet Length Configuration (Fixed packet length mode. Length configured in PKTLEN register)

    PKTCTRL1  = 0x00; // 0xDF03: PKTCTRL1 – Packet Automation Control
                      // 7:5 PQT[2:0]    000 Preamble quality estimator threshold.
                      // 4:3 Not used     00
                      //   2 APPEND_STATUS 0 When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as the CRC OK flag
                      // 1:0 ADR_CHK[1:0] 00 Controls address check configuration of received packages. (No address check)

    PKTLEN    = 0x14; // 0xDF02: PKTLEN – Packet Length: Indicates the packet length when fixed length packets are enabled. If variable length packets are used, this value indicates the maximum length packets allowed

    SYNC0     = 0x0F; // 0xDF01: SYNC0 – Sync Word, Low Byte:  8 LSB of 16-bit sync word
    SYNC1     = 0xAA; // 0xDF00: SYNC1 – Sync Word, High Byte: 8 MSB of 16-bit sync word

    TEST0     = 0x09; // 0xDF25: TEST0 – Various Test Settings (0000 1001)
                      // 7:2 TEST0[7:2]     000010 The value to use in this register is given by the SmartRF® Studio software [9].
                      //   1 VCO_SEL_CAL_EN      0 Enable VCO selection calibration stage when 1 (Disabled)
                      //   0 TEST0[0]            1 The value to use in this register is given by the SmartRF® Studio software [9].

    TEST1     = 0x35; // 0xDF24: TEST1 – Various Test Settings
                      // Always set this register to 0x31 when being in TX. At low data rates, the sensitivity can be improved by changing it to 0x35 in RX.

    TEST2     = 0x81; // 0xDF23: TEST2 – Various Test Settings
                      // At low data rates, the sensitivity can be improved by changing it to 0x35
}




/*******************************************************************************
RadioTxMessageBuild

 *******************************************************************************/
void RadioTxMessageBuild(unsigned char CharIn)
{
    RadioTxBytes[RadioTxBytesCountIn] = CharIn;
    RadioTxBytesCountIn++;
}
