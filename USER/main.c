#include "delay.h"
#include "cba.h"
#include "canp_hostcom.h"
#include "power_check.h"
#include "function.h"
#include "Voluntarily.h"
#include "MasterCarCotrol.H"
#include "Timer.h"
#include "CanP_HostCom.h"
#include "Hard_Can.h"
#include "syn7318.h"
#include "bkrc_voice.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "Timer.h"
#include "rc522.h"
#include "TaskBoard.h"
void keyScan(void)
{
	if (S1 == 0)
	{
		delay_ms(20);
		if (S1 == 0)
		{
			LED1 = SET;
			while (!S1)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x01);
		}
	}
	if (S2 == 0)
	{
		delay_ms(20);
		if (S2 == 0)
		{
			LED2 = SET;
			while (!S2)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x02);
		}
	}
	if (S3 == 0)
	{
		delay_ms(20);
		if (S3 == 0)
		{
			LED3 = SET;
			while (!S3)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x03);
		}
	}
	if (S4 == 0)
	{
		delay_ms(20);
		if (S4 == 0)
		{
			LED4 = SET;
			while (!S4)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x04);
		}
	}
}

void HardInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); // 中断分组

	/*
	Task_board_Init();								// 任务板初始化
	Infrared_Init();								// 红外初始化
	
	BH1750_Configure();								// BH1750初始化配置
	roadway_check_TimInit(999, 167);				// 路况检测
	*/
	Ultrasonic_Init();		// 超声波初始化
	delay_init(168);		// 延时初始化
	Cba_Init();				// 核心板初始化
	Hard_Can_Init();		// CAN总线初始化
	Electricity_Init();		// 电量检测初始化
	UartA72_Init();			// A72硬件串口通讯初始化
	Can_check_Init(7, 83);	// CAN总线定时器初始化 TIM7
	Timer_Init(999, 167);	// 串行数据通讯时间帧 TIM10
	Readcard_daivce_Init(); // RFID初始化
	//TIM3_Int_Init(10000 - 1, 8400 - 1);				// 定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1s
	//TIM4_Int_Init(100 - 1, 8400 - 1);				// 定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数100次为10ms
	//while(RNG_Init());	 																// 初始化随机数发生器
	//SYN_Init();										// 语音识别初始化
	//SYN7318_Init();//syn7318语音模块
	BKRC_Voice_Init();

	Tim5_Init(1, 839);	 //WIFI接收识别数据初始化
	Tim2_Init(20, 8399); //任务版使用
	Tim4_Init(1, 8399);	 //zigbee接收处理
	Tim8_Init(10,8399); //特殊循迹
	TaskBoard_Init();
	/*add*/
}

int main(void)
{
	/*Hardware_Init();	   // 硬件初始化
	
	//Tim2_Init(1, 8399); //主车任务中断
	*/

	HardInit(); //硬件初始化

	MasterCar_Stop(); //停止小车

	MP_SPK = 1; //核心板蜂鸣器
	delay_ms(200);
	MP_SPK = 0;
	delay_ms(200);
	MP_SPK = 1;
	delay_ms(200);
	MP_SPK = 0;
	Send_Debug_string("First\n"); // 复位成功标识

	while (1)
	{
		keyScan();					   //按键检测
		Running_indicator_light();	   // 运行指示灯
		//MasterCar_TaskRunThread();	   //主车执行任务线程
		MasterCar_TaskReceiveThread(); //主车任务接收线程
		Battery_power_detection();	   // 电池电量检测
	}
}

#if 0
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		/*if (Get_Flag_Track()) //查询是否循迹中防止跑飞
		{
			if (MasterCar_SmartRun_CheckH8(Get_Host_UpTrack(TRACK_H8)) == 0 || MasterCar_SmartRun_CheckQ7(Get_Host_UpTrack(TRACK_Q7)) == 0)
			{
				MasterCar_Stop();
			}
		}*/
		//Zigbee_Wifi_receive();		// Zigbee接收数据处理
		//Interrupt_flag_bit_query(); // 中断标志位查询 TIM3中断
		//MasterCar_TaskReceiveThread(); //主车任务接收线程  测试用
		Battery_power_detection(); // 电池电量检测
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
#endif
