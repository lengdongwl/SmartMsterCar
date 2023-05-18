#ifndef  _CRC_H_
#define _CRC_H_

#ifndef TRUE
#define                 TRUE                                    1
#endif

#ifndef FALSE
#define                 FALSE                                   0
#endif

#ifndef MAX_TABLE_ARRAY
#define                 MAX_TABLE_ARRAY                         256
#endif

#define                 HEXIN_REFIN_REFOUT_IS_TRUE(x)           ( ( x->refin == TRUE ) && ( x->refout == TRUE ) )

//#define                 HEXIN_POLYNOMIAL_IS_HIGH_8(x)             ( x & 0x80 )
#define                 HEXIN_GRADUAL_CALCULATE_IS_TRUE(x)      ( x->is_gradual == 2 )
/********************************************************************/
/*CRC_X*/

#define                 HEXIN_CRCX_WIDTH                        16

struct _hexin_crcx {
	unsigned int    is_initial;
	unsigned int    is_gradual;
	unsigned short  width;
	unsigned short  poly;
	unsigned short  init;
	unsigned int    refin;
	unsigned int    refout;
	unsigned short  xorout;
	unsigned short  result;
	unsigned short  table[MAX_TABLE_ARRAY];
};

unsigned short hexin_crcx_reverse12(unsigned short data);
unsigned short hexin_crcx_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crcx* param, unsigned short init);

unsigned short CRC3_GSM(char* parameter, unsigned char Szie);
unsigned short CRC3_ROHC(char* parameter, unsigned char Szie);
unsigned short CRC4_ITU(char* parameter, unsigned char Szie);
unsigned short CRC4_INTERLAKEN4(char* parameter, unsigned char Szie);
unsigned short CRC5_ITU(char* parameter, unsigned char Szie);
unsigned short CRC5_EPC(char* parameter, unsigned char Szie);
unsigned short CRC5_USB(char* parameter, unsigned char Szie);
unsigned short CRC6_ITU(char* parameter, unsigned char Szie);
unsigned short CRC6_GSM(char* parameter, unsigned char Szie);
unsigned short CRC6_DARC6(char* parameter, unsigned char Szie);
unsigned short CRC7_MMC(char* parameter, unsigned char Szie);
unsigned short CRC7_UMTS7(char* parameter, unsigned char Szie);
unsigned short CRC7_ROHC7(char* parameter, unsigned char Szie);
unsigned short CRC10_ATM10(char* parameter, unsigned char Szie);
unsigned short CRC10_CDMA2000(char* parameter, unsigned char Szie);
unsigned short CRC10_GSM10(char* parameter, unsigned char Szie);
unsigned short CRC11_FLEXRAY11(char* parameter, unsigned char Szie);
unsigned short CRC11_UMTS11(char* parameter, unsigned char Szie);
unsigned short CRC12_CDMA2000(char* parameter, unsigned char Szie);
unsigned short CRC12_DECT12(char* parameter, unsigned char Szie);
unsigned short CRC12_GSM12(char* parameter, unsigned char Szie);
unsigned short CRC12_UMTS12(char* parameter, unsigned char Szie);
unsigned short CRC13_BBC(char* parameter, unsigned char Szie);
unsigned short CRC14_DARC(char* parameter, unsigned char Szie);
unsigned short CRC14_GSM(char* parameter, unsigned char Szie);
unsigned short CRC15_MPT1327(char* parameter, unsigned char Szie);

/********************************************************************/
/********************************************************************/
/*CAN_X*/
#define                 HEXIN_CANX_WIDTH                        32

#define                 CAN15_POLYNOMIAL_00004599               0x00004599L
#define                 CAN17_POLYNOMIAL_0001685B               0x0001685BL
#define                 CAN21_POLYNOMIAL_00102899               0x00102899L

struct _hexin_canx {
	unsigned int  is_initial;
	unsigned int  is_gradual;
	unsigned int  width;
	unsigned int  poly;
	unsigned int  init;
	unsigned int  refin;
	unsigned int  refout;
	unsigned int  xorout;
	unsigned int  result;
	unsigned int  table[MAX_TABLE_ARRAY];
};

unsigned int hexin_canx_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_canx* param, unsigned int init);

unsigned short CANX_CAN15(char* parameter, unsigned char Szie);
unsigned long CANX_CAN17(char* parameter, unsigned char Szie);
unsigned long CANX_CAN21(char* parameter, unsigned char Szie);

/********************************************************************/
/*CRC_8*/
#define                 HEXIN_CRC8_WIDTH                        16

