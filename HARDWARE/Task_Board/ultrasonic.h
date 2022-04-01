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
//使用CD4051控制
#endif
#if versions == 3
//使用CD4051控制
#endif
#if versions == 4
#define INC PBout(4)
#endif
#if versions == 5
//使用CD4051控制
#endif
//IOversion 初始化在TaskBoard.c

extern uint32_t Ultrasonic_Num; // 计数值

void Ultrasonic_Init(void);      //超声波测距初始化
void Ultrasonic_Ranging(void);   //发送请求
int Ultrasonic_GetBuffer(void);  //发送请求并等待数据回传
int Ultrasonic_GetBufferN(void); //多次测结果取中位数+偏差
extern uint16_t dis;

#endif
