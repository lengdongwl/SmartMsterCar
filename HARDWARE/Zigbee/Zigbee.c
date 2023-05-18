#include "Zigbee.h"
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
#include "new_syn.h"
#include "MasterCarCotrol.h"
uint8_t TFT_license_plate[6]; //接收TFT车牌数据
uint8_t distance_QRcode[3];	  //接收距离/二维码数字
uint8_t Graph[5];			  //接收图形形状数据
uint8_t RFID_secret_key[6];	  //接收RFID密钥

uint8_t Receive_RFID;  //RFID密钥接收标志位
uint8_t Receive_graph; //图形形状接收标志位
uint8_t Receive_TFT;   //TFT车牌接收标志位

uint8_t street_lamp_data;		   //路灯接收的数据
uint8_t plate_number[6];		   //Zigbee接收的车牌数据
uint8_t plate_number_quantity = 0; //Zigbee接收的车牌数据次数

/****************************************************************
 * 函 数 名 ：  接收副车（启动）指令
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_start_instruction(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0X01) && (Zigb_Rx_Buf[3] == 0x00))
		{
			if ((Zigb_Rx_Buf[4] == 0X00) && (Zigb_Rx_Buf[5] == 0x00))
			{
				if (Zigb_Rx_Buf[7] == 0xBB)
				{
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}
/****************************************************************
 * 函 数 名 ：  接收副车二维码A
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Vice_car_QR[6]; //接收副车二维码数据
uint8_t Receive_QR_Code_A(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0X0C) && (Zigb_Rx_Buf[7] == 0xBB))
		{
			Vice_car_QR[0] = Zigb_Rx_Buf[3];
			Vice_car_QR[1] = Zigb_Rx_Buf[4];
			Vice_car_QR[2] = Zigb_Rx_Buf[5];
			return Receive_succeed;
		}
	}
	return Receive_fail;
}
/****************************************************************
 * 函 数 名 ：  接收副车二维码B
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_QR_Code_B(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0X0D) && (Zigb_Rx_Buf[7] == 0xBB))
		{
			Vice_car_QR[3] = Zigb_Rx_Buf[3];
			Vice_car_QR[4] = Zigb_Rx_Buf[4];
			Vice_car_QR[5] = Zigb_Rx_Buf[5];
			return Receive_succeed;
		}
	}
	return Receive_fail;
}
/****************************************************************
 * 函 数 名 ：  接收ETC回传
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_ETC_detection(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X0C))
	{
		if ((Zigb_Rx_Buf[2] == 0X01) && (Zigb_Rx_Buf[3] == 0x01))
		{
			if ((Zigb_Rx_Buf[4] == 0X06) && (Zigb_Rx_Buf[5] == 0x00))
			{
				if ((Zigb_Rx_Buf[6] == 0X08) && (Zigb_Rx_Buf[7] == 0xBB))
				{
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  zigbee转发至wifi
 * 参    数 ：  无
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t transmit(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0XCC) && (Zigb_Rx_Buf[7] == 0xBB))
		{
			transpond[3] = Zigb_Rx_Buf[3];
			transpond[4] = Zigb_Rx_Buf[4];
			transpond[5] = Zigb_Rx_Buf[5];
			transpond[6] = Zigb_Rx_Buf[6];
			Send_WifiData_To_Fifo(transpond, 8);
			return 1;
		}
	}
	return 0;
}

/****************************************************************
 * 函 数 名 ：  wifi转发至zigbee
 * 参    数 ：  无
 * 返 回 值 ：	发送成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t transmit_wifi(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] != 0X01))
	{
		if ((Wifi_Rx_Buf[1] != 0XAA))
		{
			if (Wifi_Rx_Buf[7] == 0xBB)
			{
				transpond_wifi[1] = Wifi_Rx_Buf[1];
				transpond_wifi[2] = Wifi_Rx_Buf[2];
				transpond_wifi[3] = Wifi_Rx_Buf[3];
				transpond_wifi[4] = Wifi_Rx_Buf[4];
				transpond_wifi[5] = Wifi_Rx_Buf[5];
				transpond_wifi[6] = Wifi_Rx_Buf[6];
				Send_ZigbeeData_To_Fifo(transpond_wifi, 8);
				return 1;
			}
		}
	}
	return 0;
}

/****************************************************************
 * 函 数 名 ：  上传调试信息（WIFI-平板）ASCLL
 * 参    数 ：  无
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Debug_wifi_ASCLL(char instruct_1, char instruct_2, char instruct_3)
{
	Send_flat_computer[2] = 0xCD;
	Send_flat_computer[3] = instruct_1;
	Send_flat_computer[4] = instruct_2;
	Send_flat_computer[5] = instruct_3;
	Send_flat_computer[6] = (instruct_1 + instruct_2 + instruct_3) % 256;
	delay_ms(100);
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
}
/****************************************************************
 * 函 数 名 ：  上传调试信息（WIFI-平板）十进制
 * 参    数 ：  无
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Debug_wifi(uint8_t instruct_1, uint8_t instruct_2, uint8_t instruct_3)
{
	Send_flat_computer[2] = 0xCE;
	Send_flat_computer[3] = instruct_1;
	Send_flat_computer[4] = instruct_2;
	Send_flat_computer[5] = instruct_3;
	Send_flat_computer[6] = (instruct_1 + instruct_2 + instruct_3) % 256;
	delay_ms(100);
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
}
/****************************************************************
 * 函 数 名 ：  上传调试信息（WIFI-平板）十六进制
 * 参    数 ：  无
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Debug_wifi_HEX(uint8_t instruct_1, uint8_t instruct_2, uint8_t instruct_3)
{
	Send_flat_computer[2] = 0xCF;
	Send_flat_computer[3] = instruct_1;
	Send_flat_computer[4] = instruct_2;
	Send_flat_computer[5] = instruct_3;
	Send_flat_computer[6] = (instruct_1 + instruct_2 + instruct_3) % 256;
	delay_ms(100);
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
}

/****************************************************************
 * 函 数 名 ：  接收距离数据
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  接收到的数据：distance_QRcode[3]
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_distance_QRcode(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0xA4 && (Zigb_Rx_Buf[7] == 0xBB)))
		{
			if (Zigb_Rx_Buf[6] == (Zigb_Rx_Buf[3] + Zigb_Rx_Buf[4] + Zigb_Rx_Buf[5] + 0xA4) % 256)
			{
				distance_QRcode[0] = Zigb_Rx_Buf[3];
				distance_QRcode[1] = Zigb_Rx_Buf[4];
				distance_QRcode[2] = Zigb_Rx_Buf[5];
				return Receive_succeed;
			}
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  接收路灯数据
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  接收到的数据：street_lamp_data
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_street_lamp(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0x02) && (Zigb_Rx_Buf[7] == 0xBB))
		{
			if (Zigb_Rx_Buf[6] == (Zigb_Rx_Buf[3] + Zigb_Rx_Buf[4] + Zigb_Rx_Buf[5] + 0x02) % 256)
			{
				street_lamp_data = Zigb_Rx_Buf[3];
				return Receive_succeed;
			}
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  接收副车发送启动道闸的指令
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  接收到的数据：street_lamp_data
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_barrier_gate(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0x03) && (Zigb_Rx_Buf[7] == 0xBB))
		{
			if (Zigb_Rx_Buf[6] == 0x03)
			{
				Gate_Open_Zigbee(); // 道闸闸门 -> 开启
				return Receive_succeed;
			}
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  接收车牌数据
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  接收到的数据：plate_number[6]
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_plate_number(void)
{
	if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X01))
	{
		if ((Zigb_Rx_Buf[2] == 0x03 && (Zigb_Rx_Buf[7] == 0xBB)))
		{
			if (Zigb_Rx_Buf[6] == (Zigb_Rx_Buf[3] + Zigb_Rx_Buf[4] + Zigb_Rx_Buf[5] + 0x03) % 256)
			{
				plate_number[0] = Zigb_Rx_Buf[3];
				plate_number[1] = Zigb_Rx_Buf[4];
				plate_number[2] = Zigb_Rx_Buf[5];
				plate_number_quantity = 1;
			}
		}
		if ((Zigb_Rx_Buf[2] == 0x04 && (Zigb_Rx_Buf[7] == 0xBB)))
		{
			if (Zigb_Rx_Buf[6] == (Zigb_Rx_Buf[3] + Zigb_Rx_Buf[4] + Zigb_Rx_Buf[5] + 0x04) % 256)
			{
				plate_number[3] = Zigb_Rx_Buf[3];
				plate_number[4] = Zigb_Rx_Buf[4];
				plate_number[5] = Zigb_Rx_Buf[5];
				if (plate_number_quantity == 1)
				{
					plate_number_quantity = 2;
				}
			}
		}
		if (plate_number_quantity == 2)
		{
			plate_number_quantity = 0;
			return Receive_succeed;
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  发送到Zigbee
 * 参    数 ：   coord1,coord2,coord3：坐标 instruct：指令
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Send_Zigbee_Data(uint8_t instruct, uint8_t coord1, uint8_t coord2, uint8_t coord3)
{
	Send_Vice_car_Fifo[2] = instruct;
	Send_Vice_car_Fifo[3] = coord1;
	Send_Vice_car_Fifo[4] = coord2;
	Send_Vice_car_Fifo[5] = coord3;
	Send_Vice_car_Fifo[6] = (coord1 + coord2 + coord3 + instruct) % 256;
	Send_ZigbeeData_To_Fifo(Send_Vice_car_Fifo, 8);
	delay_ms(200);
}

/****************************************************************
 * 函 数 名 ：  发送数据给Wifi
 * 参    数 ：  instruct:主指令
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void uploading_flat_computer(uint8_t instruct)
{
	Send_flat_computer[2] = instruct;
	Send_flat_computer[3] = 0;
	Send_flat_computer[4] = 0;
	Send_flat_computer[5] = 0;
	Send_flat_computer[6] = instruct;
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
	delay_ms(200);
}
/****************************************************************
 * 函 数 名 ：  发送数据到Wifi
 * 参    数 ：  coord1,coord2,coord3：数据 instruct：指令
 * 0xB0	TFT_A	0x00	0x00	0x00
 * 0xB1	TFT_B	0x00	0x00	0x00
 * 0xB2	交通灯A	0x00	0x00	0x00
 * 0xB3	交通灯B	0x00	0x00	0x00
 * 0xB4	静态标志物1	0x00	0x00	0x00
 * 0xB5	静态标志物2	0x00	0x00	0x00
 * 0xB6				
 * 0xB7	数据算法压缩A	01A	10B	11C
 * 0xB8	数据算法压缩B	01A	10B	11C
 * 0xB9	破损车牌1	车牌[0]	车牌[1]	车牌[2]
 * 0xBA	破损车牌2	车牌[3]	车牌[4]	车牌[5]
 * 0xBB	路灯初始档位	初始档位	0x00	0x00
 * 0xBC	主车空闲中	0x00	0x00	0x00
 * 0xCD	上传调试信息1	ASCLL码[0]	ASCLL码[1]	ASCLL码[2]
 * 0XCE	上传调试信息2	十进制[0]	十进制[1]	十进制[2]
 * 0XCF	上传调试信息3	十六进制[0]	十六进制[1]	十六进制[2]
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Send_Wifi(uint8_t instruct, uint8_t coord1, uint8_t coord2, uint8_t coord3)
{
	Send_flat_computer[2] = instruct;
	Send_flat_computer[3] = coord1;
	Send_flat_computer[4] = coord2;
	Send_flat_computer[5] = coord3;
	Send_flat_computer[6] = (Send_flat_computer[3] + Send_flat_computer[4] +
							 Send_flat_computer[5] + Send_flat_computer[2]) %
							0xFF;
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
	delay_ms(200);
}
/****************************************************************
 * 函 数 名 ：  发送路灯档位给Wifi
 * 参    数 ：  路灯档位
 * 返 回 值 ：	无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Send_street_lamp_gear(uint8_t instruct)
{
	Send_flat_computer[2] = 0xBB;
	Send_flat_computer[3] = instruct;
	Send_flat_computer[4] = 0;
	Send_flat_computer[5] = 0;
	Send_flat_computer[6] = (0xBB + instruct) % 256;
	delay_ms(100);
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
	delay_ms(100);
}
/****************************************************************
 * 函 数 名 ：  接收Wifi数据（首次启动）
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_flat_computer_first_start(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0xA0) && (Wifi_Rx_Buf[3] == 0x00))
		{
			if ((Wifi_Rx_Buf[4] == 0X00) && (Wifi_Rx_Buf[5] == 0x00))
			{
				if ((Wifi_Rx_Buf[6] == 0xA0) && (Wifi_Rx_Buf[7] == 0xBB))
				{
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（后续启动）
 * 参    数 ：  无
 * 返 回 值 ：  接收成功 （成功：0x11 失败：0x00）
**********************************************************************/
uint8_t Receive_flat_computer_second_start(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0xA1) && (Wifi_Rx_Buf[3] == 0x00))
		{
			if ((Wifi_Rx_Buf[4] == 0X00) && (Wifi_Rx_Buf[5] == 0x00))
			{
				if ((Wifi_Rx_Buf[6] == 0xA1) && (Wifi_Rx_Buf[7] == 0xBB))
				{
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}

/****************************************************************
 * 函 数 名 ：  接收Wifi数据（TFT识别到的车牌数据）
 * 参    数 ：  无
 * 返 回 值 ：	接收成功 （成功：0x11 失败：0x00）
 * 全局变量 ：  TFT_license_plate[6]
 * 备    注 ：  无
*****************************************************************/
uint8_t Receive_flat_computer_TFT(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0x40) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0x40) % 256)
			{
				TFT_license_plate[0] = Wifi_Rx_Buf[3];
				TFT_license_plate[1] = Wifi_Rx_Buf[4];
				TFT_license_plate[2] = Wifi_Rx_Buf[5];
				Receive_TFT = 1;
			}
		}
	}
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0x41) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0x41) % 256)
			{
				TFT_license_plate[3] = Wifi_Rx_Buf[3];
				TFT_license_plate[4] = Wifi_Rx_Buf[4];
				TFT_license_plate[5] = Wifi_Rx_Buf[5];
				if (Receive_TFT == 1)
				{
					Receive_TFT = 0;
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（图形形状）
 * 参    数 ：  无      
Graph[0]：矩形 Graph[1]：圆形  Graph[2]：三角 
Graph[3]: 菱形 Graph[4]：五角星
 * 返 回 值 ：  接收成功 （成功：0x11 失败：0x00）
**********************************************************************/
uint8_t Receive_flat_computer_graph(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0x45) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0x45) % 256)
			{
				Graph[0] = Wifi_Rx_Buf[3];
				Graph[1] = Wifi_Rx_Buf[4];
				Graph[2] = Wifi_Rx_Buf[5];
				Receive_graph = 1;
			}
		}
	}
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0x46) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0x46) % 256)
			{
				Graph[3] = Wifi_Rx_Buf[3];
				Graph[4] = Wifi_Rx_Buf[4];
				if (Receive_graph == 1)
				{
					Receive_graph = 0;
					return Receive_succeed;
				}
			}
		}
	}
	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（RFID密钥）
 * 参    数 ：  无      RFID_secret_key[6]
 * 返 回 值 ：  接收成功 （成功：0x11 失败：0x00）
