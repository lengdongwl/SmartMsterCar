#include "new_syn.h"
#include "stm32f4xx.h"
#include "delay.h"
#include <string.h>
#include "canp_hostcom.h"
#include "function.h"

uint8_t USART6_receiving = 0;											  //���մ���
uint8_t USART6_Back[9];													  //���յ�����
uint8_t USART6_Flag;													  //UART6������ɱ�־λ
uint8_t TIM14_Flag = 0;													  //TIM14���ζ�ʱ��־λ
uint8_t TIM14_Overtime = 0;												  //TIM14��ζ�ʱ��־λ
uint8_t Start_Buf2[] = {0xFD, 0x00, 0x02, 0x10, 0x04};					  //���β��� ʹ���û��ʵ�2
uint8_t Stop_Buf[] = {0xFD, 0x00, 0x01, 0x11};							  //ֹͣ����ʶ��
uint8_t SYN_Random[8] = {0x55, 0x06, 0x20, 0x01, 0x00, 0x00, 0x21, 0xbb}; //�������
uint8_t Setting[11] = {0XFD, 0X00, 0X07, 0X1E, 0X03, 0X0F, 0XA0, 0X0F, 0XA0, 0X02};
uint8_t Uploading[8] = {0xAF, 0x06, 0x00, 0x02, 0x00, 0x00, 0x01, 0xBF}; //�ϴ������ж�
/*********************************************************************
�� ����     ��[=tiao2]�⵵[=dang3]"
������ѡ��[m3]���[m3]  ����
						[m51]���[m3] ��С��
						[m52]���[m3] ��Сǿ
						[m53]���[m3] ������
						[m54]���[m3] ����Ѽ
						[m55]���[m3] С����
���ִ���	[n0]����123456��123����[d] �Զ��ж�
						[n1]����123456��123����[d] ǿ�ƺϳɺ������ִ���һ��һ������
						[n2]����123456��123����[d] ǿ�ƺϳ���ֵ���ִ�����С��
������      ���[p1000]�ټ� ������ú���ʱ1s�ڶ��ټ�
�����ϣ�    [r0]��λ�ĵ�С������[d]	�Զ��ж���������
						[r1]��λ�ĵ�С������[d]	ǿ�ƿ�ͷ��������
						[r2]��λ�ĵ�С������[d]	�������ĺ���ǿ�ư�������
���٣�		  [s5]���[d] 5�����ٲ���
						[s8]���[d] 8�����ٲ���
						[s2]���[d] 2�����ٲ���
�����		  [t5]���[d] 5���������
						[t8]���[d] 8���������
						[t2]���[d] 2���������
�������ڣ�  [v5]���[d] 5����������
						[v8]���[d] 8����������
						[v2]���[d] 2����������
��ʾ����    [x0]sounda msga[d]	����������Ӣ����ĸ		 			
						[x1]sounda msga[d]  ��ʾ��
����1��			[y0]1008611[d]	�������������
						[y1]1008611[d]	һ�������һһ
�������  [f0]һ��������[d] һ��һ��
						[f0]һ��������[d] ƽ��ֱ��
�������ţ�[b0]��ӭ���٣�[d] ������̾��
						[b1]��ӭ���٣�[d] ��ӭ���� ��ͨ��
**********************************************************************/

