/*
 * @Description: 任务版
 * @Autor: 309 Mushroom
 * @Date: 2021-10-27 15:14:41
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-21 14:26:37
 */
#ifndef __TaskBoard_H
#define __TaskBoard_H
#include "sys.h"
#include "stm32f4xx.h"
#define IOversion 2

#if IOversion == 0
#define TaskBoard_LED PFout(11)   //LED
#define TaskBoard_BEEP PFout(11)  //蜂鸣器
#define TaskBoard_INC PBout(4)    //超声波输出
#define TaskBoard_INT0 PAint(15)  //超声波输入
#define TaskBoard_RI_TXD PGout(8) //红外输出
#define SCK PHout(10)             //数码管
#define RCK PHout(11)
#define SER PBout(15)
#define SMG PCout(13)
#endif

#if IOversion == 1
//LED CD4051 0与1
//BEEP CD4051  5
#define SCK PCout(13) //数码管
#define RCK PFout(11)
#define SER PBout(9)
#define CD4051_COM PBout(15) //CD4051
#define CD4051_A PHout(10)
#define CD4051_B PHout(11)
#define CD4051_C PBout(4)
#endif

#if IOversion == 2
#define TaskBoard_LED_L PHout(10)  //LED左
#define TaskBoard_LED_R PHout(11)  //LED右
#define TaskBoard_BEEP PCout(13)   //蜂鸣器
#define TaskBoard_INC PAout(15)    //超声波输出
#define TaskBoard_INT0 PBint(4)    //超声波输入
#define TaskBoard_RI_TXD PFout(11) //红外输出 \
                                   //数码管无

#endif

void TaskBoard_Init(void);

void TaskBoard_Time(int time);                                //将time值利用TIM2进行倒计时 直到0
void TaskBoard_TimeITOpen(void);                              //中断倒计时
void TaskBoard_TimeClose(void);                             //关闭倒计时 尽量关闭不然定时器影响其他程序稳定
uint8_t TaskBoard_TimeStatus(void);                           //0.倒计时完成 1.倒计时状态 2.打断状态

void TaskBoard_test(void);                                    //任务版测试
uint32_t TaskBoard_WAVE(void);                                //读取超声波测距值 单位：mm
uint16_t TaskBoard_BH(void);                                  //读取光照值
void TaskBoard_RISend(uint8_t *p, int n);                     //红外发射
void TaskBoard_SMGDisplay(uint8_t wx, uint8_t number);        //数码管显示
void TaskBoard_SMGDisplay2(uint8_t number1, uint8_t number2); //刷新显示2位数据
void TaskBoard_LED_show(uint8_t LeftOrRight, uint8_t status); //LED显示
void TaskBoard_CD4051_Chooce(uint8_t N, uint8_t com);         //选择CD4051通道0-7
#endif

/**************************************************************
          【核心板接口】
            _________	
      PB9--|  O   O  |--PC13
USART6 TX--|  O   O  |--PF11
USART6 RX--|  O   O  |--PG8
     PB15--|  O   O  |--PG15
     PH10--|  O   O  |--PB6(I2C1 SCL)
     PH11--|  O   O  |--PB7(I2C1 SDA)
      PB4--|  O   O  |--+5V
     PA15--|  O   O  |--GND
            ---------- 
**************************************************************/
/**************************************************************
            G0板 IOversion=0
            _________	
  CHIPRST--|  O   O  |--SMG
   BL RXD--|  O   O  |--LED BEEP
   BL TXD--|  O   O  |--RI TXD
      SER--|  O   O  |--ADDR
      RCK--|  O   O  |--SDA
      SCK--|  O   O  |--SCL
      INC--|  O   O  |--+5
     INT0--|  O   O  |--GND
            ---------- 
**************************************************************/

/**************************************************************
Z1               _________	
      PB9<-SER--|  O   O  |--SCK->PC13
   PC6<-BL_RXD--|  O   O  |--RCK->PF11
   PC7<-BL_TXD--|  O   O  |--CHIPRST->PG8
     PB15<-COM--|  O   O  |--ADDR->PG15
       PH10<-A--|  O   O  |--SDA->PB6
       PH11<-B--|  O   O  |--SCL->PB7
        PB4<-C--|  O   O  |--+5V
    PA15<-INT0--|  O   O  |--GND
                |---------|

***************************************************************/
/**************************************************************
标准任务板	     _________	
     PB9<-RST--|  O   O  |--BEEP->PC13
  PC6<-BL_RXD--|  O   O  |--RI_TXD->PF11
  PC7<-BL_TXD--|  O   O  |--R_OUT->PG8
     PB15<-OE--|  O   O  |--ADDR->PG15
  PH10<-LED_L--|  O   O  |--SCL->PB6
  PH11<-LED_R--|  O   O  |--SDA->PB7
    PB4<-INT0--|  O   O  |--+5V
    PA15<-INC--|  O   O  |--GND
               |---------|

***************************************************************/
/**************************************************************
G1		    _________	
PB9<-LED-BEEP--|  O   O  |--CHIRST->PC13
  PC6<-BL_RXD--|  O   O  |--SMG->PF11
  PC7<-BL_TXD--|  O   O  |--RI_TXD->PG8
    PB15<-SER--|  O   O  |--SDA->PG15
    PH10<-RCK--|  O   O  |--ADDR->PB6
    PH11<-SCK--|  O   O  |--SCL->PB7
     PB4<--INC-|  O   O  |--+5V
   PA15<-INT0--|  O   O  |--GND
               |---------|

***************************************************************/
/**************************************************************
G2		  _________	
PB9<-CHIRST--|  O   O  |--SER->PC13
PC6<-BL_RXD--|  O   O  |--SCK->PF11
PC7<-BL_TXD--|  O   O  |--RCK->PG8
  PB15<-COM--|  O   O  |--ADDR->PG15
    PH10<-A--|  O   O  |--SDA->PB6
    PH11<-B--|  O   O  |--SCL->PB7
     PB4<-C--|  O   O  |--+5V
 PA15<-INT0--|  O   O  |--GND
             |---------|

***************************************************************/
/**************************************************************
G3	        _________	
   PB9<-SER--|  O   O  |--SCK->PC13
PC6<-BL_RXD--|  O   O  |--RCK->PF11
PC7<-BL_TXD--|  O   O  |--CHIRST->PG8
  PB15<-COM--|  O   O  |--ADDR->PG15
    PH10<-A--|  O   O  |--SDA->PB6
    PH11<-B--|  O   O  |--SCL->PB7
     PB4<-C--|  O   O  |--+5V
 PA15<-INT0--|  O   O  |--GND
             |---------|

***************************************************************/