**********************************************************************/
uint8_t Receive_flat_computer_RFID(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0xA2) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0xA2) % 256)
			{
				RFID_secret_key[1] = Wifi_Rx_Buf[3];
				RFID_secret_key[2] = Wifi_Rx_Buf[4];
				RFID_secret_key[3] = Wifi_Rx_Buf[5];
				Receive_RFID = 1;
			}
		}
	}
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0xA3) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[6] == (Wifi_Rx_Buf[3] + Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] + 0xA3) % 256)
			{
				RFID_secret_key[3] = Wifi_Rx_Buf[3];
				RFID_secret_key[4] = Wifi_Rx_Buf[4];
				RFID_secret_key[5] = Wifi_Rx_Buf[5];
				if (Receive_RFID == 1)
				{
					Receive_RFID = 0;
					return Receive_succeed;
				}
			}
		}
	}

	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（交通灯识别）
 * 参    数 ：  无
 * 返 回 值 ：  无
**********************************************************************/
uint8_t Receive_Traffic_light_identification(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0X0E))
	{
		if ((Wifi_Rx_Buf[2] == 0x02) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			if (Wifi_Rx_Buf[3] == 0x01)
			{
				Send_ZigbeeData_To_Fifo(TrafficA_Red, 8); // 智能交通灯 -> 红色
				delay_ms(200);
				//Send_Debug_Info((uint8_t*)"Red\n",5);
				Send_Debug_string("Red\n");
				delay_ms(200);
				SYN_Play("红色");
			}
			else if (Wifi_Rx_Buf[3] == 0x02)
			{
				Send_ZigbeeData_To_Fifo(TrafficA_Green, 8); // 智能交通灯 -> 绿色
				delay_ms(200);
				Send_Debug_string("Green\n");
				delay_ms(200);
				SYN_Play("绿色");
			}
			else if (Wifi_Rx_Buf[3] == 0x03)
			{
				Send_ZigbeeData_To_Fifo(TrafficA_Yellow, 8); // 智能交通灯 -> 黄色
				delay_ms(200);
				Send_Debug_string("Yellow\n");
				delay_ms(200);
				SYN_Play("黄色");
			}
			else
			{
				Send_Debug_string("ERROR\n");
			}
			return Receive_succeed;
		}
	}
	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（静态标志物1A）
 * 参    数 ：  无
 * 返 回 值 ：  无
