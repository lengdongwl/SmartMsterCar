#include "CRC.h"
/********************************************************************
CRCx
	CRC3 - GSM
	CRC3 - ROHC
	CRC4 - ITU
	CRC5 - ITU
	CRC5 - EPC
	CRC5 - USB
	CRC6 - ITU
	CRC6 - GSM
	CRC6 - DARC
	CRC7
	CRC7 - MMC
	CRC7 - UMTS
	CRC7 - ROHC
	CAN15
	CAN17
	CAN21
	CRC10 - ATM
	CRC13 - BBC
	MPT1327
	CDMA2000
CRC8
	INTEL
	BCC
	LRC
	MAXIM8
	ROHC
	ITU8
	CRC8
	SUM8
	FLETCHER8
	SMBUS
	AUTOSAR
	LTE
	SAE-J1850
	I-CODE
	GSM-A
	NRSC-5
	WCDMA
	BLUETOOTH
	DVB-S2
	EBU
	DARC
	MIFARE
	LIN1.3
	LIN2.x
CRC16
	MODBUS
	IBM
	XMODEM
	CCITT
	KERMIT
	MCRF4XX
	SICK
	DNP
	X25
	USB
	MAXIM16
	DECT(R/X)
	TCP/UDP
	CDMA2000
	FLETCHER16
	EPC16
	PROFIBUS
	BUYPASS
	GENIBUS
	GSM16
	RIELLO
	OPENSAFETY
	EN13757
	CMS
CRC24
	BLE
	OPENPGP
	LTE-A
	LTE-B
	OS9
	FLEXRAY-A
	FLEXRAY-B
	INTERLAKEN
	CRC24
CRC32
	FSC
	CRC32
	MPEG2
	ADLER32
	FLETCHER32
	POSIX
	BZIP2
	JAMCRC
	AUTOSAR
	C / ISCSI
	D / BASE91-D
	Q / AIXM
	XFER
	CKSUM
	XZ32
	AAL5
	ISO-HDLC
	PKZIP
	ADCCP
	V-42
	STM32
	CRC30-CDMA
	CRC31-PHILIPS
CRC64
	GO-ISO
	ECMA182
	WE
	XZ64
********************************************************************/
/*CAN_X*/
unsigned short hexin_crcx_reverse12(unsigned short data)
{
	unsigned int   i = 0;
	unsigned short t = 0;
	for (i = 0; i < 12; i++) {
		t |= ((data >> i) & 0x0001) << (11 - i);
	}
	return t;
}

static unsigned short hexin_crcx_reverse16(unsigned short data)
{
	unsigned int   i = 0;
	unsigned short t = 0;
	for (i = 0; i < 16; i++) {
		t |= ((data >> i) & 0x0001) << (15 - i);
	}
	return t;
}

static unsigned int hexin_crcx_compute_init_table(struct _hexin_crcx* param)
{
	unsigned int i = 0, j = 0;
	unsigned short crc = 0, c = 0;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = (unsigned short)i;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x0001)   crc = (crc >> 1) ^ param->poly;
				else                        crc = crc >> 1;
				c = c >> 1;
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned short)i) << 8;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x8000) crc = (crc << 1) ^ param->poly;
				else                      crc = crc << 1;
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

static unsigned short hexin_crcx_compute_char(unsigned short crcx, unsigned char c, struct _hexin_crcx* param)
{
	unsigned short crc = crcx;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x00FF & (unsigned short)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 8) ^ (0x00FF & (unsigned short)c)) & 0xFF];
	}

	return crc;
}

unsigned short hexin_crcx_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crcx* param, unsigned short init)
{
	unsigned int i = 0, result = 0;
	unsigned int offset = (HEXIN_CRCX_WIDTH - param->width);
	unsigned short crc = (init << offset);

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = (hexin_crcx_reverse16(param->poly) >> offset);
		}
		else {
			param->poly = (param->poly << offset);
		}
		param->is_initial = hexin_crcx_compute_init_table(param);
	}

	/* Fixed Issues #4  */
	if (HEXIN_REFIN_REFOUT_IS_TRUE(param) && (HEXIN_GRADUAL_CALCULATE_IS_TRUE(param))) {
		crc = init;
	}

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param) && (!HEXIN_GRADUAL_CALCULATE_IS_TRUE(param))) {
		crc = hexin_crcx_reverse16(crc);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crcx_compute_char(crc, pSrc[i], param);
	}

	result = (HEXIN_REFIN_REFOUT_IS_TRUE(param)) ? crc : (crc >> offset);

	return (result ^ param->xorout);
}

