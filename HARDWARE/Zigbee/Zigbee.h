#ifndef __ZIGBEE_H
#define __ZIGBEE_H
#include "stm32f4xx.h"	  

#define Receive_succeed 		((uint8_t)0x11)  //接收成功
#define Receive_fail 				((uint8_t)0x00)  //接收失败

#define TFT_A 							((uint8_t)0xB0)  //TFT_A
#define TFT_B 							((uint8_t)0xB1)  //TFT_B
#define traffic_light_A  		((uint8_t)0xB2)  //交通灯A
#define traffic_light_B 		((uint8_t)0xB3)  //交通灯B
#define Static_marker_A 		((uint8_t)0xB4)  //静态标志物1
#define Static_marker_B		  ((uint8_t)0xB5)  //静态标志物2
#define Trigger_car 				((uint8_t)0xB6)  //启动副车
#define Receive_Trigger_car ((uint8_t)0xCC)  //接收副车数据
#define Data_display        ((uint8_t)0xCA)  //显示数据到平板界面上
extern uint8_t TFT_license_plate[6];    //TFT车牌数据
extern uint8_t Graph[5];                //接收图形形状数据。
extern uint8_t street_lamp_data;        //接收路灯档位
extern uint8_t RFID_secret_key[6];      //接收RFID密钥
extern uint8_t Static_marker_1[7];      //接收二维码数据
extern uint8_t Vice_car_QR[6];					//接收副车二维码数据
extern uint8_t plate_number[6];         //Zigbee接收的车牌数据
extern uint8_t  distance_QRcode[3];     //接收距离/二维码数字

static uint8_t Send_flat_computer [8]={0X55,0X11,0x00,0x00,0x00,0x00,0x00,0XBB};    //上传调试信息发送数据给平板
static uint8_t transpond 					[8]={0X55,0X11,0xCC,0x00,0x00,0x00,0x00,0XBB};    //发送数据给平板
static uint8_t transpond_wifi 		[8]={0X55,0X00,0x00,0x00,0x00,0x00,0x00,0XBB};    //WIFI发送数据给Zigbee

//2021
static uint8_t Send_Vice_car_Fifo[8]={0X55,0X02,0x00,0x00,0x00,0x00,0x00,0XBB};//发送给副车的数据结构
static uint8_t Secondary_car_start[8]={0X55,0X02,0x00,0x00,0x00,0x00,0x00,0XBB};//启动副车指令

void Send_Zigbee_Data(uint8_t instruct,uint8_t coord1,uint8_t coord2,uint8_t coord3);//发送到Zigbee

/*Zigbee主副车通讯*/
uint8_t  Receive_start_instruction(void);  //接收副车（启动）指令
uint8_t  Receive_barrier_gate (void);      //接收副车（启动道闸）指令
uint8_t  Receive_distance_QRcode(void);		 //接收距离/二维码数字
uint8_t  Receive_street_lamp(void);				 //接收路灯档位数据
uint8_t  Receive_plate_number(void);       //接收车牌数据
uint8_t  Receive_ETC_detection(void);      //接收ETC回传
uint8_t  Receive_QR_Code_A(void);					 //接收副车二维码A
uint8_t  Receive_QR_Code_B(void);					 //接收副车二维码B


uint8_t transmit(void);                                 //zigbee转发至wifi
uint8_t transmit_wifi(void);                         	 //wifi转发至zigbee
void Send_street_lamp_gear(uint8_t instruct);        //发送路灯档位到wifi

void Send_Wifi(uint8_t instruct,uint8_t coord1,uint8_t coord2,uint8_t coord3);//发送数据到Wifi
void uploading_flat_computer(uint8_t instruct);      //发送数据给Wifi
uint8_t Receive_flat_computer_first_start(void);     //接收Wifi数据（首次启动）
uint8_t Receive_flat_computer_second_start(void);    //接收Wifi数据（后续启动）
uint8_t Receive_flat_computer_TFT(void);             //接收Wifi数据（TFT识别到的车牌数据）
uint8_t Receive_flat_computer_graph(void);           //接收Wifi数据（图形形状）
uint8_t Receive_flat_computer_RFID(void);            //接收Wifi数据（RFID前密钥）
uint8_t Receive_Traffic_light_identification(void);  //接收Wifi数据（交通灯识别）
uint8_t Receive_Static_marker_1_A(void);               //接收Wifi数据（静态标志物1A）
uint8_t Receive_Static_marker_1_B(void);               //接收Wifi数据（静态标志物1B）

/*Wifi平板通讯*/
void Debug_wifi_ASCLL(char instruct_1,char instruct_2,char instruct_3);							//上传调试信息（WIFI-平板）ASCLL
void Debug_wifi(uint8_t instruct_1,uint8_t instruct_2,uint8_t instruct_3);					//上传调试信息（WIFI-平板）十进制
void Debug_wifi_HEX(uint8_t instruct_1,uint8_t instruct_2,uint8_t instruct_3);			//上传调试信息（WIFI-平板）十六进制
void Receive_Operation (void);																										//平板控制

#endif

