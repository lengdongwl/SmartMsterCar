/*
 * @Description: PIDѭ��
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-07 14:58:42
 */
#ifndef __PID_H
#define __PID_H
void PID_reset(void);                      //ѭ���������ò���
float PID_Track(unsigned int speed);       //��������ѭ��(�Զ��������� ȫ���복ͷ�����м�ֱ�� ����״̬����99)
float PID_Track2(unsigned int speed);      //��������ѭ��(�Զ��������� ��4�������ϵĵƷ���99)
float PID_Track2_plus(unsigned int speed); //��ǿ��ͣ��
float PID_Track3(unsigned int speed);      //Ѱ��ѭ���������(ȫ������99�޲���)

float PID_TrackMP(unsigned int speed);         //ѭ�����̰�
float PID_TrackMP2(unsigned int speed);        //���ٵ�������
void PID_Set(float K_P, float K_I, float K_D); //PID����
void PID_Set_recovery(void);                   //�ָ�PID_Set();ǰ����

#endif