unsigned short CRC3_GSM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 3;
	CRCX.poly = 0x03;
	CRCX.init = 0x00;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x07;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC3_ROHC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 3;
	CRCX.poly = 0x03;
	CRCX.init = 0x07;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC4_ITU(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 4;
	CRCX.poly = 0x03;
	CRCX.init = 0x00;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC4_INTERLAKEN4(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 4;
	CRCX.poly = 0x03;
	CRCX.init = 0x0F;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x0F;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC5_ITU(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 5;
	CRCX.poly = 0x15;
	CRCX.init = 0x00;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC5_EPC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 5;
	CRCX.poly = 0x09;
	CRCX.init = 0x09;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC5_USB(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 5;
	CRCX.poly = 0x05;
	CRCX.init = 0x1F;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x1F;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC6_ITU(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 6;
	CRCX.poly = 0x03;
	CRCX.init = 0x00;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC6_GSM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 6;
	CRCX.poly = 0x2F;
	CRCX.init = 0x00;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x3F;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC6_DARC6(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 6;
	CRCX.poly = 0x19;
	CRCX.init = 0x00;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC7_MMC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 7;
	CRCX.poly = 0x09;
	CRCX.init = 0x00;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC7_UMTS7(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 7;
	CRCX.poly = 0x45;
	CRCX.init = 0x00;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC7_ROHC7(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 7;
	CRCX.poly = 0x4F;
	CRCX.init = 0x7F;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x00;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC10_ATM10(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 10;
	CRCX.poly = 0x233;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC10_CDMA2000(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 10;
	CRCX.poly = 0x3D9;
	CRCX.init = 0x3FF;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC10_GSM10(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 10;
	CRCX.poly = 0x175;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x3FF;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC11_FLEXRAY11(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 11;
	CRCX.poly = 0x385;
	CRCX.init = 0x01A;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC11_UMTS11(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 11;
	CRCX.poly = 0x307;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC12_CDMA2000(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 12;
	CRCX.poly = 0xF13;
	CRCX.init = 0xFFF;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC12_DECT12(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 12;
	CRCX.poly = 0x80F;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC12_GSM12(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 12;
	CRCX.poly = 0xD31;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0xFFF;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC12_UMTS12(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 12;
	CRCX.poly = 0x80F;
	CRCX.init = 0x000;
	CRCX.refin = FALSE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC13_BBC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 13;
	CRCX.poly = 0x1CF5;
	CRCX.init = 0x0000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x0000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC14_DARC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 14;
	CRCX.poly = 0x0805;
	CRCX.init = 0x0000;
	CRCX.refin = TRUE;
	CRCX.refout = TRUE;
	CRCX.xorout = 0x0000;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC14_GSM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 14;
	CRCX.poly = 0x202D;
	CRCX.init = 0x0000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x3FFF;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

unsigned short CRC15_MPT1327(char* parameter, unsigned char Szie)
{
	static struct _hexin_crcx CRCX;
	CRCX.is_initial = FALSE;
	CRCX.width = 15;
	CRCX.poly = 0x6815;
	CRCX.init = 0x0000;
	CRCX.refin = FALSE;
	CRCX.refout = FALSE;
	CRCX.xorout = 0x0001;
	CRCX.result = 0;
	return hexin_crcx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRCX, CRCX.init);
}

/********************************************************************/
/********************************************************************/
/*CAN_X*/
static unsigned int hexin_canx_reverse32(unsigned int data)
{
	unsigned int i = 0;
	unsigned int t = 0;
	for (i = 0; i < 32; i++) {
		t |= ((data >> i) & 0x00000001) << (31 - i);
	}
	return t;
}

static unsigned int hexin_canx_compute_init_table(struct _hexin_canx* param)
{
	unsigned int i = 0, j = 0;
	unsigned int crc = 0x00000000L;
	unsigned int c = 0x00000000L;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = i;
			for (j = 0; j < 8; j++) {
				if (crc & 0x00000001L) crc = (crc >> 1) ^ param->poly;
				else                     crc = (crc >> 1);
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned int)i) << 24;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x80000000L)  crc = (crc << 1) ^ param->poly;
				else                              crc = (crc << 1);
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

static unsigned int hexin_canx_compute_char(unsigned int crcx, unsigned char c, struct _hexin_canx* param)
{
	unsigned int crc = crcx;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 24) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}

	return crc;
}

unsigned int hexin_canx_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_canx* param, unsigned int init)
{
	unsigned int i = 0, result = 0;
	unsigned int crc = (init << (HEXIN_CANX_WIDTH - param->width));

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = (hexin_canx_reverse32(param->poly) >> (HEXIN_CANX_WIDTH - param->width));
		}
		else {
			param->poly = (param->poly << (HEXIN_CANX_WIDTH - param->width));
		}
		param->is_initial = hexin_canx_compute_init_table(param);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_canx_compute_char(crc, pSrc[i], param);
	}

	result = (HEXIN_REFIN_REFOUT_IS_TRUE(param)) ? crc : (crc >> (HEXIN_CANX_WIDTH - param->width));

	return (result ^ param->xorout);
}

unsigned short CANX_CAN15(char* parameter, unsigned char Szie)
{
	static struct _hexin_canx CRC_X;
	CRC_X.is_initial = FALSE;
	CRC_X.width = 15;
	CRC_X.poly = CAN15_POLYNOMIAL_00004599;
	CRC_X.init = 0x00000000L;
	CRC_X.refin = FALSE;
	CRC_X.refout = FALSE;
	CRC_X.xorout = 0x00000000L;
	CRC_X.result = 0;
	return hexin_canx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_X, CRC_X.init);
}

unsigned long CANX_CAN17(char* parameter, unsigned char Szie)
{
	static struct _hexin_canx CRC_X;
	CRC_X.is_initial = FALSE;
	CRC_X.width = 17;
	CRC_X.poly = CAN17_POLYNOMIAL_0001685B;
	CRC_X.init = 0x00000000L;
	CRC_X.refin = FALSE;
	CRC_X.refout = FALSE;
	CRC_X.xorout = 0x00000000L;
	CRC_X.result = 0;
	return hexin_canx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_X, CRC_X.init);
}

unsigned long CANX_CAN21(char* parameter, unsigned char Szie)
{
	static struct _hexin_canx CRC_X;
	CRC_X.is_initial = FALSE;
	CRC_X.width = 21;
	CRC_X.poly = CAN21_POLYNOMIAL_00102899;
	CRC_X.init = 0x00000000L;
	CRC_X.refin = FALSE;
	CRC_X.refout = FALSE;
	CRC_X.xorout = 0x00000000L;
	CRC_X.result = 0;
	return hexin_canx_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_X, CRC_X.init);
}
/********************************************************************/
/********************************************************************/
/*CRC_8*/
unsigned char hexin_reverse8(unsigned char data)
{
	unsigned int  i = 0;
	unsigned char t = 0;
	for (i = 0; i < 8; i++) {
		t |= ((data >> i) & 0x01) << (7 - i);
	}
	return t;
}

unsigned int hexin_crc8_init_table_poly_is_high(unsigned char polynomial, unsigned char* table)
{
	unsigned int i = 0, j = 0;
	unsigned char crc = 0, c = 0;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = i;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x01)   crc = (crc >> 1) ^ polynomial;
			else                      crc = crc >> 1;
			c = c >> 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned int hexin_crc8_init_table_poly_is_low(unsigned char polynomial, unsigned char* table)
{
	unsigned int i = 0, j = 0;
	unsigned char crc = 0, c = 0;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = i;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x80) crc = (crc << 1) ^ polynomial;
			else                    crc = crc << 1;
			c = c << 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned char hexin_crc8_poly_calc(unsigned char crc8, unsigned char c, const unsigned char* table)
{
	return table[crc8 ^ c];
}

unsigned char hexin_calc_crc8_bcc(const unsigned char* pSrc, unsigned int len, unsigned char crc8)
{
	unsigned int i = 0;
	unsigned char crc = crc8;

	for (i = 0; i < len; i++) {
		crc ^= pSrc[i];
	}

	return crc;
}

unsigned char hexin_calc_crc8_lrc(const unsigned char* pSrc, unsigned int len, unsigned char crc8)
{
	unsigned int i = 0;
	unsigned char crc = crc8;

	for (i = 0; i < len; i++) {
		crc += pSrc[i];
	}
	crc = (~crc) + 0x01;

	return crc;
}

unsigned char hexin_calc_crc8_sum(const unsigned char* pSrc, unsigned int len, unsigned char crc8)
{
	unsigned int i = 0;
	unsigned char crc = crc8;

	for (i = 0; i < len; i++) {
		crc += pSrc[i];
	}
	return crc;
}

unsigned char hexin_calc_crc8_fletcher(const unsigned char* pSrc, unsigned int len, unsigned char crc8 /*reserved*/)
{
	unsigned int i = 0;
	unsigned char sum1 = 0, sum2 = 0;

	for (i = 0; i < len; i++) {
		sum1 += pSrc[i];
		sum2 += sum1;
	}
	return (sum1 & 0xF) | (sum2 << 4);
}

static unsigned int hexin_crc8_compute_init_table(struct _hexin_crc8* param)
{
	unsigned int i = 0, j = 0;
	unsigned char crc = 0, c = 0;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = i;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x01)   crc = (crc >> 1) ^ param->poly;
				else                      crc = crc >> 1;
				c = c >> 1;
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = i;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x80) crc = (crc << 1) ^ param->poly;
				else                    crc = crc << 1;
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

static unsigned char hexin_crc8_compute_char(unsigned char crc8, unsigned char c, struct _hexin_crc8* param)
{
	return param->table[crc8 ^ c];
}

unsigned char hexin_crc8_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc8* param, unsigned char init)
{
	unsigned int i = 0;
	unsigned char crc = init;

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = hexin_reverse8(param->poly);
		}
		param->is_initial = hexin_crc8_compute_init_table(param);
	}
	/* Fixed Issues #4  */
	if (HEXIN_REFIN_REFOUT_IS_TRUE(param) && (!HEXIN_GRADUAL_CALCULATE_IS_TRUE(param))) {
		crc = hexin_reverse8(init);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crc8_compute_char(crc, pSrc[i], param);
	}

	return (crc ^ param->xorout);
}

const unsigned char LIN2X_PID_TABLE[60] = { 0x80, 0xC1, 0x42, 0x03, 0xC4, 0x85, 0x06, 0x47, 0x08, 0x49,
											0xCA, 0x8B, 0x4C, 0x0D, 0x8E, 0xCF, 0x50, 0x11, 0x92, 0xD3,
											0x14, 0x55, 0xD6, 0x97, 0xD8, 0x99, 0x1A, 0x5B, 0x9C, 0xDD,
											0x5E, 0x1F, 0x20, 0x61, 0xE2, 0xA3, 0x64, 0x25, 0xA6, 0xE7,
											0xA8, 0xE9, 0x6A, 0x2B, 0xEC, 0xAD, 0x2E, 0x6F, 0xF0, 0xB1,
											0x32, 0x73, 0xB4, 0xF5, 0x76, 0x37, 0x78, 0x39, 0xBA, 0xFB };

unsigned char hexin_calc_crc8_lin(const unsigned char* pSrc, unsigned int len, unsigned char crc8)
{
	unsigned int   i = 0;
	unsigned short sum = ((unsigned short)crc8) & 0x00FF;

	for (i = 1; i < len; i++) {
		sum += pSrc[i];
		sum = ((sum >> 8) & 0x00FF) + (sum & 0x00FF);
	}
	return (0xFF - sum);
}

unsigned char hexin_crc8_get_lin2x_pid(const unsigned char id)
{
	unsigned char p0 = 0, p1 = 0;

	p0 = (((id & 0x01) >> 0) ^ ((id & 0x02) >> 1) ^ ((id & 0x04) >> 2) ^ ((id & 0x10) >> 4));
	p1 = ~(((id & 0x02) >> 1) ^ ((id & 0x08) >> 3) ^ ((id & 0x10) >> 4) ^ ((id & 0x20) >> 5));

	return ((p1 & 0x01) << 7) | ((p0 & 0x01) << 6) | id;
}

unsigned char hexin_calc_crc8_lin2x(const unsigned char* pSrc, unsigned int len, unsigned char crc8)
{
//	unsigned char crc = crc8;
	unsigned char id = pSrc[0];

	/*
	 * Frames with identifiers 0x3C and 0x3D can only use classic check,
	 * these two groups of frames are LIN diagnostic frames.
	 */
	if ((id == 0x3C) || (id == 0x3D)) {
		return hexin_calc_crc8_lin(pSrc, len, 0);
	}
	return hexin_calc_crc8_lin(pSrc, len, hexin_crc8_get_lin2x_pid(id));
}

unsigned char CRC8_MAXMI(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_31;
	CRC8.init = 0x00;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_ROHC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_07;
	CRC8.init = 0xFF;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_ITU(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_07;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x55;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_CRC8(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_07;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_HACKER(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_31;
	CRC8.init = 0xFF;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}


unsigned char CRC8_AUTOSAR8(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_2F;
	CRC8.init = 0xFF;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0xFF;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_LTE8(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_9B;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_WCDMA(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_9B;
	CRC8.init = 0x00;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_SAE_J1850(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_1D;
	CRC8.init = 0xFF;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0xFF;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_ICODE(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_1D;
	CRC8.init = 0xFD;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_GSM8_A(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_1D;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_GSM8_B(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_49;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0xFF;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_NRSC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_31;
	CRC8.init = 0xFF;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_BLUETOOTH(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_A7;
	CRC8.init = 0x00;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_DVB_S2(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_D5;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_EBU8(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_1D;
	CRC8.init = 0xFF;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_DARC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_39;
	CRC8.init = 0x00;
	CRC8.refin = TRUE;
	CRC8.refout = TRUE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_OPENSAFETY8(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_2F;
	CRC8.init = 0x00;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned char CRC8_MAD(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc8 CRC8;
	CRC8.is_initial = FALSE;
	CRC8.width = HEXIN_CRC8_WIDTH;
	CRC8.poly = CRC8_POLYNOMIAL_1D;
	CRC8.init = 0xC7;
	CRC8.refin = FALSE;
	CRC8.refout = FALSE;
	CRC8.xorout = 0x00;
	CRC8.result = 0;
	return hexin_crc8_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC8, CRC8.init);
}

unsigned short hexin_reverse16(unsigned short data)
{
	unsigned int   i = 0;
	unsigned short t = 0;
	for (i = 0; i < 16; i++) {
		t |= ((data >> i) & 0x0001) << (15 - i);
	}
	return t;
}

unsigned int hexin_crc16_init_table_poly_is_high(unsigned short polynomial, unsigned short* table)
{
	unsigned int i = 0, j = 0;
	unsigned short crc = 0, c = 0;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = (unsigned short)i;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x0001)   crc = (crc >> 1) ^ polynomial;
			else                        crc = crc >> 1;
			c = c >> 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned int hexin_crc16_init_table_poly_is_low(unsigned short polynomial, unsigned short* table)
{
	unsigned int i = 0, j = 0;
	unsigned short crc = 0, c = 0;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = ((unsigned short)i) << 8;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x8000) crc = (crc << 1) ^ polynomial;
			else                      crc = crc << 1;
			c = c << 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned short hexin_crc16_poly_is_high_calc(unsigned short crc16, unsigned char c, const unsigned short* table)
{
	unsigned short crc = crc16;
	unsigned short tmp, short_c;

	short_c = 0x00FF & (unsigned short)c;
	tmp = crc ^ short_c;
	crc = (crc >> 8) ^ table[tmp & 0xFF];

	return crc;
}

unsigned short hexin_crc16_poly_is_low_calc(unsigned short crc16, unsigned char c, const unsigned short* table)
{
	unsigned short crc = crc16;
	unsigned short tmp, short_c;

	short_c = 0x00FF & (unsigned short)c;
	tmp = (crc >> 8) ^ short_c;
	crc = (crc << 8) ^ table[tmp];

	return crc;
}

static unsigned short __hexin_crc16_sick(unsigned short crc16, unsigned char c, char prev_byte)
{
	unsigned short crc = crc16;
	unsigned short short_c, short_p;

	short_c = 0x00FF & (unsigned short)c;
	short_p = (0x00FF & (unsigned short)prev_byte) << 8;

	if (crc & 0x8000) crc = (crc << 1) ^ CRC16_POLYNOMIAL_8005;
	else                crc = crc << 1;

	crc &= 0xFFFF;
	crc ^= (short_c | short_p);

	return crc;
}

unsigned short hexin_calc_crc16_sick(const unsigned char* pSrc, unsigned int len, unsigned short crc16)
{
	unsigned int   i = 0;
	char  prev_byte = 0x00;
	unsigned short crc = crc16;

	for (i = 0; i < len; i++) {
		crc = __hexin_crc16_sick(crc, pSrc[i], prev_byte);
		prev_byte = pSrc[i];
	}

	return crc;
}

unsigned short hexin_calc_crc16_network(const unsigned char* pSrc, unsigned int len, unsigned short crc16 /*reserved*/)
{
	unsigned int sum = 0;

	while (len > 1) {
		sum += *(unsigned short*)pSrc;
		len -= 2;
		pSrc += 2;
	}

	if (len) {
		sum += *(unsigned char*)pSrc;
	}

	while (sum >> 16) {
		sum = (sum >> 16) + (sum & 0xFFFF);
	}

	return (unsigned short)(~sum);
}

unsigned short hexin_calc_crc16_fletcher(const unsigned char* pSrc, unsigned int len, unsigned short crc16 /*reserved*/)
{
	unsigned int i = 0;
	unsigned short sum1 = 0, sum2 = 0;

	for (i = 0; i < len; i++) {
		sum1 = (sum1 + pSrc[i]) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum1 & 0xFF) | (sum2 << 8);
}

static unsigned int hexin_crc16_compute_init_table(struct _hexin_crc16* param)
{
	unsigned int i = 0, j = 0;
	unsigned short crc = 0, c = 0;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = (unsigned short)i;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x0001)   crc = (crc >> 1) ^ param->poly;
				else                        crc = crc >> 1;
				c = c >> 1;
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned short)i) << 8;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x8000) crc = (crc << 1) ^ param->poly;
				else                      crc = crc << 1;
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

static unsigned short hexin_crc16_compute_char(unsigned short crc16, unsigned char c, struct _hexin_crc16* param)
{
	unsigned short crc = crc16;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x00FF & (unsigned short)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 8) ^ (0x00FF & (unsigned short)c)) & 0xFF];
	}

	return crc;
}

unsigned short hexin_crc16_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc16* param, unsigned short init)
{
	unsigned int i = 0;
	unsigned short crc = init;              /* Fixed Issues #4  */

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = hexin_reverse16(param->poly);
		}
		param->is_initial = hexin_crc16_compute_init_table(param);
	}

	/* Fixed Issues #4  */
	if (HEXIN_REFIN_REFOUT_IS_TRUE(param) && (!HEXIN_GRADUAL_CALCULATE_IS_TRUE(param))) {
		crc = hexin_reverse16(init);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crc16_compute_char(crc, pSrc[i], param);
	}

	return (crc ^ param->xorout);
}

unsigned short CRC16_IBM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0x0000;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_USB(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_MODBUS(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_XMODEM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CCITT_FALSE(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xFFFF;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CCITT(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x0000;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_MAXIM(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0x0000;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_X25(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_DNP(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_3D65;
	CRC16.init = 0x0000;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CCTI_AUG(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x1D0F;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_KERMIT(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x0000;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_MCRF4XX(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_DECT_R(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_0589;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0001;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_DECT_X(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_0589;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_HACKER(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_RFID_EPC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xFFFF;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_PROFIBUS(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1DCF;
	CRC16.init = 0xFFFF;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_BUYPASS(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_GSM16(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_RIELLO(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xB2AA;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CRC16_A(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0xC6C6;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CDMA2000(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_C867;
	CRC16.init = 0xFFFF;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_TELEDISK(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0xA097;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_TMS37157(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_1021;
	CRC16.init = 0x89EC;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_EN13757(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_3D65;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0xFFFF;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_T10_DIF(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0x8BB7;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_DDS_110(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0x800D;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_CMS(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = CRC16_POLYNOMIAL_8005;
	CRC16.init = 0xFFFF;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_LJ1200(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0x6F63;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_NRSC5(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0x080B;
	CRC16.init = 0xFFFF;
	CRC16.refin = TRUE;
	CRC16.refout = TRUE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}

unsigned short CRC16_OPENSAFETY_A(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0x5935;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}
unsigned short CRC16_OPENSAFETY_B(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc16 CRC16;
	CRC16.is_initial = FALSE;
	CRC16.width = HEXIN_CRC16_WIDTH;
	CRC16.poly = 0x755B;
	CRC16.init = 0x0000;
	CRC16.refin = FALSE;
	CRC16.refout = FALSE;
	CRC16.xorout = 0x0000;
	CRC16.result = 0;
	return hexin_crc16_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC16, CRC16.init);
}
/********************************************************************/
/********************************************************************/
/*CRC24*/
static unsigned int hexin_reverse24(unsigned int data)
{
	unsigned int i = 0;
	unsigned int t = 0;
	for (i = 0; i < 24; i++) {
		t |= ((data >> i) & 0x00000001) << (23 - i);
	}
	return t;
}

unsigned int hexin_crc24_compute_init_table(struct _hexin_crc24* param)
{
	unsigned int i = 0, j = 0;
	unsigned int crc = 0x00000000L;
	unsigned int c = 0x00000000L;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = i;
			for (j = 0; j < 8; j++) {
				if (crc & 0x00000001L) crc = (crc >> 1) ^ param->poly;
				else                     crc = (crc >> 1);
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned int)i) << 16;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x00800000L)  crc = (crc << 1) ^ param->poly;
				else                              crc = (crc << 1);
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

unsigned int hexin_crc24_compute_char(unsigned int crc24, unsigned char c, struct _hexin_crc24* param)
{
	unsigned int crc = crc24;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 16) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}

	return crc;
}

unsigned int hexin_crc24_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc24* param, unsigned int init)
{
	unsigned int i = 0;
	unsigned int crc = init;

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = hexin_reverse24(param->poly);
		}
		param->is_initial = hexin_crc24_compute_init_table(param);
	}
	/* Fixed Issues #4  */
	if (HEXIN_REFIN_REFOUT_IS_TRUE(param) && (!HEXIN_GRADUAL_CALCULATE_IS_TRUE(param))) {
		crc = hexin_reverse24(init);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crc24_compute_char(crc, pSrc[i], param);
//		printf("%d:%X\n", i, crc);

	}

	return ((crc & 0xFFFFFF) ^ param->xorout);
}

unsigned long CRC24_BLE(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;		
	CRC_24.poly = CRC24_POLYNOMIAL_00065B;	
	CRC_24.init = 0x00555555;				
	CRC_24.refin = TRUE;					
	CRC_24.refout = TRUE;					
	CRC_24.xorout = 0x00000000;				
	CRC_24.result = 0;						
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_FLEXRAYA(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_5D6DCB;
	CRC_24.init = 0x00FEDCBA;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_FLEXRAYB(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_5D6DCB;
	CRC_24.init = 0x00ABCDEF;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_OPENPGP(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_864CFB;
	CRC_24.init = 0x00B704CE;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_LTE_A(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_864CFB;
	CRC_24.init = 0x00000000;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_LTE_B(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_800063;
	CRC_24.init = 0x00000000;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_OS9(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_800063;
	CRC_24.init = 0x00FFFFFF;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00000000;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_INTERLAKEN(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_328B63;
	CRC_24.init = 0x00FFFFFF;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00FFFFFF;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}

unsigned long CRC24_HACKER(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc24 CRC_24;
	CRC_24.is_initial = FALSE;
	CRC_24.width = HEXIN_CRC24_WIDTH;
	CRC_24.poly = CRC24_POLYNOMIAL_800063;
	CRC_24.init = 0x00FFFFFF;
	CRC_24.refin = FALSE;
	CRC_24.refout = FALSE;
	CRC_24.xorout = 0x00FFFFFF;
	CRC_24.result = 0;
	return hexin_crc24_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_24, CRC_24.init);
}
/********************************************************************/
/********************************************************************/
/*CRC32*/
unsigned int hexin_reverse32(unsigned int data)
{
	unsigned int i = 0;
	unsigned int t = 0;
	for (i = 0; i < 32; i++) {
		t |= ((data >> i) & 0x00000001) << (31 - i);
	}
	return t;
}

unsigned int hexin_crc32_init_table_poly_is_high(unsigned int polynomial, unsigned int* table)
{
	unsigned int i = 0, j = 0;
	unsigned int crc = 0x00000000L;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = i;
		for (j = 0; j < 8; j++) {
			if (crc & 0x00000001L) crc = (crc >> 1) ^ polynomial;
			else                     crc = crc >> 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned int hexin_crc32_init_table_poly_is_low(unsigned int polynomial, unsigned int* table)
{
	unsigned int i = 0, j = 0;
	unsigned int crc = 0x00000000L;
	unsigned int c = 0x00000000L;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = ((unsigned int)i) << 24;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x80000000L)  crc = (crc << 1) ^ polynomial;
			else                              crc = crc << 1;
			c = c << 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned int hexin_crc32_poly_is_high_calc(unsigned int crc32, unsigned char c, const unsigned int* table)
{
	unsigned int crc = crc32;
	unsigned int tmp = 0x00000000L;
	unsigned int int_c = 0x00000000L;

	int_c = 0x000000FFL & (unsigned int)c;
	tmp = crc ^ int_c;
	crc = (crc >> 8) ^ table[tmp & 0xFF];

	return crc;
}

unsigned int hexin_crc32_poly_is_low_calc(unsigned int crc32, unsigned char c, const unsigned int* table)
{
	unsigned int crc = crc32;
	unsigned int tmp = 0x00000000L;
	unsigned int int_c = 0x00000000L;

	int_c = 0x000000FF & (unsigned int)c;
	tmp = (crc >> 24) ^ int_c;
	crc = (crc << 8) ^ table[tmp & 0xFF];

	return crc;
}

unsigned int hexin_calc_crc32_adler(const unsigned char* pSrc, unsigned int len, unsigned int crc32 /*reserved*/)
{
	unsigned int sum1 = 1, sum2 = 0;
	unsigned int i = 0x00000000L;

	for (i = 0; i < len; i++) {
		sum1 = (sum1 + pSrc[i]) % HEXIN_MOD_ADLER;
		sum2 = (sum2 + sum1) % HEXIN_MOD_ADLER;
	}
	return (sum2 << 16) | sum1;
}

unsigned int hexin_calc_crc32_fletcher(const unsigned char* pSrc, unsigned int len, unsigned int crc32 /*reserved*/)
{
	unsigned long sum1 = 0xFFFF, sum2 = 0xFFFF;

	while (len > 1) {
		sum1 += *(unsigned short*)pSrc;
		sum2 += sum1;
		sum1 = (sum1 & 0xFFFF) + (sum1 >> 16);
		sum2 = (sum2 & 0xFFFF) + (sum2 >> 16);
		len -= 2;
		pSrc += 2;
	}

	if (len) {
		sum1 += *(unsigned char*)pSrc;
		sum2 += sum1;
		sum1 = (sum1 & 0xFFFF) + (sum1 >> 16);
		sum2 = (sum2 & 0xFFFF) + (sum2 >> 16);
	}

	return (sum1 & 0xFFFF) | (sum2 << 16);
}

unsigned int hexin_crc32_compute_init_table(struct _hexin_crc32* param)
{
	unsigned int i = 0, j = 0;
	unsigned int crc = 0x00000000L;
	unsigned int c = 0x00000000L;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = i;
			for (j = 0; j < 8; j++) {
				if (crc & 0x00000001L) crc = (crc >> 1) ^ param->poly;
				else                     crc = (crc >> 1);
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned int)i) << 24;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x80000000L)  crc = (crc << 1) ^ param->poly;
				else                              crc = (crc << 1);
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

unsigned int hexin_crc32_compute_char(unsigned int crc32, unsigned char c, struct _hexin_crc32* param)
{
	unsigned int crc = crc32;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 24) ^ (0x000000FFL & (unsigned int)c)) & 0xFF];
	}

	return crc;
}

unsigned int hexin_crc32_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc32* param, unsigned int init)
{
	unsigned int i = 0, result = 0;
	unsigned int crc = (init << (HEXIN_CRC32_WIDTH - param->width));       /* Fixed Issues #4  */

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = (hexin_reverse32(param->poly) >> (HEXIN_CRC32_WIDTH - param->width));
		}
		else {
			param->poly = (param->poly << (HEXIN_CRC32_WIDTH - param->width));
		}
		param->is_initial = hexin_crc32_compute_init_table(param);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crc32_compute_char(crc, pSrc[i], param);
	}

	result = (HEXIN_REFIN_REFOUT_IS_TRUE(param)) ? crc : (crc >> (HEXIN_CRC32_WIDTH - param->width));

	return (result ^ param->xorout);
}

unsigned int hexin_crc32_compute_stm32(const unsigned char* pSrc, unsigned int len, struct _hexin_crc32* param, unsigned int init)
{
	unsigned int i = 0, j = 0, result = 0;
	unsigned int crc = init;

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = (hexin_reverse32(param->poly) >> (HEXIN_CRC32_WIDTH - param->width));
		}
		else {
			param->poly = (param->poly << (HEXIN_CRC32_WIDTH - param->width));
		}
		param->is_initial = hexin_crc32_compute_init_table(param);
	}

	for (i = 0; i < len; i++) {
		crc ^= (unsigned int)pSrc[i];
		for (j = 0; j < 4; j++) {
			result = param->table[(crc >> 24) & 0xFF];
			crc <<= 8;
			crc ^= result;
		}
	}

	return crc;
}
unsigned long CRC32_MPEG2(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = CRC32_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0x00000000L;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_CRC_32(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = CRC32_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = TRUE;
	CRC_32.refout = TRUE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_POSIX(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = CRC32_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0x00000000L;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_BZIP2(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = CRC32_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_JAMCRC(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0x04C11DB7L;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = TRUE;
	CRC_32.refout = TRUE;
	CRC_32.xorout = 0x00000000L;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_AUTOSAR(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0xF4ACFB13L;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = TRUE;
	CRC_32.refout = TRUE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_ISCSI(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0x1EDC6F41L;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = TRUE;
	CRC_32.refout = TRUE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_CRC32_D(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0xA833982BL;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = TRUE;
	CRC_32.refout = TRUE;
	CRC_32.xorout = 0xFFFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_CRC32_Q(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0x814141ABL;
	CRC_32.init = 0;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_XFER(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = HEXIN_CRC32_WIDTH;
	CRC_32.poly = 0x000000AFL;
	CRC_32.init = 0x00000000L;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0x00000000L;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC30_CDMA(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = 30;
	CRC_32.poly = CRC30_POLYNOMIAL_2030B9C7;
	CRC_32.init = 0x3FFFFFFFL;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0x3FFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC31_PHILIPS(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = 31;
	CRC_32.poly = CRC31_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0x7FFFFFFFL;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0x7FFFFFFFL;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}

unsigned long CRC32_STM32(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc32 CRC_32;
	CRC_32.is_initial = FALSE;
	CRC_32.width = 32;
	CRC_32.poly = CRC32_POLYNOMIAL_04C11DB7;
	CRC_32.init = 0xFFFFFFFFL;
	CRC_32.refin = FALSE;
	CRC_32.refout = FALSE;
	CRC_32.xorout = 0x00000000L;
	CRC_32.result = 0;
	return hexin_crc32_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_32, CRC_32.init);
}
/********************************************************************/
/********************************************************************/
/*CRC64*/
unsigned long long hexin_reverse64(unsigned long long data)
{
	unsigned int i = 0;
	unsigned long long t = 0;
	for (i = 0; i < 64; i++) {
		t |= ((data >> i) & 0x0000000000000001L) << (63 - i);
	}
	return t;
}

unsigned char hexin_crc64_init_table_poly_is_high(unsigned long long polynomial, unsigned long long* table)
{
	unsigned int i = 0, j = 0;
	unsigned long long crc = 0x0000000000000000L;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = (unsigned long long) i;
		for (j = 0; j < 8; j++) {
			if (crc & 0x0000000000000001L) {
				crc = (crc >> 1) ^ polynomial;
			}
			else {
				crc = crc >> 1;
			}
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned char hexin_crc64_init_table_poly_is_low(unsigned long long polynomial, unsigned long long* table)
{
	unsigned int i = 0, j = 0;
	unsigned long long crc = 0x0000000000000000L;
	unsigned long long c = 0x0000000000000000L;

	for (i = 0; i < MAX_TABLE_ARRAY; i++) {
		crc = 0;
		c = ((unsigned long long) i) << 56;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x8000000000000000L) {
				crc = (crc << 1) ^ polynomial;
			}
			else {
				crc = crc << 1;
			}
			c = c << 1;
		}
		table[i] = crc;
	}
	return TRUE;
}

unsigned long long hexin_crc64_poly_is_high_calc(unsigned long long crc64, unsigned char c, const unsigned long long* table)
{
	unsigned long long crc = crc64;
	unsigned long long tmp, long_c;

	long_c = 0x00000000000000FFL & (unsigned long long) c;
	tmp = crc ^ long_c;
	crc = (crc >> 8) ^ table[tmp & 0xFF];
	return crc;
}

unsigned long long hexin_crc64_poly_is_low_calc(unsigned long long crc64, unsigned char c, const unsigned long long* table)
{
	unsigned long long crc = crc64;
	unsigned long long tmp, long_c;

	long_c = 0x00000000000000FFL & (unsigned long long) c;
	tmp = (crc >> 56) ^ long_c;
	crc = (crc << 8) ^ table[tmp & 0xFF];
	return crc;
}

static unsigned int hexin_crc64_compute_init_table(struct _hexin_crc64* param)
{
	unsigned int i = 0, j = 0;
	unsigned long long crc = 0x0000000000000000L;
	unsigned long long c = 0x0000000000000000L;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = (unsigned long long) i;
			for (j = 0; j < 8; j++) {
				if (crc & 0x0000000000000001L) {
					crc = (crc >> 1) ^ param->poly;
				}
				else {
					crc = (crc >> 1);
				}
			}
			param->table[i] = crc;
		}
	}
	else {
		for (i = 0; i < MAX_TABLE_ARRAY; i++) {
			crc = 0;
			c = ((unsigned long long) i) << 56;
			for (j = 0; j < 8; j++) {
				if ((crc ^ c) & 0x8000000000000000L) {
					crc = (crc << 1) ^ param->poly;
				}
				else {
					crc = (crc << 1);
				}
				c = c << 1;
			}
			param->table[i] = crc;
		}
	}
	return TRUE;
}

static unsigned long long hexin_crc64_compute_char(unsigned long long crc64, unsigned char c, struct _hexin_crc64* param)
{
	unsigned long long crc = crc64;

	if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
		crc = (crc >> 8) ^ param->table[((crc >> 0) ^ (0x00000000000000FFL & (unsigned long long)c)) & 0xFF];
	}
	else {
		crc = (crc << 8) ^ param->table[((crc >> 56) ^ (0x00000000000000FFL & (unsigned long long)c)) & 0xFF];
	}

	return crc;
}

unsigned long long hexin_crc64_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc64* param, unsigned long long init)
{
	unsigned int i = 0;
	unsigned long long result = 0;
	unsigned long long crc = (init << (HEXIN_CRC64_WIDTH - param->width));

	if (param->is_initial == FALSE) {
		if (HEXIN_REFIN_REFOUT_IS_TRUE(param)) {
			param->poly = (hexin_reverse64(param->poly) >> (HEXIN_CRC64_WIDTH - param->width));
		}
		else {
			param->poly = (param->poly << (HEXIN_CRC64_WIDTH - param->width));
		}
		param->is_initial = hexin_crc64_compute_init_table(param);
	}

	for (i = 0; i < len; i++) {
		crc = hexin_crc64_compute_char(crc, pSrc[i], param);
	}

	result = (HEXIN_REFIN_REFOUT_IS_TRUE(param)) ? crc : (crc >> (HEXIN_CRC64_WIDTH - param->width));

	return (result ^ param->xorout);
}

unsigned long long CRC64_ISO(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc64 CRC_64;
	CRC_64.is_initial = FALSE;
	CRC_64.width = HEXIN_CRC64_WIDTH;
	CRC_64.poly = 0x000000000000001BL;
	CRC_64.init = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.refin = TRUE;
	CRC_64.refout = TRUE;
	CRC_64.xorout = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.result = 0;
	return hexin_crc64_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_64, CRC_64.init);
}

unsigned long long CRC64_ECMA182(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc64 CRC_64;
	CRC_64.is_initial = FALSE;
	CRC_64.width = HEXIN_CRC64_WIDTH;
	CRC_64.poly = CRC64_POLYNOMIAL_ECMA182;
	CRC_64.init = 0x0000000000000000L;
	CRC_64.refin = FALSE;
	CRC_64.refout = FALSE;
	CRC_64.xorout = 0x0000000000000000L;
	CRC_64.result = 0;
	return hexin_crc64_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_64, CRC_64.init);
}

unsigned long long CRC64_WE(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc64 CRC_64;
	CRC_64.is_initial = FALSE;
	CRC_64.width = HEXIN_CRC64_WIDTH;
	CRC_64.poly = CRC64_POLYNOMIAL_ECMA182;
	CRC_64.init = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.refin = FALSE;
	CRC_64.refout = FALSE;
	CRC_64.xorout = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.result = 0;
	return hexin_crc64_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_64, CRC_64.init);
}

unsigned long long CRC64_XZ(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc64 CRC_64;
	CRC_64.is_initial = FALSE;
	CRC_64.width = HEXIN_CRC64_WIDTH;
	CRC_64.poly = CRC64_POLYNOMIAL_ECMA182;
	CRC_64.init = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.refin = TRUE;
	CRC_64.refout = TRUE;
	CRC_64.xorout = 0xFFFFFFFFFFFFFFFFL;
	CRC_64.result = 0;
	return hexin_crc64_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_64, CRC_64.init);
}

unsigned long long CRC64_GMS40(char* parameter, unsigned char Szie)
{
	static struct _hexin_crc64 CRC_64;
	CRC_64.is_initial = FALSE;
	CRC_64.width = 40;
	CRC_64.poly = 0x0004820009;
	CRC_64.init = 0x0000000000;
	CRC_64.refin = FALSE;
	CRC_64.refout = FALSE;
	CRC_64.xorout = 0xFFFFFFFFFFL;
	CRC_64.result = 0;
	return hexin_crc64_compute((const unsigned char*)parameter, (unsigned int)Szie, &CRC_64, CRC_64.init);
}
/********************************************************************/
/********************************************************************/