**********************************************************************/
uint8_t Static_marker_1[7]={0,0,0,0,0,0,'\0'}; //接收到的数据
uint8_t Receive_Static_marker_1_A(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA))
	{
		if ((Wifi_Rx_Buf[2] == 0xA2) && (Wifi_Rx_Buf[7] == 0xBB))
		{
			Static_marker_1[0] = Wifi_Rx_Buf[3];
			Static_marker_1[1] = Wifi_Rx_Buf[4];
			Static_marker_1[2] = Wifi_Rx_Buf[5];
			return Receive_succeed;
		}
	}
	return Receive_fail;
}
/**********************************************************************
 * 函 数 名 ：  接收Wifi数据（静态标志物1B）
 * 参    数 ：  无
 * 返 回 值 ：  无
**********************************************************************/
uint8_t Receive_Static_marker_1_B(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA) && (Wifi_Rx_Buf[7] == 0xBB))
	{
		if (Wifi_Rx_Buf[2] == 0xA3)
		{
			Static_marker_1[3] = Wifi_Rx_Buf[3];
			Static_marker_1[4] = Wifi_Rx_Buf[4];
			Static_marker_1[5] = Wifi_Rx_Buf[5];
			return Receive_succeed;
		}
	}
	return Receive_fail;
}

/**********************************************************************
 * 函 数 名 ：  平板控制
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Receive_Operation(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] == 0XAA) && (Wifi_Rx_Buf[7] == 0xBB))
	{
		switch (Wifi_Rx_Buf[2])
		{
		case 0x01:
			MasterCar_Stop();
			break;
		case 0x02:
			MasterCar_GoMP(Wifi_Rx_Buf[3], Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] * 0x100); // 主车前进
			break;
		case 0x03:
			MasterCar_BackMP(Wifi_Rx_Buf[3], Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] * 0x100); // 主车后退
			break;
		case 0x04:
			if (Wifi_Rx_Buf[3] == 0)
			{
				Left_hand_bend(95); // 主车左转
			}
			else
			{
				MasterCar_LeftMP(Wifi_Rx_Buf[3], Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] * 0x100); 
				//Full_RL_little(1, Wifi_Rx_Buf[3]); // 左微调
			}
			break;
		case 0x05:
			if (Wifi_Rx_Buf[3] == 0)
			{
				Right_hand_bend(95); // 主车右转
			}
			else
			{
				MasterCar_RightMP(Wifi_Rx_Buf[3], Wifi_Rx_Buf[4] + Wifi_Rx_Buf[5] * 0x100); 
				//Full_RL_little(2, Wifi_Rx_Buf[3]); // 右微调
			}
			break;
		case 0x06:
			MasterCar_SmartRun(Wifi_Rx_Buf[3]); // 主车循迹
			break;
		case 0xA2://二维码A
			Static_marker_1[0] = Wifi_Rx_Buf[3];
			Static_marker_1[1] = Wifi_Rx_Buf[4];
			Static_marker_1[2] = Wifi_Rx_Buf[5];
			break;
		case 0xA3://二维码B
			break;
			Static_marker_1[3] = Wifi_Rx_Buf[3];
			Static_marker_1[4] = Wifi_Rx_Buf[4];
			Static_marker_1[5] = Wifi_Rx_Buf[5];
		default:
			break;
		}
	}
}
