#ifndef __NEW_TASK_BOARD_H
#define __NEW_TASK_BOARD_H
#include "sys.h"
#include "delay.h"

#define Test_Daat 0
//任务板测试打开查看参数

#define versions 99
/**************************************************************
 0 ：标准任务板
 1 ：2020 G1
 2 ：2020 G2
 3 ：2020 G3
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
#define SCK PHout(10)//以实际连接为标准！！（原理图有误）
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


void Task_board_Init(void);							          							//任务板初始化
void Tba_Beep_Config(void);																      //任务板蜂鸣器初始化
void Task_board_buzzer(uint8_t state);    											//任务板蜂鸣器控制
void Tba_WheelLED_Config(void);                                 //任务板转向灯初始化
void Task_board_turn_light(uint8_t LorR,uint8_t swch);					//任务板转向灯
void Nixie_tube_595_Init(void);																	//数码管初始化								
void CD4051_init(void);																					//CD4051初始化		
void Choose_CD4051_Gear(uint8_t gear);													//CD4051选择通道
void SMG_display(uint8_t Data,uint8_t bit);											//数码管显示
void Digital_Display(uint8_t One,uint8_t Two);  								//数码管动态显示
void TIM4_Int_Init(u16 arr,u16 psc);														//定时器TIM4初始化
void Digital_tube_Infrared_Send(uint8_t *s,int n);							//红外发射子程序
void Tba_Photoresistance_Config(void);													//任务板光敏电阻配置
uint8_t Get_tba_phsis_value(void);															//获取光敏电阻状态

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
标准任务板					 _________	
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








