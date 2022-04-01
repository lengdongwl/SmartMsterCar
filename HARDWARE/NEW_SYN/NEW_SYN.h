
#ifndef __NEW_SYN_H
#define __NEW_SYN_H
#include "sys.h"			
#include "NEW_Task_board.h"

#define Voice  0

extern uint8_t USART6_Flag;				//UART6接收完成标志位
extern uint8_t USART6_Back[9];				//接收的数据

//根据任务板的不同来改变端口
#if versions==0
#define SYN_RST PBout(9)
#endif
#if versions==1
#define SYN_RST PCout(13)
#endif
#if versions==2
#define SYN_RST PBout(9)
#endif
#if versions==3
#define SYN_RST PGout(8)
#endif
#if versions==4
#define SYN_RST PBout(9)
#endif
#if versions==5
#define SYN_RST PGout(8)
#endif

uint8_t SYN_Extern(void);																		//语言识别
uint8_t SYN73118_RST(void);
void USART6_INIT(uint32_t baudrate);												//USART6初始化
void SYN_Init(void);																				//SYN初始化
void USART6_SendChar(uint8_t SYN_Data);											//USART6发送一个字节函数
void USART6_SendString(uint8_t* SYN_Data,uint8_t size);			//USART6发送字符串函数
void SYN7318_Play(char *Pst);																		//SYN语音合成播放函数
void SYN_Status_Query(void);																//查询模块状态
void TIM14_Int_Init(u16 arr,u16 psc);												//定时器14初始化		

uint8_t ASR(void);

#endif

