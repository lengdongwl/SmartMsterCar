#ifndef __ATITHMETIC_H
#define __ATITHMETIC_H
#include "sys.h"


#define LZW 0 //LZW�����㷨��1������ 0���رգ�

#if 0
typedef struct
{
    uint8_t number_a;           //��ϵ��a(����)
    uint8_t number_b;           //��ϵ��b(����)
    uint8_t capital_letter_a;   //��ϵ��a(��д��ĸ)
    uint8_t capital_letter_b;   //��ϵ��b(��д��ĸ)
    uint8_t lowercase_a;        //��ϵ��a(Сд��ĸ)
    uint8_t lowercase_b;        //��ϵ��b(Сд��ĸ)
    uint8_t Data_Size;          //����/�������ݴ�С
    uint8_t amount_m;           //��ĸ����Ŀ
    uint8_t number_m;           //���ֵ���Ŀ
}Coefficient;//�����������/���ܼ���ϵ��

uint8_t Arithmetic_coding(uint8_t* coding,uint8_t size);										//��������
uint16_t CRC_16_MODBUS(uint8_t* check_code,uint8_t length);									//CRCУ��
uint16_t RSA_encryption(uint16_t message, uint16_t e_d, uint16_t N);				//RSA����/����
uint16_t relatively_prime(uint16_t A, uint16_t B);													//���Լ����/�������ж�
uint8_t prime_number(uint16_t X);																						//�����ж�
void ence_code_encryption(uint8_t size);																		//դ������ܣ�2����
void ence_code_decode(uint8_t size);																				//դ������ܣ�2����
void Polybius_Square_encryption(uint8_t* data, uint8_t size);								//�����Ȱ�˹�����������
void Polybius_Square_decode(uint8_t* data, uint8_t size);										//�����Ȱ�˹�����������
uint8_t multiplicative_inverse(uint8_t a, uint8_t m);												//�˷���Ԫ����
void Affine_cipher_encryption(uint8_t* parameter,Coefficient* calculate);		//�����������
void Affine_cipher_decode(uint8_t* parameter, Coefficient* calculate);			//�����������
#endif

#if LZW==1	//LZW�㷨
extern uint8_t LZW_Code_Stream[20];				//LZW�������
void LZW_Coding(uint8_t *LZW_String);																				//LZW����
uint8_t String_len(uint8_t* temp);																					//�ַ�����С���
#endif 

uint8_t ASCLL_conversion(char n1,char n2);																	//ASCLL��ת��ʮ������0xXX
uint8_t Extract_letter_figure(char* parameter, uint8_t size, uint8_t Mode); //�ַ�����ȡ��ĸ/����
uint8_t Seek_biggest_minimum(char* Data_value, uint8_t size,uint8_t Mode);	//��ȡ�ַ���������С����ĸ

#endif
/*
		//��������
    Coefficient Encryption_coefficient;
    Encryption_coefficient.number_a = 13;           //��ϵ��a(����)
    Encryption_coefficient.number_b = 3;            //��ϵ��b(����)
    Encryption_coefficient.capital_letter_a = 5;    //��ϵ��a(��д��ĸ)
    Encryption_coefficient.capital_letter_b = 8;    //��ϵ��b(��д��ĸ)
    Encryption_coefficient.lowercase_a = 19;        //��ϵ��a(Сд��ĸ)
    Encryption_coefficient.lowercase_b = 7;         //��ϵ��b(Сд��ĸ)
    Encryption_coefficient.amount_m = 26;           //��ĸ����Ŀ
    Encryption_coefficient.number_m = 10;           //���ֵ���Ŀ
    Encryption_coefficient.Data_Size = 9;           //����/�������ݴ�С
    Affine_cipher_encryption(enciphered_Data,&Encryption_coefficient);//�����������
    Affine_cipher_decode(Decrypt_Data,&Encryption_coefficient);//�����������

*/