#define CRC8_POLYNOMIAL_07                                      0x07
#define CRC8_POLYNOMIAL_31                                      0x31
#define CRC8_POLYNOMIAL_39                                      0x39
#define CRC8_POLYNOMIAL_49                                      0x49
#define CRC8_POLYNOMIAL_1D                                      0x1D
#define CRC8_POLYNOMIAL_2F                                      0x2F
#define CRC8_POLYNOMIAL_9B                                      0x9B
#define CRC8_POLYNOMIAL_A7                                      0xA7
#define CRC8_POLYNOMIAL_D5                                      0xD5

struct _hexin_crc8 {
	unsigned int   is_initial;
	unsigned int   is_gradual;
	unsigned char  width;
	unsigned char  poly;
	unsigned char  init;
	unsigned int   refin;
	unsigned int   refout;
	unsigned char  xorout;
	unsigned char  result;
	unsigned char  table[MAX_TABLE_ARRAY];
};
unsigned char hexin_reverse8(unsigned char data);
unsigned int hexin_crc8_init_table_poly_is_high(unsigned char polynomial, unsigned char* table);
unsigned int hexin_crc8_init_table_poly_is_low(unsigned char polynomial, unsigned char* table);
unsigned char hexin_calc_crc8_bcc(const unsigned char* pSrc, unsigned int len, unsigned char crc8);
unsigned char hexin_calc_crc8_lrc(const unsigned char* pSrc, unsigned int len, unsigned char crc8);
unsigned char hexin_calc_crc8_sum(const unsigned char* pSrc, unsigned int len, unsigned char crc8);
unsigned char hexin_calc_crc8_fletcher(const unsigned char* pSrc, unsigned int len, unsigned char crc8 /*reserved*/);
unsigned char hexin_crc8_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc8* param, unsigned char init);
unsigned char hexin_crc8_get_lin2x_pid(const unsigned char id);
unsigned char hexin_calc_crc8_lin(const unsigned char* pSrc, unsigned int len, unsigned char crc8);
unsigned char hexin_calc_crc8_lin2x(const unsigned char* pSrc, unsigned int len, unsigned char crc8);
/*CRC-8*/
unsigned char CRC8_MAXMI(char* parameter, unsigned char Szie);
unsigned char CRC8_ROHC(char* parameter, unsigned char Szie);
unsigned char CRC8_ITU(char* parameter, unsigned char Szie);
unsigned char CRC8_CRC8(char* parameter, unsigned char Szie);
unsigned char CRC8_HACKER(char* parameter, unsigned char Szie);
unsigned char CRC8_AUTOSAR8(char* parameter, unsigned char Szie);
unsigned char CRC8_LTE8(char* parameter, unsigned char Szie);
unsigned char CRC8_WCDMA(char* parameter, unsigned char Szie);
unsigned char CRC8_SAE_J1850(char* parameter, unsigned char Szie);
unsigned char CRC8_ICODE(char* parameter, unsigned char Szie);
unsigned char CRC8_GSM8_A(char* parameter, unsigned char Szie);
unsigned char CRC8_GSM8_B(char* parameter, unsigned char Szie);
unsigned char CRC8_NRSC(char* parameter, unsigned char Szie);
unsigned char CRC8_BLUETOOTH(char* parameter, unsigned char Szie);
unsigned char CRC8_DVB_S2(char* parameter, unsigned char Szie);
unsigned char CRC8_EBU8(char* parameter, unsigned char Szie);
unsigned char CRC8_DARC(char* parameter, unsigned char Szie);
unsigned char CRC8_OPENSAFETY8(char* parameter, unsigned char Szie);
unsigned char CRC8_MAD(char* parameter, unsigned char Szie);
/********************************************************************/
/********************************************************************/
/*CRC_16*/
#define                 HEXIN_CRC16_WIDTH                       16

