#include "function.h"
#include "drive.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "infrared.h"
#include "cba.h"
#include "ultrasonic.h"
#include "canp_hostcom.h"
#include "hard_can.h"
#include "bh1750.h"
#include "power_check.h"
#include "can_user.h"
#include "data_base.h"
#include "roadway_check.h"
#include "data_base.h"
#include "swopt_drv.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "delay.h"
#include "can_user.h"
#include "Timer.h"
#include "Rc522.h"
#include "string.h"
#include "Zigbee.h"
#include "NEW_Task_board.h"

#include "Voluntarily.h"
#include "rng.h"
#include "Card.h"
/***************************变---量---区***********************************/
//开车标志位
static uint8_t Go_Speed = 36; // 全局前进速度值
//static uint8_t wheel_Speed = 90;            // 全局转弯速度值
//static uint16_t Go_Temp = 420;              // 前进码盘值
//TIM3标志位
uint16_t TIM3_Flag = 0; //定时器3标志位
//RFID检测和地形检测
//uint8_t Testing_number=1;//RFID与地形检测次数
//uint8_t tracking_card; //循迹灯数据（白卡）
//RFID变量
//其他变量
uint8_t Fifo_B[8];			// 调试信息缓存B
uint8_t ETC_Flag;			// ETC检测标志位
uint8_t LED_OUT = 0;		// LED闪烁位
uint32_t Power_check_times; // 电量检测周期
uint32_t LED_twinkle_times; // LED闪烁周期
/**************************************************************************/

/**********************************************************************
 * 函 数 名 ：  调试语音控制
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  测试开启 
*****************************************************************/
void SYN_Play(char *Pst)
{
#if Voice == 0
	SYN7318_Play(Pst);
#endif
#if Voice == 1
	delay_ms(100);
	MP_SPK = 1; //蜂鸣器开
	delay_ms(100);
	MP_SPK = 0; //蜂鸣器关
	delay_ms(100);
	MP_SPK = 1; //蜂鸣器开
	delay_ms(100);
	MP_SPK = 0; //蜂鸣器关
#endif
}

/**********************************************************************
 * 函 数 名 ：  调光
 * 参    数 ：  目标档位
 * 返 回 值 ：  初始档位
 * 全局变量 ：  无
 * 备    注 ：  无 
*****************************************************************/
uint8_t Dimming(uint8_t target)
{
	uint8_t MOV_Fifo;
	for (int CT = 0; CT < 4; CT++) // 如果没有检测到就重新检测3次
	{
		delay_ms(100);
		MOV_Fifo = Light_djustment(target); //光照探测
		delay_ms(50);
		if (MOV_Fifo > 0)
		{
			delay_ms(50);
			sprintf((char *)Fifo_B, "L:%dT:%d\n", MOV_Fifo, target);
			Send_Debug_Info(Fifo_B, 8);
			delay_ms(100);
			break;
		}
	}
	return MOV_Fifo;
}

/**********************************************************************
 * 函 数 名 ：  测距
 * 参    数 ：  无
 * 返 回 值 ：  距离
 * 全局变量 ：  无
 * 备    注 ：  无 
*****************************************************************/
uint16_t Stadiometry(void)
{
	uint16_t DIS_Fifo;
	for (int i2 = 0; i2 < 4; i2++)
	{
		DIS_Fifo = Full_Measure(); //准确，慢
								   //				DIS_Fifo=measure_display();//小误差，快
		if (DIS_Fifo > 0)
		{
			SYN_Play("[s10]测距成功[d]");
			break;
		}
		else
		{
			SYN_Play("[s10]测距失败[d]");
		}
	}
	/*sprintf((char *)Fifo_B, "%dmm\n", DIS_Fifo);
	Send_Debug_Info(Fifo_B, 8);*/
	return DIS_Fifo;
	//			LED_Dis_Zigbee(DIS_Fifo);														//发送LED标志物显示距离
	//			LED_Dis_Zigbee(DIS_Fifo);														//发送LED标志物显示距离
}

