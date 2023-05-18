
#ifndef __NEW_SYN_H
#define __NEW_SYN_H
#include "sys.h"			
#include "NEW_Task_board.h"

#define Voice  0

extern uint8_t USART6_Flag;				//UART6������ɱ�־λ
extern uint8_t USART6_Back[9];				//���յ�����

//���������Ĳ�ͬ���ı�˿�
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

uint8_t SYN_Extern(void);																		//����ʶ��
uint8_t SYN73118_RST(void);
void USART6_INIT(uint32_t baudrate);												//USART6��ʼ��
void SYN_Init(void);																				//SYN��ʼ��
void USART6_SendChar(uint8_t SYN_Data);											//USART6����һ���ֽں���
void USART6_SendString(uint8_t* SYN_Data,uint8_t size);			//USART6�����ַ�������
void SYN7318_Play(char *Pst);																		//SYN�����ϳɲ��ź���
void SYN_Status_Query(void);																//��ѯģ��״̬
void TIM14_Int_Init(u16 arr,u16 psc);												//��ʱ��14��ʼ��		

uint8_t ASR(void);

#endif

