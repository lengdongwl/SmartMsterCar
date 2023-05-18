/*
 * @Description: ����ͨѶЭ�������
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 10:02:43
 * @version: 2021
 */
#ifndef __OperationFlag_H
#define __OperationFlag_H
#include "CanP_HostCom.h"

#define zNum_MasterCar 0x01 //����
#define zNum_SlaveCar 0x02  //�ӳ�
#define zNum_DZ 0x03        //��բ��־��
#define zNum_LED 0x04       //LED��ʾ��־��
#define zNum_CK_B 0x05      //���峵��B��־��
#define zNum_YY 0x06        //����������־��
#define zNum_alarm 0x07     //���̨��������־��
#define zNum_TFT_B 0x08     //TFTB��ʾ��־��
#define zNum_light 0x09     //����·�Ʊ�־��
#define zNum_WX 0x0A        //���߳���־��
#define zNum_TFT_A 0x0B     //TFTA��ʾ��־��
#define zNum_ETC 0x0C       //ETCϵͳ��־��
#define zNum_CK_A 0x0D      //���峵��A��־��
#define zNum_JT_A 0x0E      //���ܽ�ͨ��־��A
#define zNum_JT_B 0x0F      //���ܽ�ͨ��־��B
#define zNum_DX 0x10        //������α�־��
#define zNum_LT 0x11        //������ʾ��־��

extern uint8_t OFlag_SLAVEflag;
extern uint8_t OFlag_ETCflag;
uint8_t OFlag_GetCmd(uint8_t *status);  //��ȡ��ָ��
uint8_t OFlag_GetCmd1(uint8_t *status); //��ȡ��ָ��1
uint8_t OFlag_GetCmd2(uint8_t *status); //��ȡ��ָ��2
uint8_t OFlag_GetCmd3(uint8_t *status); //��ȡ��ָ��3