/**********************************************************************
 * �� �� �� �� 	USART6��ʼ��
 * ��    �� ��  baudrate��������
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void USART6_INIT(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_TypeDefStructure;	  //GPIO�ṹ�嶨��
	USART_InitTypeDef USART_TypeDefStructure; //USART�ṹ�嶨��
	NVIC_InitTypeDef NVIC_InitStructure;	  //NVIC�ṹ�嶨��

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //����GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //����USART6ʱ��

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //USART6���õ�PC6
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //USART6���õ�PC7

	//PC6->Tx    PC7->RX
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //����PC6 PC7
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;			  //���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;		  //�������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;			  //����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;	  //100MHz
	GPIO_Init(GPIOC, &GPIO_TypeDefStructure);				  //GPIO��ʼ��
	//USART6
	USART_TypeDefStructure.USART_BaudRate = baudrate; //������
	USART_TypeDefStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;								   //��Ӳ��������
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //�����뷢��ģʽ
	USART_TypeDefStructure.USART_Parity = USART_Parity_No;			   //��У��λ
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;		   //ֹͣλ1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;	   //����λ8λ
	USART_Init(USART6, &USART_TypeDefStructure);					   //USART��ʼ��

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); //��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;		  //����6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;		  //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //NVIC��ʼ��
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART6, ENABLE);				  //ʹ��USART6
	USART_ClearFlag(USART6, USART_FLAG_TC);	  //���������ɱ�־λ
	USART_ClearFlag(USART6, USART_FLAG_RXNE); //���������ɱ�־λ
}

/**********************************************************************
 * �� �� �� �� 	SYN��ʼ��
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��SYN7318��λ���ų�ʼ����
*****************************************************************/
void SYN_Init(void)
{
	USART6_INIT(115200);				 //USART6��ʼ��
	TIM14_Int_Init(10000 - 1, 8400 - 1); //TIM14��ʼ��
	USART6_SendChar(0xFD);				 //����֡ͷ�������һ������һ���Ǵ�ģ�
#if versions == 0						 //��׼�����
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // Ĭ��Ϊ�ߵ�ƽ
#endif
#if versions == 1 //2020 G1
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//PC13 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_13); // Ĭ��Ϊ�ߵ�ƽ
#endif
#if versions == 2 //2020 G2
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // Ĭ��Ϊ�ߵ�ƽ
#endif
#if versions == 3 //2020 G3
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//PG8 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8); // Ĭ��Ϊ�ߵ�ƽ
#endif
#if versions == 4 //2020 G0
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // Ĭ��Ϊ�ߵ�ƽ
#endif
#if versions == 5 //2021 Z1
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//PG8 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8); // Ĭ��Ϊ�ߵ�ƽ
#endif
	//USART6_SendString(Setting,10);
}

/**********************************************************************
 * �� �� �� �� 	USART6����һ���ֽں���
 * ��    �� ��  SYN_Data����Ҫ���͵����ݣ��ֽڣ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void USART6_SendChar(uint8_t SYN_Data)
{
	USART_SendData(USART6, SYN_Data); //�򴮿�6��������
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) != SET)
		; //�ȴ����ͽ���
}

/**********************************************************************
 * �� �� �� �� 	USART6�����ַ�������
 * ��    �� ��   SYN_Data����Ҫ���͵����ݣ��ַ����� size����С
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void USART6_SendString(uint8_t *SYN_Data, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		USART6_SendChar(SYN_Data[i]); //ѭ�����͵��ֽ�
	}
}

/**********************************************************************
 * �� �� �� �� 	SYN�����ϳɲ��ź���
 * ��    �� ��  Pst�����Ҫ�ϳɲ��ŵ��ı���������
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��						 
 * ��    ע ��  SYN_TTS("���");
*****************************************************************/
void SYN7318_Play(char *Pst)
{
	uint8_t Length;
	uint8_t Frame[5]; //���淢�����������

	Length = strlen((char *)Pst);
	Frame[0] = 0xFD;	   //֡ͷ
	Frame[1] = 0x00;	   //���ݳ���
	Frame[2] = Length + 2; //���ݳ���
	Frame[3] = 0x01;	   //�����ϳɲ�������
	Frame[4] = 0x00;	   //���ű����ʽΪ��GB2312��

	TIM_Cmd(TIM14, ENABLE);					   //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
	USART6_SendString(Frame, 5);			   //��֡ͷ+���ݳ���+����
	USART6_SendString((uint8_t *)Pst, Length); //��������
	while (!(USART6_Back[3] == 0x41))		   //ģ����ճɹ��ж�
	{
		//Send_Debug_HEX(USART6_Back[3]);
		if (TIM14_Overtime == 1) //��ʱ
		{
			break;
		}
	}
	while (!(USART6_Back[3] == 0x4f)) //ģ������ж�
	{
		Send_Debug_HEX(USART6_Back[3]);
		if (TIM14_Overtime == 1) //��ʱ
		{
			break;
		}
	}
	TIM_Cmd(TIM14, DISABLE); //ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
	TIM14_Flag = 0;			 //�����ʱ��14��ʱ��־λ
	delay_ms(200);
}

 //SYN7318
