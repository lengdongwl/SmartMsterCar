/*
 * @Description: 
 * @Autor: 309 Mushroom
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-11-02 22:13:53
 */
#include "stm32f4xx.h"
#include "sys.h"
#include "NEW_Task_board.h"

#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#if versions == 0
#define INC PAout(15)
#endif
#if versions == 1
#define INC PBout(4)
#endif
#if versions == 2
//ʹ��CD4051����
#endif
#if versions == 3
//ʹ��CD4051����
#endif
#if versions == 4
#define INC PBout(4)
#endif
#if versions == 5
//ʹ��CD4051����
#endif
//IOversion ��ʼ����TaskBoard.c

extern uint32_t Ultrasonic_Num; // ����ֵ

void Ultrasonic_Init(void);      //����������ʼ��
void Ultrasonic_Ranging(void);   //��������
int Ultrasonic_GetBuffer(void);  //�������󲢵ȴ����ݻش�
int Ultrasonic_GetBufferN(void); //��β���ȡ��λ��+ƫ��
extern uint16_t dis;

#endif