/**********************************************************************
 * 函 数 名 ：  ETC检测
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无 
*****************************************************************/
void ETC_detection(void)
{
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	for (uint8_t i = 0; i < 6; i++)
	{
		if (Zigbee_Rx_flag == 1)
		{
			if (Receive_ETC_detection() == Receive_succeed) //ETC接收检测
			{
				SYN_Play("ETC检测成功");
				Zigbee_Rx_flag = 0;
				break;
			}
		}
		if (measure_display() > 200)
		{
			break;
		}
		Car_Back(Go_Speed, 200); //主车后退
		delay_ms(500);
		Car_Go(Go_Speed, 200); // 主车前进
		delay_ms(500);
		if (i == 5)
		{
			SYN_Play("ETC检测失败");
		}
	}
}

/**********************************************************************
 * 函 数 名 ：  车库上升层
 * 参    数 ：  层
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  进车库停稳了才调用这个函数 ！！！！！！！！！！！！
*****************************************************************/
void Garage(uint8_t Tier)
{
	switch (Tier)
	{
	case 1:
	{
		break;
	}
	case 2:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 3:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To3, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To3, 8);
		for (int i = 0; i < 15; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 4:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To4, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To4, 8);
		for (int i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	default:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	}
}

/**********************************************************************
 * 函 数 名 ：  清除wifi/zigbee接收完成标志位
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Eliminate_Flag(void)
{
	Wifi_Rx_flag = 0;	//wifi缓存数据标志位清除
	Zigbee_Rx_flag = 0; //zigbee缓存数据标志位清除
}

/**********************************************************************
 * 函 数 名 ：  微调位置
 * 参    数 ：  RL：左右
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Full_RL_little(uint8_t RL, uint8_t JY)
{
	Roadway_nav_syn();	  //角度同步
	Roadway_mp_syn();	  //码盘同步
	Roadway_Flag_clean(); //清除标志位
	wheel_Nav_Flag = 1;
	if (RL == 1)
		temp_Nav = JY; //经验值
	else
		temp_Nav = JY; //
	Car_Spend = 80;
	if (RL == 1)
	{
		Send_UpMotor(-Car_Spend, Car_Spend); //左转
	}
	else
	{
		Send_UpMotor(Car_Spend, -Car_Spend); //右转
	}
	while (Stop_Flag != 2)
		;
}

/**********************************************************************
 * 函 数 名 ：  转弯后微调
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Full_adjust(void)
{
	uint8_t zw;
	zw = Get_Host_UpTrack(TRACK_H8);
	if (zw == 0XFB || zw == 0XF9 || zw == 0XFD || zw == 0XFC || zw == 0XFE)
	{
		Full_RL_little(2, 90);
	}
	else if (zw == 0X9F || zw == 0XDF || zw == 0X3F || zw == 0XBF || zw == 0X7F)
	{
		Full_RL_little(1, 90);
	}
	delay_ms(100);
}

/****************************************************************
 * 函 数 名 ：全自动码盘循迹
 * 参    数 ：sp：速度  mp：码盘
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void MP_Tracking(uint8_t sp, u16 mp)
{
	u16 MP = 0;
	Roadway_mp_syn(); //码盘同步
	Roadway_Flag_clean();
	Track_Flag = 1;
	temp_MP = mp;
	Car_Spend = sp;
	while ((MP < mp) && (Stop_Flag != 0x01))
	{
		MP = Roadway_mp_Get();
	}
	Full_STOP(); // 停止
	delay_ms(100);
}

/****************************************************************
 * 函 数 名 ：  停止
 * 参    数 ：	无
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Full_STOP(void)
{
	Roadway_Flag_clean(); //清除所有标志位
	Send_UpMotor(0, 0);	  // 停止
}
/**********************************************************************
 * 函 数 名 ：  2021年新板全自动循迹灯加码盘左转
 * 参    数 ：  sp 速度
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Left_hand_bend(u8 sp)
{
	uint8_t T8;
	int16_t Mpa;
	Roadway_mp_syn(); // 码盘同步
	Roadway_Flag_clean();
	Control(-sp, sp);
	Mpa = CanHost_Mp;
	delay_ms(200);
	while (1)
	{
		T8 = Get_Host_UpTrack(TRACK_H8);
		T8 &= 0x18;
		if (T8 == 0x00 || T8 == 0X10)
		{
			Send_UpMotor(0, 0);
			break;
		}
		if (Mpa <= 0 && CanHost_Mp > 0)
		{
			if ((Mpa - (-32768) + 32768 - CanHost_Mp) > 930)
			{
				/* 左转-32768到32768临界点判断 */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (Mpa - CanHost_Mp > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * 函 数 名 ：  2021年新板全自动循迹灯加码盘右转
 * 参    数 ：  sp 速度
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Right_hand_bend(u8 sp)
{
	uint8_t T8;
	int16_t Mpa;
	Roadway_mp_syn(); // 码盘同步
	Roadway_Flag_clean();
	Control(sp, -sp);
	Mpa = CanHost_Mp;
	delay_ms(300);
	while (1)
	{
		T8 = Get_Host_UpTrack(TRACK_H8);
		T8 &= 0x18;
		if (T8 == 0x00 || T8 == 0X08)
		{
			Send_UpMotor(0, 0);
			break;
		}
		if (Mpa >= 0 && CanHost_Mp < 0)
		{
			if ((32768 - Mpa) + (CanHost_Mp - (-32768)) > 930)
			{
				/* 右转32768到-32768 临界点判断 */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (CanHost_Mp - Mpa > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * 函 数 名 ：  码盘右转
 * 参    数 ：  sp 速度
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Right_MP(u8 sp)
{
	int16_t Mpa;
	Roadway_mp_syn(); // 码盘同步
	Roadway_Flag_clean();
	Control(sp, -sp);
	Mpa = CanHost_Mp;
	while (1)
	{
		if (Mpa >= 0 && CanHost_Mp < 0)
		{
			if ((32768 - Mpa) + (CanHost_Mp - (-32768)) > 930)
			{
				/* 右转32768到-32768 临界点判断 */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (CanHost_Mp - Mpa > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * 函 数 名 ：  码盘左转
 * 参    数 ：  sp 速度
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Left_MP(u8 sp)
{
	int16_t Mpa;
	Roadway_mp_syn(); // 码盘同步
	Roadway_Flag_clean();
	Control(-sp, sp);
	Mpa = CanHost_Mp;
	while (1)
	{
		if (Mpa <= 0 && CanHost_Mp > 0)
		{
			if ((Mpa - (-32768) + 32768 - CanHost_Mp) > 930)
			{
				/* 左转-32768到32768临界点判断 */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (Mpa - CanHost_Mp > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}

void Full_Right(u8 sp)
{
	Right_hand_bend(sp);
	//	Right_MP(sp);
}
void Full_Left(u8 sp)
{
	Left_hand_bend(sp);
	//	Left_MP(sp);
}

/**********************************************************************
 * 函 数 名 ：  硬件初始化函数
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); // 中断分组
	delay_init(168);								// 延时初始化
	
	Task_board_Init();								// 任务板初始化
	Infrared_Init();								// 红外初始化
	Cba_Init();										// 核心板初始化
	Ultrasonic_Init();								// 超声波初始化
	Hard_Can_Init();								// CAN总线初始化
	BH1750_Configure();								// BH1750初始化配置
	Electricity_Init();								// 电量检测初始化
	UartA72_Init();									// A72硬件串口通讯初始化
	Can_check_Init(7, 83);							// CAN总线定时器初始化
	roadway_check_TimInit(999, 167);				// 路况检测
	Timer_Init(999, 167);							// 串行数据通讯时间帧
	Readcard_daivce_Init();							// RFID初始化
	TIM3_Int_Init(10000 - 1, 8400 - 1);				// 定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1s
	TIM4_Int_Init(100 - 1, 8400 - 1);				// 定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数100次为10ms
	//while(RNG_Init());	 																// 初始化随机数发生器
	//SYN_Init();										// 语音识别初始化
	SYN7318_Init();
	Tim5_Init(1,8399);		//WIFI接收识别数据初始化
	
	LED_twinkle_times = gt_get() + 50;
	Power_check_times = gt_get() + 200;
}

/**********************************************************************
 * 函 数 名 ：  自动调光
 * 参    数 ：  Sun_gear：目标档位值
 * 返 回 值 ：  初始档位值
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Light_djustment(uint8_t Sun_gear)
{
	uint8_t i = 4;
	uint8_t gears_Value = 0;
	u16 Light_Value[5];
	Get_Bh_Value();
	delay_ms(500);
	for (i = 0; i < 4; i++)
	{
		Digital_tube_Infrared_Send(Light_plus1, 4); //光源档位加1
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Light_Value[i] = Get_Bh_Value(); //测试
#if Test_Daat == 1
		Send_Debug(Light_Value[i]); //测试
#endif
		delay_ms(500);
	}
	if (Light_Value[0] > Light_Value[1]) //判断到当前档位为第三档
	{
		gears_Value = 3;
	}
	else if (Light_Value[1] > Light_Value[2]) //判断到当前档位为第二档
	{
		gears_Value = 2;
	}
	else if (Light_Value[2] > Light_Value[3]) //判断到当前档位为第一档
	{
		gears_Value = 1;
	}
	else //判断到当前档位为第四档
	{
		gears_Value = 4;
	}
	delay_ms(500);
	if (Sun_gear != 0)
	{
		switch (Sun_gear) //输入控制档位
		{
		case 1: //调1档
		{
			switch (gears_Value) //状态查询
			{
			case 2: //当前档位为2
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //光源档位加1
				break;
			}
			case 3: //当前档位为3
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //光源档位加1
				break;
			}
			case 4: //当前档位为4
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //光源档位加1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 2: //调2档
		{
			switch (gears_Value) //状态查询
			{
			case 3: //当前档位为3
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //光源档位加1
				break;
			}
			case 4: //当前档位为4
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //光源档位加1
				break;
			}
			case 1: //当前档位为1
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //光源档位加1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 3: //调3档
		{
			switch (gears_Value) //状态查询
			{
			case 4: //当前档位为4
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //光源档位加1
				break;
			}
			case 1: //当前档位为1
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //光源档位加1
				break;
			}
			case 2: //当前档位为2
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //光源档位加1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 4: //调4档
		{
			switch (gears_Value) //状态查询
			{
			case 1: //当前档位为1
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //光源档位加1
				break;
			}
			case 2: //当前档位为2
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //光源档位加1
				break;
			}
			case 3: //当前档位为3
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //光源档位加1
				break;
			}
			default:
				break;
			}
			break;
		}
		}
	}
	return gears_Value;
}

/****************************************************************
函数功能：测距函数//频率31.6
参	  数：Full_Measure();				Measure()；
返回  值：无
*****************************************************************/
uint16_t Full_Measure(void)
{
	int i, j, temp, disss;
	float brr[8] = {0};
	for (i = 0; i < 8; i++)
	{
		LED2 = !LED2;
		measure_display();
		delay_ms(100);
#if Test_Daat == 1
		Send_Debug(dis);
#endif
		brr[i] = dis;
	}
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 7 - i; j++)
		{
			if (brr[j] > brr[j + 1])
			{
				temp = brr[j];
				brr[j] = brr[j + 1];
				brr[j + 1] = temp;
			}
		}
	}
	//delay_ms(500);
	disss = (u16)(((brr[3] + brr[4]) / 2));
	delay_ms(200);
	return disss;
	//return measure_display();
}
/**********************************************************************
 * 函 数 名 ：  超声波测距（多次）
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint16_t measure_display(void)
{
	int i, j, temp;
	float arr[8] = {0};

	for (i = 0; i < 8; i++)
	{
		Ultrasonic_Ranging(); //超声波测距

		delay_ms(100);
		arr[i] = dis;
	}

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 7 - i; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	//dis=(u16)(((arr[5]+arr[6])/2)-22);			//红板1:	-22
	dis = (u16)(((arr[5] + arr[6]) / 2) - 12); //红板1:	-12

	//				TFT_Dis_Zigbee('A',dis);	  // TFT显示距离信息
	//delay_ms(200);
	return dis;
}

/**********************************************************************
 * 函 数 名 ：  通用定时器3中断初始化
 * 参    数 ：  arr：自动重装值 psc：时钟预分频数
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us. Ft=定时器工作频率,单位:Mhz
*****************************************************************/
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能TIM3时钟

	TIM_TimeBaseInitStructure.TIM_Period = arr;						//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //初始化TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //允许定时器3更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;	  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************
 * 函 数 名 ：  定时器3中断服务函数
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //溢出中断
	{
		TIM3_Flag += 1;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位
}
/**********************************************************************
 * 函 数 名 ：  任务板测试
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Digital_tube_TEXT(void)
{
	/*蜂鸣器测试*/
	Task_board_buzzer(SET); // 复位成功指示
	delay_ms(500);
	delay_ms(500);
	Task_board_buzzer(RESET);

	/*语言模块发声与收音测试*/
	//SYN_Extern(); // 语音识别

	/*转向灯功能测试*/
	Task_board_turn_light(R_LED, SET); // 右闪 开
	delay_ms(500);
	Task_board_turn_light(R_LED, RESET); // 右闪 关
	Task_board_turn_light(L_LED, SET);	 // 左闪 开
	delay_ms(500);
	Task_board_turn_light(L_LED, RESET); // 左闪 关
	Task_board_turn_light(R_LED, SET);	 // 右闪 开
	delay_ms(500);
	Task_board_turn_light(R_LED, RESET); // 右闪 关
	Task_board_turn_light(L_LED, SET);	 // 左闪 开
	delay_ms(500);
	Task_board_turn_light(L_LED, RESET); // 左闪 关

	/*数码管及74HC595测试*/
	for (uint8_t i = 0; i < 10; i++)
	{
		Digital_Display(i, i); // 数码管显示
		delay_ms(200);
	}
	/*超声波模块测试*/
	Stadiometry(); // 测距

	/*红外测试*/
	Rotate_Dis_Inf(200); // 立体显示标志物显示距离信息（单位：mm）
	Rotate_Dis_Inf(200); // 立体显示标志物显示距离信息（单位：mm）

	/*BH1750光照传感器测试*/
	Send_Debug(Light_djustment(4)); // 光照探测
	SYN_Play("完成");
}

/**********************************************************************
 * 函 数 名 ：  语音播放（数字+文字）内容 
 * 参    数 ：  number：数字播放  unt：文字播部分
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  数字部分大小：0~65535  文字：0~3个字
*****************************************************************/
void voice(uint16_t number, char *unt)
{
	uint8_t Size_Flag;
	char Send_voice[12];
	//数字部分计算合成
	Send_voice[0] = (number % 100000 / 10000) + 0x30;
	Send_voice[1] = (number % 10000 / 1000) + 0x30;
	Send_voice[2] = (number % 1000 / 100) + 0x30;
	Send_voice[3] = (number % 100 / 10) + 0x30;
	Send_voice[4] = (number % 10) + 0x30;
	//文字部分计算合成
	Send_voice[5] = unt[0];
	Send_voice[6] = unt[1];
	Send_voice[7] = unt[2];
	Send_voice[8] = unt[3];
	Send_voice[9] = unt[4];
	Send_voice[10] = unt[5];
	Send_voice[11] = unt[6];
	for (int i = 0; i < 5; i++) //判断为0（0033->（2））
	{
		if (Send_voice[i] != 0x30)
		{
			Size_Flag = i;
			if (Send_voice[i] > '9')
			{
				Size_Flag = 0;
			}
			break;
		}
	}
	if (Size_Flag != 0) //往前缩进 （0011 -> 11）
	{
		for (int i2 = 0; i2 < Size_Flag; i2++)
		{
			for (int i3 = 0; i3 < 11; i3++)
			{
				Send_voice[i3] = Send_voice[i3 + 1];
			}
		}
	}
	SYN_Play(Send_voice); //语音合成播放函数
}

/**********************************************************************
 * 函 数 名 ：  运行指示灯 
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Running_indicator_light(void)
{
	if (gt_get_sub(LED_twinkle_times) == 0) // 运行指示灯
	{
		LED_twinkle_times = gt_get() + 50; // LED4运行状态指示灯
		if (LED_OUT == 0)
		{
			LED4 = RESET;
			LED1 = SET;
			LED_OUT = 1;
		}
		else if (LED_OUT == 1)
		{
			LED1 = RESET;
			LED2 = SET;
			LED_OUT = 2;
		}
		else if (LED_OUT == 2)
		{
			LED2 = RESET;
			LED3 = SET;
			LED_OUT = 3;
		}
		else if (LED_OUT == 3)
		{
			LED3 = RESET;
			LED4 = SET;
			LED_OUT = 0;
		}
	}
}

/**********************************************************************
 * 函 数 名 ：  电池电量检测 
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Battery_power_detection(void)
{
	if (gt_get_sub(Power_check_times) == 0) // 电池电量检测
	{
		Power_check_times = gt_get() + 200;
		Power_Check();
	}
}

uint8_t Garage_Tier = 0; // 车库层标志位
/**********************************************************************
 * 函 数 名 ：  让车库达到第一层
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  自己不动
*****************************************************************/
void Into_the_garage(void)
{
	uint8_t i = 0;
	Send_ZigbeeData_To_Fifo(GarageA_Get_Floor, 8); // 立体车库A 请求返回车库位于第几层
	Send_ZigbeeData_To_Fifo(GarageA_Get_Floor, 8); // 立体车库A 请求返回车库位于第几层
	delay_ms(100);
	while (1)
	{							 //55 0D 03 01 01 00 05 BB
								 //55 0D 03 01 02 00 05 BB
		if (Zigbee_Rx_flag == 1) // zigbee返回信息
		{
			if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X0D))
			{
				if ((Zigb_Rx_Buf[2] == 0X03) && (Zigb_Rx_Buf[7] == 0xBB) && (Zigb_Rx_Buf[3] == 0X01))
				{
					Garage_Tier = Zigb_Rx_Buf[4];
					break;
				}
			}
			Zigbee_Rx_flag = 0;
		}
		delay_ms(100);
		i += 1;
		if (i >= 30) //3S内没有接收到
		{
			Garage_Tier = 0;
			break;
		}
	}
	switch (Garage_Tier)
	{
	case 0:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (int i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 1:
	{
		Garage_Tier = 0;
		break;
	}
	case 2:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 3:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (uint8_t i = 0; i < 15; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 4:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (uint8_t i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	}
}

/**********************************************************************
 * 函 数 名 ：  烽火台改密码
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  若修改不了就按下烽火台里面的S4按键更改到初始密码后在进行修改新的密码
*****************************************************************/
void Beacon_change_password(uint8_t PassWord[6])
{
	uint8_t password[] = {0x55, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};
	password[2] = 0x10;
	password[3] = PassWord[0];
	password[4] = PassWord[1];
	password[5] = PassWord[2];
	password[6] = (password[2] + password[3] + password[4] + password[5]) % 0xFF;
	Send_ZigbeeData_To_Fifo(password, 8);
	delay_ms(100);
	password[2] = 0x11;
	password[3] = PassWord[3];
	password[4] = PassWord[4];
	password[5] = PassWord[5];
	password[6] = (password[2] + password[3] + password[4] + password[5]) % 0xFF;
	Send_ZigbeeData_To_Fifo(password, 8);
}
/**********************************************************************
 * 函 数 名 ：  查看卡123缓冲区数据
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Check_the_cache(void)
{
	uint8_t RFL[8], RFH[8];
	Send_Debug_Info((uint8_t *)"Card 1\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_One[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_One[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);

	Send_Debug_Info((uint8_t *)"Card 2\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_Two[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_Two[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);

	Send_Debug_Info((uint8_t *)"Card 3\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_Three[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_Three[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);
}

/**********************************************************************
 * 函 数 名 ：  小创语言识控制
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Voice_control(void)
{
#if Voice == 1
	if (USART6_Flag)
	{
		if (USART6_Back[0] == 0x55 && USART6_Back[1] == 0x02)
		{
			switch (USART6_Back[2])
			{
			case 0x06:
			{
				Car_Go(Go_Speed, Go_Temp); // 主车前进
				break;
			}
			case 0x07:
			{
				Car_Back(Go_Speed, Go_Temp); // 主车后退
				break;
			}
			case 0x08:
			{
				Left_MP(wheel_Speed); // 主车左转
				break;
			}
			case 0x09:
			{
				Right_MP(wheel_Speed); // 主车右转
				break;
			}
			case 0x0A:
			{
				Car_Track(Go_Speed); // 主车循迹
				break;
			}
			}
		}
		USART6_Flag = 0;
	}
#endif
}
