#ifndef __VOLUNTARILY_H
#define __VOLUNTARILY_H
#include "sys.h"

#define ALL_Flag 					 		0x00				//ȫ��������
#define QR_code_Falg 			 		0xAA			  //ƽ���ά��
#define Plate_number_Falg	 		0xBB 				//ƽ�峵��
#define Graph_Falg 			   		0xCC 				//ƽ��ͼ��
#define Traffic_light_Falg 		0xDD        //ƽ�彻ͨ��
#define S_vehicle_Start 		  0xEE 				//�ӳ���������
#define S_vehicle_QR 		      0xEE 				//�ӳ���������


extern uint8_t Detected;//�����Ƿ��⵽

void key(void);																															//����ɨ��
void Car_Thread(void);																											//ȫ�Զ����к���
void Zigbee_Wifi_receive(void);																							//Zigbee+WIFI�������ݴ���
void Interrupt_flag_bit_query(void);																				//�жϱ�־λ��ѯ
void Timeout_detection(uint16_t Time,uint8_t position,uint8_t Return_run);	// ������ʱ���

#endif
