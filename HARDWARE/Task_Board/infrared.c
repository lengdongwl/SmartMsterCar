#include "stm32f4xx.h"
#include "infrared.h"
#include "delay.h"

void Infrared_Init()
{
#if versions == 0
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//GPIOF11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	RI_TXD = 1;
#endif
#if versions == 1
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOG8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8);
#endif
#if versions == 2
//CD4051����
#endif
#if versions == 3
//CD4051����
#endif
#if versions == 4
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOG8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // �������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8);
#endif
#if versions == 5
//CD4051����
#endif
}

/***************************************************************
** ���ܣ�     ���ⷢ���ӳ���
** ������	  *s��ָ��Ҫ���͵�����
**             n�����ݳ���
** ����ֵ��    ��
****************************************************************/
void Infrared_Send(uint8_t *s, int n)
{
	uint8_t i, j, temp;
#if IOversion == 0

	TaskBoard_RI_TXD = 0;
	delay_ms(9);
	TaskBoard_RI_TXD = 1;
	delay_ms(4);
	delay_us(560);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 8; j++)
		{
			temp = (s[i] >> j) & 0x01;
			if (temp == 0) //����0
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //��ʱ0.5ms
				TaskBoard_RI_TXD = 1;
				delay_us(500); //��ʱ0.5ms
			}
			if (temp == 1) //����1
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //��ʱ0.5ms
				TaskBoard_RI_TXD = 1;
				delay_ms(1);
				delay_us(800); //��ʱ1.69ms
			}
		}
	}
	TaskBoard_RI_TXD = 0; //����
	delay_us(560);		  //��ʱ0.56ms
	TaskBoard_RI_TXD = 1; //�رպ��ⷢ��
#endif
#if IOversion == 1
	//Z0 : TaskBoard_CD4051_Chooce(uint8_t N,uint8_t com) 6
	TaskBoard_CD4051_Chooce(6, 0);
	delay_ms(9);
	TaskBoard_CD4051_Chooce(6, 1);
	delay_ms(4);
	delay_us(560);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 8; j++)
		{
			temp = (s[i] >> j) & 0x01;
			if (temp == 0) //����0
			{
				TaskBoard_CD4051_Chooce(6, 0);
				delay_us(500); //��ʱ0.5ms
				TaskBoard_CD4051_Chooce(6, 1);
				delay_us(500); //��ʱ0.5ms
			}
			if (temp == 1) //����1
			{
				TaskBoard_CD4051_Chooce(6, 0);
				delay_us(500); //��ʱ0.5ms
				TaskBoard_CD4051_Chooce(6, 1);
				delay_ms(1);
				delay_us(800); //��ʱ1.69ms
			}
		}
	}
	TaskBoard_CD4051_Chooce(6, 0); //����
	delay_us(560);				   //��ʱ0.56ms
	TaskBoard_CD4051_Chooce(6, 1); //�رպ��ⷢ��
#endif
#if IOversion == 2

	TaskBoard_RI_TXD = 0;
	delay_ms(9);
	TaskBoard_RI_TXD = 1;
	delay_ms(4);
	delay_us(560);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 8; j++)
		{
			temp = (s[i] >> j) & 0x01;
			if (temp == 0) //����0
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //��ʱ0.5ms
				TaskBoard_RI_TXD = 1;
				delay_us(500); //��ʱ0.5ms
			}
			if (temp == 1) //����1
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //��ʱ0.5ms
				TaskBoard_RI_TXD = 1;
				delay_ms(1);
				delay_us(800); //��ʱ1.69ms
			}
		}
	}
	TaskBoard_RI_TXD = 0; //����
	delay_us(560);		  //��ʱ0.56ms
	TaskBoard_RI_TXD = 1; //�رպ��ⷢ��
#endif
	//Digital_tube_Infrared_Send(s,n);//ת�ӵ��µķ���������
}
