/*
 * @Description: WiFi���մ����
 * @Autor: 309
 * @Date: 2021-10-15 16:34:03
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-19 19:54:57
 */
#ifndef __OperationWiFi_H
#define __OperationWiFi_H
#include "can_user.h"

extern uint8_t OWiFi_END;         //WIFI��ɽ��ս�����־
extern uint8_t OWifi_CRCode[7];   //���ն�ά��
extern uint8_t OWifi_TFTCP[7];    //����TFT����
extern uint8_t OWifi_TFTShape[5]; ////[0]:����,[1]:Բ��,[2]:����,[3]:����,[4]:�����
extern uint8_t OWifi_alarm[7];    //���շ��̨ 1.
extern uint8_t OWifi_JT;          //���ս�ͨ������
extern uint8_t OWifi_TFTJTFLAG;   //����TFTʶ��Ľ�ͨ��־2.��ͷ 3.��ת 4.ֱ�� 5.��ת 6.��ֹͨ�� 7.��ֱֹ��

void OWiFi_Send(uint8_t instruct, uint8_t coord1, uint8_t coord2, uint8_t coord3); //�����������ݵ�ƽ��
void OWiFi_Send_test(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3);   //��ƽ���ϴ�������Ϣ

void Operation_WiFi(void);                            // WiFiʶ��̬��־����ơ�ͼ�εȽ��
uint8_t OWiFi_cmd(uint8_t cmd, uint32_t time);        //wifi������ָ��ʶ������
uint8_t OWiFi_QRCode1(uint32_t time);                 //����ʶ��̬��־��1
uint8_t OWiFi_QRCode2(uint32_t time);                 //����ʶ��̬��־��2
uint8_t OWiFi_TFT(uint8_t AorB, uint32_t time);       //����ʶ��TFT���ơ���״
uint8_t OWiFi_JTlight(uint8_t AorB);                  //����ͨ��ʶ�� �����־����ȷ��ʶ����
uint8_t OWiFi_CodeCalc(uint8_t *data, uint32_t time); //��������ѹ���㷨����
uint8_t OWiFi_toZigbee(void);                         //WiFi������Zigbee��Ϣ
void Operation_WiFi_Master(uint8_t *buf);             //WIFI�ٿ�����

#endif
