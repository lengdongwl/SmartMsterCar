/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-15 09:42:21
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-11-22 20:15:05
 */
#ifndef __SYN7318_H
#define __SYN7318_H
#include "stm32f4xx.h"
#include "sys.h"

#define SYN7318RESET PBout(5)

void SYN7318_Init(void);
void SYN7318_Play(char *Pst);//�����ϳɲ��ź���
void SYN7318_Play_NotAscll(char *Pst);//�����ϳɲ��ź��� ���˷������ַ�
void SYN7318_Test( void);					// ������������
void SYN_7318_One_test(uint8_t mode,uint8_t num);		// mode ģʽ���� 1 ���ָ�� 0 ָ��ָ��
void Yu_Yin_Asr(void);						// ����ʶ������

uint8_t SYN7318_Extern(void);		// ����ʶ��
uint32_t SYN_Extern_wait(uint8_t count);//�ȴ�SYN7318����ʶ����ɲ��ϴ�����

#endif

