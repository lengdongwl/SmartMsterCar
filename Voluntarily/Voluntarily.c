#include "Voluntarily.h"
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
#include "new_syn.h"
#include "rng.h"
#include "Card.h"


#include "OperationFlag.h"
//#include "CRC.h"

static uint8_t Go_Speed = 36;	 // 全局前进速度值 Go_Speed
static uint8_t wheel_Speed = 90; // 全局转弯速度值 wheel_Speed
static uint16_t Go_Temp = 420;	 // 前进码盘值 Go_Temp

uint8_t Overtime_Flag = 0; // 超时标志位
uint8_t timing_time = 2;   // 定时器超时时间设置
uint8_t Exit_position;	   // 退出运行的位置
uint8_t Operation_Flag;	   // 超时回到的位置
//RFID变量
uint8_t RFID_Flag = 0;										   // RFID检测标志位
uint8_t Terrain_Flag = 0;									   // 地形检测标志位
uint8_t make = 0;											   // 全自动驾驶标志位
uint8_t RFID_addr = 0;										   // RFID有效数据块地址
uint8_t Stereo_Base[6] = {0xff, 0x11, 0x00, 0x00, 0x00, 0x00}; //立体显示距离控制码

uint8_t Fifo[8];	  //调试信息缓存
uint8_t Detected = 0; //地形是否检测到

uint8_t Car_location;		   //卡的位置
uint8_t Car_location_ASCll[2]; //卡的位置ASCll

uint16_t Distance_Data = 0; //距离

uint8_t From_Car_Position[3] = {0}; //副车读卡位置数据缓存
//信息代码
//uint16_t MOV1;				  //信息代码1(  )
//uint16_t MOV2;				  //信息代码2(  )
//uint16_t MOV3;				  //信息代码3(  )
//uint16_t MOV4;				  //信息代码4(  )
//uint16_t MOV5;				  //信息代码5(  )
//uint16_t MOV6;				  //信息代码6(  )
//uint16_t MOV7;				  //信息代码7(  )
//uint16_t MOV8;				  //信息代码8(  )
//uint16_t MOV9;				  //信息代码9(  )
uint8_t Card_Temp = 0;

uint16_t Tempp = 0, Temppp = 0;

uint8_t quantity(char *data_temp)
{
	uint8_t temp = 0;
	for (int i = 0; i < 16; i++)
	{
		if (data_temp[i] == 'A')
		{
			temp += 1;
		}
	}
	return temp;
}

/*************************************************************************
 * 函 数 名 ：  找字符串中最大和最小数字（ASCLL）组成新的数据
 * 参    数 ：  chepai：输入数据
 * 返 回 值 ：  数量
 * 备    注 ：  1ABC5D ->	15（十进制）
*************************************************************************/
uint8_t extract(char *chepai)
{
	char temp;
	uint8_t TP[6];
	uint8_t Big = 0;
	for (int i = 0; i < 6; i++)
	{
		TP[i] = chepai[i];
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[i] < '0' || TP[i] > '9')
		{
			TP[i] = 0xFF;
		}
		else
		{
			TP[i] -= '0';
		}
	}
	for (int i = 0; i < 6; i++)
	{
		for (int ii = 0; ii < 5; ii++)
		{
			if (TP[ii] > TP[ii + 1])
			{
				temp = TP[ii];
				TP[ii] = TP[ii + 1];
				TP[ii + 1] = temp;
			}
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[i] != 0xff)
		{
			Big = TP[i] * 10;
			break;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[5 - i] != 0xff)
		{
			Big += TP[5 - i];
			break;
		}
	}
	return Big;
}

/**********************************************************************
 * 函 数 名 ：  超时扫描
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Interrupt_flag_bit_query(void)
{

	/*数码管显示倒计时*/
	//Digital_Display((timing_time-TIM3_Flag)/10,(timing_time-TIM3_Flag)%10);

	if (TIM3_Flag >= timing_time)
	{
		TIM_Cmd(TIM3, DISABLE); // 失能定时器3
		SYN_Play("超时");
		make_Yes = Operation_Flag; // 超时后回到的地方
		TIM3_Flag = 0;			   // 清除超时检测标志位
		Overtime_Flag = 1;		   // 超时标志位
	}
}

