/*
 * @Description: PIDѭ��
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-22 11:17:17
 */
#ifndef __PID_H
#define __PID_H
void PID_reset(void);//ѭ���������ò���
float PID_Track(unsigned int speed);//��ͨѭ��
float PID_Track2(unsigned int speed);//Ѱ��ѭ��
float PID_Track3(unsigned int speed);//Ѱ��ѭ���������(���߷���99�޲���)
float PID_Track4(unsigned int speed);//Ѱ��������(����ֱ�з���99��ͣ��)
float PID_Track5(unsigned int speed);//ʮ��·�ڿ��ܴ��ڰ׿���

float PID_TrackMP(unsigned int speed);//ѭ�����̰�
float PID_TrackMP2(unsigned int speed);//���ٵ�������
void PID_Set(float K_P,float K_I,float K_D);//PID����
void PID_Set_recovery(void);//�ָ�PID_Set();ǰ����

#endif
