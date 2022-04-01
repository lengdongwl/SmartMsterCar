#ifndef __VOLUNTARILY_H
#define __VOLUNTARILY_H
#include "sys.h"

#define ALL_Flag 					 		0x00				//全部不限制
#define QR_code_Falg 			 		0xAA			  //平板二维码
#define Plate_number_Falg	 		0xBB 				//平板车牌
#define Graph_Falg 			   		0xCC 				//平板图形
#define Traffic_light_Falg 		0xDD        //平板交通灯
#define S_vehicle_Start 		  0xEE 				//从车控制启动
#define S_vehicle_QR 		      0xEE 				//从车控制启动


extern uint8_t Detected;//地形是否检测到

void key(void);																															//按键扫描
void Car_Thread(void);																											//全自动运行函数
void Zigbee_Wifi_receive(void);																							//Zigbee+WIFI接收数据处理
void Interrupt_flag_bit_query(void);																				//中断标志位查询
void Timeout_detection(uint16_t Time,uint8_t position,uint8_t Return_run);	// 启动超时检测

#endif
