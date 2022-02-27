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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // 输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	RI_TXD = 1;
#endif
#if versions == 1
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOG8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // 输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8);
#endif
#if versions == 2
//CD4051控制
#endif
#if versions == 3
//CD4051控制
#endif
#if versions == 4
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//GPIOG8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // 输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8);
#endif
#if versions == 5
//CD4051控制
#endif
}

/***************************************************************
** 功能：     红外发射子程序
** 参数：	  *s：指向要发送的数据
**             n：数据长度
** 返回值：    无
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
			if (temp == 0) //发射0
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //延时0.5ms
				TaskBoard_RI_TXD = 1;
				delay_us(500); //延时0.5ms
			}
			if (temp == 1) //发射1
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //延时0.5ms
				TaskBoard_RI_TXD = 1;
				delay_ms(1);
				delay_us(800); //延时1.69ms
			}
		}
	}
	TaskBoard_RI_TXD = 0; //结束
	delay_us(560);		  //延时0.56ms
	TaskBoard_RI_TXD = 1; //关闭红外发射
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
			if (temp == 0) //发射0
			{
				TaskBoard_CD4051_Chooce(6, 0);
				delay_us(500); //延时0.5ms
				TaskBoard_CD4051_Chooce(6, 1);
				delay_us(500); //延时0.5ms
			}
			if (temp == 1) //发射1
			{
				TaskBoard_CD4051_Chooce(6, 0);
				delay_us(500); //延时0.5ms
				TaskBoard_CD4051_Chooce(6, 1);
				delay_ms(1);
				delay_us(800); //延时1.69ms
			}
		}
	}
	TaskBoard_CD4051_Chooce(6, 0); //结束
	delay_us(560);				   //延时0.56ms
	TaskBoard_CD4051_Chooce(6, 1); //关闭红外发射
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
			if (temp == 0) //发射0
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //延时0.5ms
				TaskBoard_RI_TXD = 1;
				delay_us(500); //延时0.5ms
			}
			if (temp == 1) //发射1
			{
				TaskBoard_RI_TXD = 0;
				delay_us(500); //延时0.5ms
				TaskBoard_RI_TXD = 1;
				delay_ms(1);
				delay_us(800); //延时1.69ms
			}
		}
	}
	TaskBoard_RI_TXD = 0; //结束
	delay_us(560);		  //延时0.56ms
	TaskBoard_RI_TXD = 1; //关闭红外发射
#endif
	//Digital_tube_Infrared_Send(s,n);//转接到新的发射红外程序
}