#define                 CRC16_POLYNOMIAL_1021                   0x1021
#define                 CRC16_POLYNOMIAL_8005                   0x8005
#define                 CRC16_POLYNOMIAL_0589                   0x0589
#define                 CRC16_POLYNOMIAL_1DCF                   0x1DCF
#define                 CRC16_POLYNOMIAL_3D65                   0x3D65
#define                 CRC16_POLYNOMIAL_C867                   0xC867
struct _hexin_crc16 {
	unsigned int    is_initial;
	unsigned int    is_gradual;
	unsigned short  width;
	unsigned short  poly;
	unsigned short  init;
	unsigned int    refin;
	unsigned int    refout;
	unsigned short  xorout;
	unsigned short  result;
	unsigned short  table[MAX_TABLE_ARRAY];
};
unsigned short hexin_reverse16(unsigned short data);
unsigned int hexin_crc16_init_table_poly_is_high(unsigned short polynomial, unsigned short* table);
unsigned int hexin_crc16_init_table_poly_is_low(unsigned short polynomial, unsigned short* table);
unsigned short hexin_calc_crc16_sick(const unsigned char* pSrc, unsigned int len, unsigned short crc16);
unsigned short hexin_calc_crc16_network(const unsigned char* pSrc, unsigned int len, unsigned short crc16 /*reserved*/);
unsigned short hexin_calc_crc16_fletcher(const unsigned char* pSrc, unsigned int len, unsigned short crc16 /*reserved*/);
unsigned short hexin_crc16_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc16* param, unsigned short init);

unsigned short CRC16_IBM(char* parameter, unsigned char Szie);
unsigned short CRC16_USB(char* parameter, unsigned char Szie);
unsigned short CRC16_MODBUS(char* parameter, unsigned char Szie);
unsigned short CRC16_XMODEM(char* parameter, unsigned char Szie);
unsigned short CRC16_CCITT_FALSE(char* parameter, unsigned char Szie);
unsigned short CRC16_CCITT(char* parameter, unsigned char Szie);
unsigned short CRC16_MAXIM(char* parameter, unsigned char Szie);
unsigned short CRC16_X25(char* parameter, unsigned char Szie);
unsigned short CRC16_DNP(char* parameter, unsigned char Szie);
unsigned short CRC16_CCTI_AUG(char* parameter, unsigned char Szie);
unsigned short CRC16_KERMIT(char* parameter, unsigned char Szie);
unsigned short CRC16_MCRF4XX(char* parameter, unsigned char Szie);
unsigned short CRC16_DECT_R(char* parameter, unsigned char Szie);
unsigned short CRC16_DECT_X(char* parameter, unsigned char Szie);
unsigned short CRC16_HACKER(char* parameter, unsigned char Szie);
unsigned short CRC16_RFID_EPC(char* parameter, unsigned char Szie);
unsigned short CRC16_PROFIBUS(char* parameter, unsigned char Szie);
unsigned short CRC16_BUYPASS(char* parameter, unsigned char Szie);
unsigned short CRC16_GSM16(char* parameter, unsigned char Szie);
unsigned short CRC16_RIELLO(char* parameter, unsigned char Szie);
unsigned short CRC16_CRC16_A(char* parameter, unsigned char Szie);
unsigned short CRC16_CDMA2000(char* parameter, unsigned char Szie);
unsigned short CRC16_TELEDISK(char* parameter, unsigned char Szie);
unsigned short CRC16_TMS37157(char* parameter, unsigned char Szie);
unsigned short CRC16_EN13757(char* parameter, unsigned char Szie);
unsigned short CRC16_T10_DIF(char* parameter, unsigned char Szie);
unsigned short CRC16_DDS_110(char* parameter, unsigned char Szie);
unsigned short CRC16_CMS(char* parameter, unsigned char Szie);
unsigned short CRC16_LJ1200(char* parameter, unsigned char Szie);
unsigned short CRC16_NRSC5(char* parameter, unsigned char Szie);
unsigned short CRC16_OPENSAFETY_A(char* parameter, unsigned char Szie);
unsigned short CRC16_OPENSAFETY_B(char* parameter, unsigned char Szie);

/********************************************************************/
/*CRC_24*/
#define                 HEXIN_CRC24_WIDTH                       24

#define                 CRC24_POLYNOMIAL_00065B                 0x00065B
#define                 CRC24_POLYNOMIAL_5D6DCB                 0x5D6DCB
#define                 CRC24_POLYNOMIAL_864CFB                 0x864CFB
#define                 CRC24_POLYNOMIAL_800063                 0x800063
#define                 CRC24_POLYNOMIAL_328B63                 0x328B63

struct _hexin_crc24 {
	unsigned int  is_initial;
	unsigned int  is_gradual;
	unsigned int  width;
	unsigned int  poly;
	unsigned int  init;
	unsigned int  refin;
	unsigned int  refout;
	unsigned int  xorout;
	unsigned int  result;
	unsigned int  table[MAX_TABLE_ARRAY];
};

unsigned int hexin_crc24_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc24* param, unsigned int init);

