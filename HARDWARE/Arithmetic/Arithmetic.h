#ifndef __ATITHMETIC_H
#define __ATITHMETIC_H
#include "sys.h"


#define LZW 0 //LZW编码算法（1：开启 0：关闭）

#if 0
typedef struct
{
    uint8_t number_a;           //乘系数a(数字)
    uint8_t number_b;           //加系数b(数字)
    uint8_t capital_letter_a;   //乘系数a(大写字母)
    uint8_t capital_letter_b;   //加系数b(大写字母)
    uint8_t lowercase_a;        //乘系数a(小写字母)
    uint8_t lowercase_b;        //加系数b(小写字母)
    uint8_t Data_Size;          //加密/解密数据大小
    uint8_t amount_m;           //字母的数目
    uint8_t number_m;           //数字的数目
}Coefficient;//仿射密码加密/解密计算系数

uint8_t Arithmetic_coding(uint8_t* coding,uint8_t size);										//算数编码
uint16_t CRC_16_MODBUS(uint8_t* check_code,uint8_t length);									//CRC校验
uint16_t RSA_encryption(uint16_t message, uint16_t e_d, uint16_t N);				//RSA加密/解密
uint16_t relatively_prime(uint16_t A, uint16_t B);													//最大公约计算/互质数判断
uint8_t prime_number(uint16_t X);																						//质数判断
void ence_code_encryption(uint8_t size);																		//栅栏码加密（2栏）
void ence_code_decode(uint8_t size);																				//栅栏码解密（2栏）
void Polybius_Square_encryption(uint8_t* data, uint8_t size);								//波利比奥斯方阵密码加密
void Polybius_Square_decode(uint8_t* data, uint8_t size);										//波利比奥斯方阵密码解密
uint8_t multiplicative_inverse(uint8_t a, uint8_t m);												//乘法逆元计算
void Affine_cipher_encryption(uint8_t* parameter,Coefficient* calculate);		//仿射密码加密
void Affine_cipher_decode(uint8_t* parameter, Coefficient* calculate);			//仿射密码解密
#endif

#if LZW==1	//LZW算法
extern uint8_t LZW_Code_Stream[20];				//LZW输出码流
void LZW_Coding(uint8_t *LZW_String);																				//LZW编码
uint8_t String_len(uint8_t* temp);																					//字符串大小检测
#endif 

uint8_t ASCLL_conversion(char n1,char n2);																	//ASCLL码转换十六进制0xXX
uint8_t Extract_letter_figure(char* parameter, uint8_t size, uint8_t Mode); //字符串提取字母/数字
uint8_t Seek_biggest_minimum(char* Data_value, uint8_t size,uint8_t Mode);	//提取字符串最大或最小的字母

#endif
/*
		//仿射密码
    Coefficient Encryption_coefficient;
    Encryption_coefficient.number_a = 13;           //乘系数a(数字)
    Encryption_coefficient.number_b = 3;            //加系数b(数字)
    Encryption_coefficient.capital_letter_a = 5;    //乘系数a(大写字母)
    Encryption_coefficient.capital_letter_b = 8;    //加系数b(大写字母)
    Encryption_coefficient.lowercase_a = 19;        //乘系数a(小写字母)
    Encryption_coefficient.lowercase_b = 7;         //加系数b(小写字母)
    Encryption_coefficient.amount_m = 26;           //字母的数目
    Encryption_coefficient.number_m = 10;           //数字的数目
    Encryption_coefficient.Data_Size = 9;           //加密/解密数据大小
    Affine_cipher_encryption(enciphered_Data,&Encryption_coefficient);//仿射密码加密
    Affine_cipher_decode(Decrypt_Data,&Encryption_coefficient);//仿射密码解密

*/


