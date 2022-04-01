#ifndef __AGV_H
#define __AGV_H
#include "sys.h"

void AGV_Data_Open(void);	// AGV�������ݴ���
void AGV_Data_Stop(void);	// AGVֹͣ���ݻش�
void AGV_GO(uint8_t speed, uint16_t mp);	// AGVǰ�� �������ٶ� ����
void AGV_Break(uint8_t speed, uint16_t mp);	// AGV���� �������ٶ� ����
void AGV_Track(uint8_t speed);	// AGVѭ�� �������ٶ�
void AGV_Reght(uint8_t speed);	// AGV��ת �������ٶ�
void AGV_Left(uint8_t speed);		// AGV��ת �������ٶ�
void AGV_STOP(void);		// AGVֹͣ
void AGV_Beep(uint8_t mode);		// AGV_Zigbee���������� ����������
void AGV_Infrared(uint8_t *Src);	// AGV_Zigbee���ͺ���
void AGV_SendData(uint8_t Pri,uint8_t Sec1,uint8_t Sec2,uint8_t Sec3);	// AGV��������
void AGV_Wheel_L45(uint8_t speed,uint16_t time);	// AGV��ת45 �������ٶ�
void AGV_Wheel_R45(uint8_t speed,uint16_t time);	// AGV��ת45 �������ٶ�
void AGV_Thread(uint8_t mode);			// AGV�Զ���ʻ
void AGV_GetThread(uint8_t mode);		// AGVȫ�Զ���ɱ�־��ȡ
	
#endif

