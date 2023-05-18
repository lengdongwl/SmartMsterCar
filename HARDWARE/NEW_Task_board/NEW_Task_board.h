#ifndef __NEW_TASK_BOARD_H
#define __NEW_TASK_BOARD_H
#include "sys.h"
#include "delay.h"

#define Test_Daat 0
//�������Դ򿪲鿴����

#define versions 99
/**************************************************************
 0 ����׼�����
 1 ��2020 G1
 2 ��2020 G2
 3 ��2020 G3
 4 : 2020 G0
 5 : 2021 Z1
***************************************************************/

#define L_LED  0x01
#define R_LED  0x02

#if versions==0

#define Infrared_out PFout(11)

#endif
#if versions==1
#define SMG PFout(11)
#define Infrared_out PGout(8)
#define CD4051_COM 	PBout(15)
#define SCK PHout(11)
#define RCK PHout(10)
#define SER  PBout(15)  
#endif
#if versions==2
#define CD4051_COM 	PBout(15)
#define Infrared_out 	CD4051_COM
#define CD4051_A 	PHout(10)
#define CD4051_B 	PHout(11)
#define CD4051_C 	PBout(4)
#define SCK PFout(11)
#define RCK PGout(8)
#define SER  PCout(13)  
#endif
#if versions==3
#define CD4051_COM 	PBout(15)
#define Infrared_out 	CD4051_COM
#define CD4051_A 	PHout(10)
#define CD4051_B 	PHout(11)
#define CD4051_C 	PBout(4)
#define SCK PCout(13)
#define RCK PFout(11)
#define SER  PBout(9)  
#endif
#if versions==4
#define SMG PCout(13)
#define Infrared_out PGout(8)
#define SCK PHout(10)//��ʵ������Ϊ��׼������ԭ��ͼ����
#define RCK PHout(11)
#define SER PBout(15)  
#endif
#if versions==5
#define SCK PCout(13)
#define RCK PFout(11)
#define SER PBout(9)  
#define CD4051_COM 	PBout(15)
#define Infrared_out 	CD4051_COM
#define CD4051_A 	PHout(10)
#define CD4051_B 	PHout(11)
#define CD4051_C 	PBout(4)
#endif


void Task_board_Init(void);							          							//������ʼ��
void Tba_Beep_Config(void);																      //������������ʼ��
void Task_board_buzzer(uint8_t state);    											//��������������
void Tba_WheelLED_Config(void);                                 //�����ת��Ƴ�ʼ��
void Task_board_turn_light(uint8_t LorR,uint8_t swch);					//�����ת���
void Nixie_tube_595_Init(void);																	//����ܳ�ʼ��								
void CD4051_init(void);																					//CD4051��ʼ��		
void Choose_CD4051_Gear(uint8_t gear);													//CD4051ѡ��ͨ��
void SMG_display(uint8_t Data,uint8_t bit);											//�������ʾ
void Digital_Display(uint8_t One,uint8_t Two);  								//����ܶ�̬��ʾ
void TIM4_Int_Init(u16 arr,u16 psc);														//��ʱ��TIM4��ʼ��
void Digital_tube_Infrared_Send(uint8_t *s,int n);							//���ⷢ���ӳ���
void Tba_Photoresistance_Config(void);													//����������������
uint8_t Get_tba_phsis_value(void);															//��ȡ��������״̬

#endif

/**************************************************************
Z1									 _________	
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
��׼�����					 _________	
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
G1									 _________	
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
G2									 _________	
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
G3									 _________	
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
/**************************************************************
G0									 _________	
			PB9<-CHIPRST--|  O   O  |--SMG->PC13
	  	 PC6<-BL_RXD--|  O   O  |--LED_BEEP->PF11
			 PC7<-BL_TXD--|  O   O  |--RI_TXD->PG8
				 PB15<-SER--|  O   O  |--ADDR->PG15
			   PH10<-SCK--|  O   O  |--SDA->PB6
			   PH11<-RCK--|  O   O  |--SCL->PB7
				  PB4<-INC--|  O   O  |--+5V
			  PA15<-INT0--|  O   O  |--GND
                    |---------|

***************************************************************/








