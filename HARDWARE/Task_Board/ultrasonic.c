#include "stm32f4xx.h"
#include "ultrasonic.h"
#include "delay.h"
#include "cba.h"
#include "NEW_Task_board.h"
#include "TaskBoard.h"
#include "canp_hostcom.h"
float Ultrasonic_Value = 0;
uint32_t Ultrasonic_Num = 0; // ����ֵ
uint16_t dis = 0;

/**
 * @description: ���ͳ�������ಢ��ȡ���
 * @param {*}
 * @return {*}
 */
int Ultrasonic_GetBuffer(void)
{
	int r = 0, _count;
	Ultrasonic_Ranging();
	while (dis == 0)
	{
		_count++;
		if (_count > 2000) //��ֹ��ѭ��1���޽��������ѭ��
		{
			break;
		}
		delay_ms(1);
	}
	r = dis;
	dis = 0;
	return r;
}
int Ultrasonic_GetBufferN()
{
	int i, j, b;
	int buf[10];
	for (i = 0; i < 10; i++)
	{
		buf[i] = Ultrasonic_GetBuffer();
		/*Send_Debug_num(buf[i]);
		Send_Debug_string("\n");*/
		delay_ms(200);
	}
	for (i = 0; i < 10; i++) //����
	{
		for (j = 0; j < 10 - i - 1; j++)
		{
			if (buf[j] > buf[j + 1])
			{
				b = buf[j];
				buf[j] = buf[j + 1];
				buf[j + 1] = b;
			}
		}
	}
	//ȡ�м���+ƫ��
	return (buf[5] + buf[6]) / 2;
}
void Ultrasonic_Port(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_SWJ);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_SWJ);
#if versions == 0
	//GPIOA15---INC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIOB4---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#if versions == 1
	//GPIOB4---INC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOA15---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#if versions == 2
	//GPIOA15---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#if versions == 3
	//GPIOA15---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#if versions == 4
	//GPIOB4---INC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOA15---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#if versions == 5
	//GPIOA15---INT0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

void Ultrasonic_TIM(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_InitStructure.TIM_Period = arr;
	TIM_InitStructure.TIM_Prescaler = psc;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// �˲����Ի�����ʱ����Ч
	// TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	// TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6, &TIM_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, DISABLE);
}

void Ultrasonic_EXTI()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

#if versions == 0 //�ⲿ�ж�4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);

	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if versions == 1												   //�ⲿ�ж�15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if versions == 2												   //�ⲿ�ж�15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if versions == 3												   //�ⲿ�ж�15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if versions == 4												   //�ⲿ�ж�15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if versions == 5												   //�ⲿ�ж�15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if IOversion == 0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if IOversion == 1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15); //�ж���15

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if IOversion == 2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4); //�ж���4

	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

}

void Ultrasonic_Init(void)
{
	Ultrasonic_Port();	   // ������Ӳ���˿ڳ�ʼ��
	Ultrasonic_TIM(9, 83); // ������������ʱ����ʼ��
	Ultrasonic_EXTI();	   // ���������������жϳ�ʼ��
}

/**********************************************************************
 * �� �� �� ��  ��������ࣨ���Σ�
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Ultrasonic_Ranging()
{
#if versions == 0
	INC = 1;
	delay_us(3);
	INC = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	INC = 1;
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif
#if versions == 1
	INC = 1;
	delay_us(3);
	INC = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	INC = 1;
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif
#if versions == 2
	Choose_CD4051_Gear(2);
	CD4051_COM = 1;
	delay_us(3);
	CD4051_COM = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	CD4051_COM = 1;

	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
//CD4051_COM = 0;
#endif
#if versions == 3
	Choose_CD4051_Gear(2);
	CD4051_COM = 1;
	delay_us(3);
	CD4051_COM = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	CD4051_COM = 1;

	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
//CD4051_COM = 0;
#endif
#if versions == 4
	INC = 1;
	delay_us(3);
	INC = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	INC = 1;
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif
#if versions == 5
	Choose_CD4051_Gear(2);
	CD4051_COM = 1;
	delay_us(3);
	CD4051_COM = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	CD4051_COM = 1;

	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
//CD4051_COM = 0;
#endif

#if IOversion == 0
	TaskBoard_INC = 1;
	delay_us(3);
	TaskBoard_INC = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	TaskBoard_INC = 1;
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif

#if IOversion == 1
	TaskBoard_CD4051_Chooce(2,1);
	delay_us(3);
	TaskBoard_CD4051_Chooce(2,0);

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	TaskBoard_CD4051_Chooce(2,1);
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif

#if IOversion == 2
	TaskBoard_INC = 1;
	delay_us(3);
	TaskBoard_INC = 0;

	TIM_Cmd(TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	Ultrasonic_Num = 0; // ��ʱ������
	delay_ms(30);		// �ȴ�һ��ʱ�䣬�ȴ����ͳ����������ź�
	TaskBoard_INC = 1;
	delay_ms(5);
	TIM_Cmd(TIM6, DISABLE);
#endif
}

void TIM6_DAC_IRQHandler()
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
	{
		Ultrasonic_Num++;
	}
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}

/***************************************************************************************************/
//�ⲿ�жϣ�������INC��
#if versions == 0
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = (float)Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
#endif
#if versions == 1
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = (float)Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if versions == 2
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if versions == 3
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if versions == 4
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = (float)Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if versions == 5
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if IOversion == 0
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if IOversion == 1
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
#endif
#if IOversion == 2
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == RESET)
		{
			TIM_Cmd(TIM6, DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value = (float)Ultrasonic_Value * 1.72f - 20.0f; // ������붨ʱ10us��S=Vt/2����2��������
			dis = (uint16_t)Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
#endif