/**********************************************************************
 * �� �� �� �� 	USART6�жϷ������
 * ��    �� ��  baudrate��������
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void USART6_IRQHandler(void)
{
	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET) //����ж�
	{
		Send_Debug_HEX(USART_ReceiveData(USART6));//����

		
		USART6_Back[USART6_receiving] = USART_ReceiveData(USART6); //USART6�յ������ݻ�ȡ
		if (USART6_Back[0] == 0xFC)								   //�жϱ�׼֡ͷ
		{
			USART6_receiving += 1;
			if (USART6_receiving > USART6_Back[2] + 2)
			{
				//				 Send_Debug_Info((uint8_t*)"\n",2);//����
				USART6_Flag = 1;
				USART6_receiving = 0;
			}
		}
		else if (USART6_Back[USART6_receiving] == 0x55) //�жϸ�λ����
		{
			USART6_receiving += 1;
			if (USART6_receiving > 4)
			{
				//					Send_Debug_Info((uint8_t*)"\n",2);//����
				USART6_Flag = 1;
				USART6_receiving = 0;
			}
		}
		else //֡����
		{
			USART6_receiving = 0;
		}
	}
	USART_ClearITPendingBit(USART6, USART_IT_RXNE); //����жϱ�־λ
}


/**********************************************************************
 * �� �� �� �� 	ģ��״̬��ѯ����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void SYN_Status_Query(void)
{
	uint8_t Frame[4]; // ���淢�����������

	Frame[0] = 0xFD; // ֡ͷ
	Frame[1] = 0x00; //���ݳ���
	Frame[2] = 0x01; //���ݳ���
	Frame[3] = 0x21; // ״̬��ѯ����

	TIM_Cmd(TIM14, ENABLE);			  //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
	USART6_SendString(Frame, 4);	  //����ģ��״̬��ѯָ��
	while (!(USART6_Back[3] == 0x41)) //ģ����ճɹ��ж�
	{
		//ģ�鴦������ʶ���������ѣ������ϳɻ��յ�����ʶ�������֡
		if (USART6_Back[3] == 0x42 || USART6_Back[3] == 0x45 || USART6_Back[3] == 0x45)
		{
			SYN73118_RST();				 //����ģ�鸴λ
			USART6_SendString(Frame, 4); //����ģ��״̬��ѯָ��
		}
		if (TIM14_Overtime == 1) //��ʱ
		{
			break;
		}
	}
	TIM_Cmd(TIM14, DISABLE); //ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
	TIM14_Flag = 0;			 //�����ʱ��14��ʱ��־λ
}

/**********************************************************************
 * �� �� �� �� 	����ʶ��
 * ��    �� ��  ��
 * �� �� ֵ ��  ʶ�𵽵�ָ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
uint8_t SYN_Extern(void)
{
#if Voice == 0

	uint8_t Order = 0;
	TIM_Cmd(TIM14, ENABLE); //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
	SYN7318_Play("��ʼ");
	SYN_Status_Query();				//��ѯģ��״̬
	USART6_SendString(Stop_Buf, 4); //ֹͣ����ʶ��
	
	for (int i = 0; i < 4; i++)
	{
		USART6_SendString(Start_Buf2, 5); //���Ϳ�ʼ����ʶ��
		TIM_Cmd(TIM14, ENABLE);			  //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
		while (!(USART6_Back[3] == 0x41)) //ģ����ճɹ��ж�
		{
			if (TIM14_Overtime == 1) //��ʱ
			{
				return 0xFF; //��ʱ����ֵ
			}
		}
		TIM14_Flag = 0;					//�����ʱ��14��ʱ��־λ
		TIM_Cmd(TIM14, DISABLE);		//ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
		for (uint8_t i = 0; i < 9; i++) //������ݻ�����
		{
			USART6_Back[i] = 0;
		}
		Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
		Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
		delay_ms(100);
		delay_ms(500); //��ʱԽ��Ч��Խ��
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500); //���ǲ�Ҫ̫��

		if (USART6_Back[3] == 0x01)
		{
			switch (USART6_Back[8]) //�ж�ʶ�𵽵�����
			{
			case 0x01:
			{
				SYN7318_Play("��������");
				i = 5;
				Order = 0x02;
				break;
			}
			case 0x02:
			{
				SYN7318_Play("����ɽ��");
				i = 5;
				Order = 0x03;
				break;
			}
			case 0x03:
			{
				SYN7318_Play("׷������");
				Order = 0x04;
				i = 5;
				break;
			}
			case 0x04:
			{
				SYN7318_Play("�﷫��");
				Order = 0x05;
				i = 5;
				break;
			}
			case 0x05:
			{
				SYN7318_Play("��ͷ����");
				Order = 0x06;
				i = 5;
				break;
			}
			default:
				break;
			}
		}
		else
		{
			SYN7318_Play("ʶ��ʧ��");
			Order = 0x01;
		}
		USART6_Back[8] = 0;				//������ݻ���
		USART6_SendString(Stop_Buf, 4); //ֹͣ����ʶ��
	}
	Uploading[2] = Order;
	Send_ZigbeeData_To_Fifo(Uploading, 8);
	Send_ZigbeeData_To_Fifo(Uploading, 8);

	TIM_Cmd(TIM14, DISABLE); //ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
	TIM14_Flag = 0;			 //�����ʱ��14��ʱ��־λ
	delay_ms(200);
	return Order;
#endif
#if Voice == 1
	return ASR(); //С������
#endif
}

/**********************************************************************
 * �� �� �� ��  ͨ�ö�ʱ��14�жϳ�ʼ��
 * ��    �� ��  arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us. Ft=��ʱ������Ƶ��,��λ:Mhz
*****************************************************************/
void TIM14_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; //��ʱ���ṹ�嶨��
	NVIC_InitTypeDef NVIC_InitStructure;			   //NVIC�ṹ�嶨��

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); //ʹ��TIM14ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr;						//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStructure); //��ʼ��TIM14

	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); //����ʱ��14�����ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn; //��ʱ��14�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	  //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;			  //�����ȼ�10
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				  //ʹ���ж�����
	NVIC_Init(&NVIC_InitStructure);								  //�жϳ�ʼ��
}

