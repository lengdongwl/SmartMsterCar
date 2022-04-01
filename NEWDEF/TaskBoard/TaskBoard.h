/*
 * @Description: �����
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
#define TaskBoard_BEEP PFout(11)  //������
#define TaskBoard_INC PBout(4)    //���������
#define TaskBoard_INT0 PAint(15)  //����������
#define TaskBoard_RI_TXD PGout(8) //�������
#define SCK PHout(10)             //�����
#define RCK PHout(11)
#define SER PBout(15)
#define SMG PCout(13)
#endif

#if IOversion == 1
//LED CD4051 0��1
//BEEP CD4051  5
#define SCK PCout(13) //�����
#define RCK PFout(11)
#define SER PBout(9)
#define CD4051_COM PBout(15) //CD4051
#define CD4051_A PHout(10)
#define CD4051_B PHout(11)
#define CD4051_C PBout(4)
#endif

#if IOversion == 2
#define TaskBoard_LED_L PHout(10)  //LED��
#define TaskBoard_LED_R PHout(11)  //LED��
#define TaskBoard_BEEP PCout(13)   //������
#define TaskBoard_INC PAout(15)    //���������
#define TaskBoard_INT0 PBint(4)    //����������
#define TaskBoard_RI_TXD PFout(11) //������� \
                                   //�������

#endif

void TaskBoard_Init(void);

void TaskBoard_Time(int time);                                //��timeֵ����TIM2���е���ʱ ֱ��0
void TaskBoard_TimeITOpen(void);                              //�жϵ���ʱ
void TaskBoard_TimeClose(void);                             //�رյ���ʱ �����رղ�Ȼ��ʱ��Ӱ�����������ȶ�
uint8_t TaskBoard_TimeStatus(void);                           //0.����ʱ��� 1.����ʱ״̬ 2.���״̬

void TaskBoard_test(void);                                    //��������
uint32_t TaskBoard_WAVE(void);                                //��ȡ���������ֵ ��λ��mm
uint16_t TaskBoard_BH(void);                                  //��ȡ����ֵ
void TaskBoard_RISend(uint8_t *p, int n);                     //���ⷢ��
void TaskBoard_SMGDisplay(uint8_t wx, uint8_t number);        //�������ʾ
void TaskBoard_SMGDisplay2(uint8_t number1, uint8_t number2); //ˢ����ʾ2λ����
void TaskBoard_LED_show(uint8_t LeftOrRight, uint8_t status); //LED��ʾ
void TaskBoard_CD4051_Chooce(uint8_t N, uint8_t com);         //ѡ��CD4051ͨ��0-7
#endif

/**************************************************************
          �����İ�ӿڡ�
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
            G0�� IOversion=0
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
��׼�����	     _________	
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