unsigned long CRC24_BLE(char* parameter, unsigned char Szie);
unsigned long CRC24_FLEXRAYA(char* parameter, unsigned char Szie);
unsigned long CRC24_FLEXRAYB(char* parameter, unsigned char Szie);
unsigned long CRC24_OPENPGP(char* parameter, unsigned char Szie);
unsigned long CRC24_LTE_A(char* parameter, unsigned char Szie);
unsigned long CRC24_LTE_B(char* parameter, unsigned char Szie);
unsigned long CRC24_OS9(char* parameter, unsigned char Szie);
unsigned long CRC24_INTERLAKEN(char* parameter, unsigned char Szie);
unsigned long CRC24_HACKER(char* parameter, unsigned char Szie);
/********************************************************************/
/*CRC_32*/
#define                 HEXIN_CRC32_WIDTH                       32
#define                 HEXIN_MOD_ADLER                         65521

#define                 CRC30_POLYNOMIAL_2030B9C7               0x2030B9C7L
#define                 CRC31_POLYNOMIAL_04C11DB7               0x04C11DB7L
#define		            CRC32_POLYNOMIAL_04C11DB7		        0x04C11DB7L

struct _hexin_crc32 {
	unsigned int  is_initial;
	unsigned int  is_gradual;
	unsigned int  width;
	unsigned int  poly;
	unsigned int  init;
	unsigned int  refin;
	unsigned int  refout;
	unsigned int  xorout;
	unsigned int  result;
	unsigned int  table[MAX_TABLE_ARRAY];
};

unsigned int hexin_reverse32(unsigned int data);
unsigned int hexin_crc32_init_table_poly_is_high(unsigned int polynomial, unsigned int* table);
unsigned int hexin_crc32_init_table_poly_is_low(unsigned int polynomial, unsigned int* table);
unsigned int hexin_calc_crc32_adler(const unsigned char* pSrc, unsigned int len, unsigned int crc32 /*reserved*/);
unsigned int hexin_calc_crc32_fletcher(const unsigned char* pSrc, unsigned int len, unsigned int crc32 /*reserved*/);
unsigned int hexin_crc32_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc32* param, unsigned int init);
unsigned int hexin_crc32_compute_stm32(const unsigned char* pSrc, unsigned int len, struct _hexin_crc32* param, unsigned int init);

unsigned long CRC32_MPEG2(char* parameter, unsigned char Szie);
unsigned long CRC32_CRC_32(char* parameter, unsigned char Szie);
unsigned long CRC32_POSIX(char* parameter, unsigned char Szie);
unsigned long CRC32_BZIP2(char* parameter, unsigned char Szie);
unsigned long CRC32_JAMCRC(char* parameter, unsigned char Szie);
unsigned long CRC32_AUTOSAR(char* parameter, unsigned char Szie);
unsigned long CRC32_ISCSI(char* parameter, unsigned char Szie);
unsigned long CRC32_CRC32_D(char* parameter, unsigned char Szie);
unsigned long CRC32_CRC32_Q(char* parameter, unsigned char Szie);
unsigned long CRC32_XFER(char* parameter, unsigned char Szie);
unsigned long CRC30_CDMA(char* parameter, unsigned char Szie);
unsigned long CRC31_PHILIPS(char* parameter, unsigned char Szie);
unsigned long CRC32_STM32(char* parameter, unsigned char Szie);

/********************************************************************/
/*CRC_64*/

#define                 HEXIN_CRC64_WIDTH                       64

#define		            CRC64_POLYNOMIAL_ECMA182                0x42F0E1EBA9EA3693L
struct _hexin_crc64 {
	unsigned int  is_initial;
	unsigned int  is_gradual;
	unsigned int  width;
	unsigned long long  poly;
	unsigned long long  init;
	unsigned int  refin;
	unsigned int  refout;
	unsigned long long  xorout;
	unsigned long long  result;
	unsigned long long  table[MAX_TABLE_ARRAY];
};

unsigned long long hexin_reverse64(unsigned long long data);
unsigned char hexin_crc64_init_table_poly_is_high(unsigned long long polynomial, unsigned long long* table);
unsigned char hexin_crc64_init_table_poly_is_low(unsigned long long polynomial, unsigned long long* table);
unsigned long long hexin_crc64_compute(const unsigned char* pSrc, unsigned int len, struct _hexin_crc64* param, unsigned long long init);

unsigned long long CRC64_ISO(char* parameter, unsigned char Szie);
unsigned long long CRC64_ECMA182(char* parameter, unsigned char Szie);
unsigned long long CRC64_WE(char* parameter, unsigned char Szie);
unsigned long long CRC64_XZ(char* parameter, unsigned char Szie);
unsigned long long CRC64_GMS40(char* parameter, unsigned char Szie);

#endif