unsigned int OFlag_check(uint8_t *data);                                                         //����Ƿ�ΪZigbee�ڵ�İ�
void Agreement_Send(unsigned char mode, uint8_t *data);                                          //Э��ѡ���ͷ�ʽ��������
void OFlag_Zigbee(uint8_t zigbee_num, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //ZigbeeͨѶ

unsigned int OFlag_light(unsigned int gear);  //����·�Ʊ�־�ﵵλ����
unsigned int OFlag_light2(unsigned int gear); //����·�Ʊ�־�ﵵλ���ã��жϺ������Ƿ���ɹ��棩

void OFlag_alarm(unsigned char mode);         //���̨������־�� ���������رղ���
void OFlag_alarm_open(unsigned char *KEY);    //���Ϳ����뿪�����̨
void OFlag_alarm_change(uint8_t *data);       //���̨�޸Ŀ�����
uint8_t OFlag_alarm_getP(uint8_t t);          //���̨��ȡ�����Ԯ����

void OFlag_ltDisplay(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4); //������ʾ��־������֡����
void OFlag_ltDisplay_show(uint8_t *CP, uint8_t *coordinate);                                   //������ʾ��־�ﳵ�Ƽ�������ʾ
void OFlag_ltDisplay_Text(uint8_t *str);                                                       //������ʾ��־���Զ����ı�
void OFlag_LED(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3);                      //LED��ʾ��־�����
void OFlag_LED_time(uint8_t cmd);                                                              //LED��ʾ��־����� ��ʱģʽ
void OFlag_LED_show(uint8_t row, uint8_t data12, uint8_t data34, uint8_t data56);              //LED��ʾ��־����� ָ������ʾָ������
void OFlag_LED_jl(unsigned int mm);                                                            //LED��ʾ��־����� ��ʾ����ģʽ

void OFlag_DZ(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //��բ��־�����
void OFlag_DZ_open(uint8_t *str);                                        //��բ����
void OFlag_DZ_close(void);                                               //��բ�ر�
void OFlag_DZ_show(uint8_t *str);                                        //��բ��ʾ����
unsigned int OFlag_DZ_wait(uint8_t count, uint8_t *str);                 //�򿪵�բ�ҵȴ���բ���ڴ�״̬
unsigned int OFlag_DZ_status(uint8_t *status);                           //��բ��־����� ��բ״̬����

void OFlag_WX_open(void); //���߳���־�￪�� 10����Զ��ر�
void OFlag_WX_close(void);//���߳���־��ر�

void OFlag_YY(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //����������־�����
void OFlag_YY_cmd(uint8_t cmd);                                          //��������0.��� 1-7ָ����Ų���
void OFlag_YY_set(uint8_t mode, uint8_t t1, uint8_t t2, uint8_t t3);     //������ʼ���ڡ�ʱ�䡢�������¶�
uint8_t *OFlag_YY_get(uint8_t mode, uint32_t count);                     //��ȡ��ʼ���ڡ�ʱ�䡢�������¶�
uint8_t OFlag_YY_play(uint8_t *str,uint8_t encode);                      //����������־��ϳ�ָ�������ı�

void OFlag_TFT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //TFT��־�����
void OFlag_TFT_show(uint8_t mode, uint8_t *str);                                        //TFT��ʾ����
void OFlag_TFT_showHEX(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6);
void OFlag_TFT_jl(uint8_t mode, uint32_t data); //TFT��ʾ����

void OFlag_JT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //���ܽ�ͨ�Ʊ�־�����
void OFlag_JT_cmd(uint8_t mode, uint8_t sb);                                           //���ܽ�ͨ�Ʊ�־����� ����ʶ��ģʽ
unsigned int OFlag_JT_status(uint8_t *status, uint8_t mode);                           //���ܽ�ͨ�Ʊ�־����� ״̬����
//unsigned int OFlag_JT_Wait(uint8_t mode);                                              //���ܽ�ͨ�ƽ���ʶ��ģʽ�ҵȴ�ʶ���� �ȴ�ʱ��10s

void OFlag_CK(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //���峵���־�����
void OFlag_CK_cmd(uint8_t mode, uint8_t num);                                          //���峵���־�� ����ָ�����
uint8_t OFlag_CK_statusGet(uint8_t mode, uint8_t num, uint8_t count);                  //���峵���־�� ���󷵻س���״̬��Ϣ
unsigned int OFlag_CK_Wait(uint8_t mode, uint8_t num, uint8_t count);                  //���峵���־�ﵽ��ָ����β��ҵȴ��������
unsigned int OFlag_CK_status(uint8_t *status, uint8_t CK, uint8_t mode);               //���峵���־��״̬����

void OFlag_ETC(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //ETC��־�����
void OFlag_ETC_cmd(uint8_t mode, uint8_t cmd);                            //ETC��־������բ�ſ���
unsigned int OFlag_ETC_status(uint8_t *status);                           //��ȡբ��״̬ 10��բ���Զ��ر�
void OFlag_ETC_wait(void);                                                //����ETC�������

void OFlag_DX(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //������α�־�����
void OFlag_DX_statusGet(void);                                           //�����ѯ����ͨ��״̬
unsigned int OFlag_DX_status(uint8_t *status);                           //��ȡ����ͨ��״̬
unsigned int OFlag_DX_wait(uint8_t count);                               //������εȴ���ȡ����ͨ��״̬
void OFlag_DX_carGo(unsigned int speed, unsigned int refMP);             //����ɫ˫���������
void OFlag_YYupload(uint8_t num);                                        //����ƽ̨�������Զ�����ϵͳ�ϴ��������

void OFlag_SlaveRun(void);                                                            //��������Zigbeeָ����������
uint8_t OFlag_SlaveRun_wait(uint8_t count);                                           //���������󲢵ȴ�������������(��bug��δʹ��)
uint8_t OFlag_SlaveRun_wait_flag(uint8_t count,uint8_t flag);
void OFlag_SlaveSendZigbee(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //��������Zigbeeָ�����
#endif