/**********************************************************************
 * �� �� �� ��  ��ʱ��14�жϷ�����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET) //����ж�
	{
		TIM14_Flag += 1;
		if (TIM14_Flag > 20) //20s��ʱ
		{
			TIM14_Overtime = 1; //�����־λ
			TIM14_Flag = 0;		//�����־λ
		}
	}
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update); //����жϱ�־λ
}

/**********************************************************************
 * �� �� �� ��  ����ģ�鸴λ
 * ��    �� ��  ��
 * �� �� ֵ ��  ��λ�ɹ���1�� ��λʧ�ܣ�0��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ������ģ��Ӹ����͸�λ��
*****************************************************************/
uint8_t SYN73118_RST(void)
{
	SYN_RST = SET; //�ø�
	delay_ms(10);
	SYN_RST = RESET; //�õ�
	delay_ms(100);
	SYN_RST = SET;					  //�ø�
	TIM_Cmd(TIM14, ENABLE);			  //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
	while (!(USART6_Back[3] == 0x55)) //��λ�ɹ��ж�
	{
		if (TIM14_Overtime == 1) //��ʱ
		{
			return 0;
		}
	}
	while (!(USART6_Back[3] == 0x4A)) //���ģ�����
	{
		if (TIM14_Overtime == 1) //��ʱ
		{
			return 0;
		}
	}
	USART6_Back[3] = 0;
	TIM_Cmd(TIM14, DISABLE); //ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
	TIM14_Flag = 0;			 //�����ʱ��14��ʱ��־λ
	return 1;
}

/**********************************************************************
 * �� �� �� ��  С������
 * ��    �� ��  ��
 * �� �� ֵ ��  �������
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
uint8_t ASR(void)
{
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xA1);
	Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
	Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
	uint16_t Counter_Fifo = 0;
	uint8_t Temp_flag = 0;
	USART6_Flag = 0;
	while (1)
	{
		if (USART6_Flag == 1)
		{
			if (USART6_Back[1] == 0x02 && USART6_Back[3] == 0x00)
			{
				switch (USART6_Back[2])
				{
				case 1:
				{
					Uploading[2] = 2;
					break;
				}
				case 2:
				{
					Uploading[2] = 3;
					break;
				}
				case 3:
				{
					Uploading[2] = 4;
					break;
				}
				case 4:
				{
					Uploading[2] = 5;
					break;
				}
				case 5:
				{
					Uploading[2] = 6;
					break;
				}
				}
				Send_ZigbeeData_To_Fifo(Uploading, 8);
				Send_ZigbeeData_To_Fifo(Uploading, 8);
				return Uploading[2];
			}
		}
		delay_ms(100);
		Counter_Fifo += 1;
		if (Counter_Fifo > 30)
		{
			Counter_Fifo = 0;
			if (Temp_flag >= 4)
			{
				break;
			}
			Temp_flag += 1;
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xA1);
			Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
			Send_ZigbeeData_To_Fifo(SYN_Random, 8); // �������
		}
	}
	return 0;
}
