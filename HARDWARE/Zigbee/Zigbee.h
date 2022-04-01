#ifndef __ZIGBEE_H
#define __ZIGBEE_H
#include "stm32f4xx.h"	  

#define Receive_succeed 		((uint8_t)0x11)  //���ճɹ�
#define Receive_fail 				((uint8_t)0x00)  //����ʧ��

#define TFT_A 							((uint8_t)0xB0)  //TFT_A
#define TFT_B 							((uint8_t)0xB1)  //TFT_B
#define traffic_light_A  		((uint8_t)0xB2)  //��ͨ��A
#define traffic_light_B 		((uint8_t)0xB3)  //��ͨ��B
#define Static_marker_A 		((uint8_t)0xB4)  //��̬��־��1
#define Static_marker_B		  ((uint8_t)0xB5)  //��̬��־��2
#define Trigger_car 				((uint8_t)0xB6)  //��������
#define Receive_Trigger_car ((uint8_t)0xCC)  //���ո�������
#define Data_display        ((uint8_t)0xCA)  //��ʾ���ݵ�ƽ�������
extern uint8_t TFT_license_plate[6];    //TFT��������
extern uint8_t Graph[5];                //����ͼ����״���ݡ�
extern uint8_t street_lamp_data;        //����·�Ƶ�λ
extern uint8_t RFID_secret_key[6];      //����RFID��Կ
extern uint8_t Static_marker_1[7];      //���ն�ά������
extern uint8_t Vice_car_QR[6];					//���ո�����ά������
extern uint8_t plate_number[6];         //Zigbee���յĳ�������
extern uint8_t  distance_QRcode[3];     //���վ���/��ά������

static uint8_t Send_flat_computer [8]={0X55,0X11,0x00,0x00,0x00,0x00,0x00,0XBB};    //�ϴ�������Ϣ�������ݸ�ƽ��
static uint8_t transpond 					[8]={0X55,0X11,0xCC,0x00,0x00,0x00,0x00,0XBB};    //�������ݸ�ƽ��
static uint8_t transpond_wifi 		[8]={0X55,0X00,0x00,0x00,0x00,0x00,0x00,0XBB};    //WIFI�������ݸ�Zigbee

//2021
static uint8_t Send_Vice_car_Fifo[8]={0X55,0X02,0x00,0x00,0x00,0x00,0x00,0XBB};//���͸����������ݽṹ
static uint8_t Secondary_car_start[8]={0X55,0X02,0x00,0x00,0x00,0x00,0x00,0XBB};//��������ָ��

void Send_Zigbee_Data(uint8_t instruct,uint8_t coord1,uint8_t coord2,uint8_t coord3);//���͵�Zigbee

/*Zigbee������ͨѶ*/
uint8_t  Receive_start_instruction(void);  //���ո�����������ָ��
uint8_t  Receive_barrier_gate (void);      //���ո�����������բ��ָ��
uint8_t  Receive_distance_QRcode(void);		 //���վ���/��ά������
uint8_t  Receive_street_lamp(void);				 //����·�Ƶ�λ����
uint8_t  Receive_plate_number(void);       //���ճ�������
uint8_t  Receive_ETC_detection(void);      //����ETC�ش�
uint8_t  Receive_QR_Code_A(void);					 //���ո�����ά��A
uint8_t  Receive_QR_Code_B(void);					 //���ո�����ά��B


uint8_t transmit(void);                                 //zigbeeת����wifi
uint8_t transmit_wifi(void);                         	 //wifiת����zigbee
void Send_street_lamp_gear(uint8_t instruct);        //����·�Ƶ�λ��wifi

void Send_Wifi(uint8_t instruct,uint8_t coord1,uint8_t coord2,uint8_t coord3);//�������ݵ�Wifi
void uploading_flat_computer(uint8_t instruct);      //�������ݸ�Wifi
uint8_t Receive_flat_computer_first_start(void);     //����Wifi���ݣ��״�������
uint8_t Receive_flat_computer_second_start(void);    //����Wifi���ݣ�����������
uint8_t Receive_flat_computer_TFT(void);             //����Wifi���ݣ�TFTʶ�𵽵ĳ������ݣ�
uint8_t Receive_flat_computer_graph(void);           //����Wifi���ݣ�ͼ����״��
uint8_t Receive_flat_computer_RFID(void);            //����Wifi���ݣ�RFIDǰ��Կ��
uint8_t Receive_Traffic_light_identification(void);  //����Wifi���ݣ���ͨ��ʶ��
uint8_t Receive_Static_marker_1_A(void);               //����Wifi���ݣ���̬��־��1A��
uint8_t Receive_Static_marker_1_B(void);               //����Wifi���ݣ���̬��־��1B��

/*Wifiƽ��ͨѶ*/
void Debug_wifi_ASCLL(char instruct_1,char instruct_2,char instruct_3);							//�ϴ�������Ϣ��WIFI-ƽ�壩ASCLL
void Debug_wifi(uint8_t instruct_1,uint8_t instruct_2,uint8_t instruct_3);					//�ϴ�������Ϣ��WIFI-ƽ�壩ʮ����
void Debug_wifi_HEX(uint8_t instruct_1,uint8_t instruct_2,uint8_t instruct_3);			//�ϴ�������Ϣ��WIFI-ƽ�壩ʮ������
void Receive_Operation (void);																										//ƽ�����

#endif