/**********************************************************************
 * 函 数 名 ：  启动超时检测
 * 参    数 ：  Time：超时时间 position:退出运行的位置 Return_run：超时回到的位置
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Timeout_detection(uint16_t Time, uint8_t position, uint8_t Return_run)
{
	for (int i = 0; i < 8; i++) //清除缓存
	{
		Wifi_Rx_Buf[i] = 0;
	}
	Wifi_Rx_flag = 0;			 // 清除Wifi接收标志位+
	Zigbee_Rx_flag = 0;			 // 清除Zigbee接收标志位
	TIM3_Flag = 0;				 // 清除错误
	Overtime_Flag = 0;			 // 清除超时标志位
	timing_time = Time;			 // 超时时间设置
	make_Yes = position;		 // 退出运行的位置设置
	Exit_position = position;	 // 退出运行的位置设置
	Operation_Flag = Return_run; // 超时后回到的位置设置
	TIM_Cmd(TIM3, ENABLE);		 // 使能定时器3（开启超时检测）
	/*上传调试信息*/
	Send_Debug_Info((uint8_t *)"OpenTIM:", 8);
	sprintf((char *)Fifo, "%d\n", Time);
	Send_Debug_Info(Fifo, 3);
	delay_ms(100);
}

/**********************************************************************
 * 函 数 名 ：  Zigbee+WIFI接收副车数据处理
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  1.Zigbee_Rx_flag 2.Wifi_Rx_flag 来自can_user
 * 备    注 ：  Zigbee_Rx_flag(Zigbee接收标志) Wifi_Rx_flag (Wifi接收标志)
*****************************************************************/
void Zigbee_Wifi_receive(void)
{
	/*小创语音*/
	//Voice_control();
	/*WIFI*/
	if (Wifi_Rx_flag == 1)
	{
		if (Receive_Traffic_light_identification() == Receive_succeed) // 交通灯识别
		{
			TIM_Cmd(TIM3, DISABLE);		   // 失能定时器3
			if (make_Yes == Exit_position) // 判断是否启动全自动
			{
				make_Yes = Operation_Flag; // 识别成功回到的位置
			}
		}
		if (transmit_wifi() == 1)
		{
		} // wifi转发至zigbee
		else
		{
			Receive_Operation();								// 平板控制
																//			Receive_Static_marker_1_A();
			if (Receive_Static_marker_1_A() == Receive_succeed) // 静态标志物1（二维码）
			{
				TIM_Cmd(TIM3, DISABLE); // 失能定时器3
				SYN_Play("[s10]二维码识别成功[d]");
				if (make_Yes == Exit_position) // 判断是否启动全自动
				{
					make_Yes = Operation_Flag; // 识别成功回到的位置
				}
				/*上传调试信息*/
				Send_Debug_Info((uint8_t *)"QR Code\n", 8);
				Send_Debug_HEX(Static_marker_1[0]);
				Send_Debug_HEX(Static_marker_1[1]);
				Send_Debug_HEX(Static_marker_1[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				/*数据块*/
				Read_Car_Address = (Static_marker_1[0] + 1) * 4 + (Static_marker_1[1] + 1); // X*4+N(十进制)

				//								Send_Debug_HEX(Static_marker_1[3]);
				//								Send_Debug_HEX(Static_marker_1[4]);
				//								Send_Debug_HEX(Static_marker_1[5]);
				//								Send_Debug_Info((uint8_t*)"\n",2);
				//								for(uint8_t i=0;i<6;i++)
				//								{
				//									if(Static_marker_1[i]==0x00)
				//									{
				//											SSIZE=i;
				//											break;
				//									}
				//								}
				//								sprintf((char*)Fifo,"%X\n",CRC24_BLE((char*)Static_marker_1,SSIZE));
				//								Send_Debug_Info(Fifo,8);
			}
			else if (Receive_flat_computer_TFT() == Receive_succeed) // 车牌识别
			{
				TIM_Cmd(TIM3, DISABLE); // 失能定时器3
				SYN_Play("[s10]车牌识别成功[d]");
				if (make_Yes == Exit_position) // 判断是否启动全自动
				{
					make_Yes = Operation_Flag; // 识别成功回到的位置
				}
				//								TFT_Show_Zigbee('B',(char*)(TFT_license_plate));	// TFT显示车牌
				//								TFT_Show_Zigbee('B',(char*)(TFT_license_plate));	// TFT显示车牌
				TFT_Show_Zigbee('A', (char *)TFT_license_plate);
				TFT_Show_Zigbee('A', (char *)TFT_license_plate);

				/*上传调试信息*/
				Send_Debug_Info((uint8_t *)"P:", 2);
				Send_Debug_Info(TFT_license_plate, 8);
				Send_Debug_Info((uint8_t *)"\n", 2);
				//
				//								for (uint8_t i = 0; i < 6; i++)
				//								{
				//									if (TFT_license_plate[i] >= '0' && TFT_license_plate[i] <= '9')
				//									{
				//										Oupt += TFT_license_plate[i] - '0';
				//									}
				//								}
			}
			else if (Receive_flat_computer_graph() == Receive_succeed) // 图形识别
			{
				TIM_Cmd(TIM3, DISABLE); // 失能定时器3
				SYN_Play("[s10]图形识别成功[d]");
				/*（Graph[0]：矩形 ）（Graph[1]：圆形 ）（Graph[2]：三角）
												（Graph[3]: 菱形）（Graph[4]：五角星）*/
				TFT_Test_Zigbee('B', 0x40, 0xA0 + Graph[0], 0xB0 + Graph[4], 0xC0 + Graph[1]); //TFTA显示
				TFT_Test_Zigbee('B', 0x40, 0xA0 + Graph[0], 0xB0 + Graph[4], 0xC0 + Graph[1]); //TFTA显示

				if (make_Yes == Exit_position) // 判断是否启动全自动
				{
					make_Yes = Operation_Flag;
				}
			}
			else if (Receive_flat_computer_first_start() == Receive_succeed) // 启动控制（平板启动）
			{
				make_Yes = 0x01;
			}
		}
		Wifi_Rx_flag = 0;
	}

	/*Zigbee*/
	if (Zigbee_Rx_flag == 1) // zigbee返回信息
	{
		if (transmit() == 1)
		{
		} // zigbee转发至wifi
		else
		{
			Receive_barrier_gate(); // 接收开启道闸指令

			if (Receive_QR_Code_B() == Receive_succeed) // 接收副车二维码数据B
			{
				TIM_Cmd(TIM3, DISABLE); // 失能定时器3
				SYN_Play("[s10]收到小伙伴二维码[d]");
				Send_Debug_Info((uint8_t *)"QR Code\n", 8);
				Send_Debug_HEX(Vice_car_QR[0]);
				Send_Debug_HEX(Vice_car_QR[1]);
				Send_Debug_HEX(Vice_car_QR[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				Send_Debug_HEX(Vice_car_QR[3]);
				Send_Debug_HEX(Vice_car_QR[4]);
				Send_Debug_HEX(Vice_car_QR[5]);
				Send_Debug_Info((uint8_t *)"\n", 2);
			}
			else if (Receive_start_instruction() == Receive_succeed) // 接收副车（启动）指令
			{
				TIM_Cmd(TIM3, DISABLE); // 失能定时器3
				From_Car_Position[0] = Zigb_Rx_Buf[3];
				From_Car_Position[1] = Zigb_Rx_Buf[4];
				From_Car_Position[2] = Zigb_Rx_Buf[5];
				Send_Debug_Info((uint8_t *)"Car\n", 8);
				Send_Debug_HEX(From_Car_Position[0]);
				Send_Debug_HEX(From_Car_Position[1]);
				Send_Debug_HEX(From_Car_Position[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				SYN_Play("[s10]收到小伙伴数据[d]");
				if (make_Yes == Exit_position) // 超时回到的地方
				{
					make_Yes = Operation_Flag;
				}
			}

			//Send_Debug_num(OFlag_DZ_status(Zigb_Rx_Buf));
			//Send_Debug_string(Zigb_Rx_Buf);
			//Send_Debug(OFlag_CK_status(Zigb_Rx_Buf,1,3));
			//Send_Debug_num(OFlag_check(Zigb_Rx_Buf));
		}
		Zigbee_Rx_flag = 0; // 清除Zigbee接收标志位
	}
}

/**********************************************************************
 * 函 数 名 ：  主车线程运行函数 
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  make_Yes //全自动标志位 在function.h重定义
 * 备    注 ：  无 
*****************************************************************/
void Car_Thread(void)
{

	switch (make_Yes)
	{
	case 0x01:
		Car_Go(Go_Speed, Go_Temp);
		break;
	case 0x02:
		Car_Back(Go_Speed, Go_Temp);
		break;
	case 0x03:
		Car_Back(Go_Speed, Go_Temp);
		break;
	case 0x04:
		Car_Back(Go_Speed, Go_Temp);
		break;
	}
}
