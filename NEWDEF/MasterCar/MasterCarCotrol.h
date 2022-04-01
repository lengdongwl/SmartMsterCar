/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-03-16 10:04:27
 */
#ifndef __MasterCarCotrol_H
#define __MasterCarCotrol_H

extern unsigned int MasterCar_GoMpValue; //����ʱǰ������ֵ
extern unsigned int MasterCar_GoSpeed;   //����Ĭ��ѭ�������ٶ�
void MasterCar_DebugKEY(void);                              
void MasterCar_Stop(void);                                   //������ͣ
void MasterCar_Right(unsigned int speed, unsigned int mode); //������ת
void MasterCar_RightMP(unsigned int speed, unsigned int mp); //������ת����
void MasterCar_Left(unsigned int speed, unsigned int mode);  //������ת
void MasterCar_LeftMP(unsigned int speed, unsigned int mp);  //������ת����

void MasterCar_Back(unsigned int speed);                        //��������
void MasterCar_Go(unsigned int speed);                          //����ǰ��������ѭ����Go ת��ʱ��GoMP��
void MasterCar_GoMP(unsigned int speed, unsigned int mp);       //����ǰ������ֵ
void MasterCar_BackMP(unsigned int speed, unsigned int mp);     //������������ֵ
void MasterCar_SmartRun(unsigned int speed);                    //����ѭ������
void MasterCar_SmartRun1(unsigned int speed);                   //����ѭ������ �����׿�����ʮ��·�ڵĳ���
void MasterCar_SmartRun2(unsigned int speed);                   //����ѭ������ Ѱ�κ����Ƶİ׿�
void MasterCar_SmartRunMP(unsigned int speed, unsigned int MP); //����ѭ����������ֵ
void MasterCar_SmartRunMP2(unsigned int speed, unsigned int MP);//����ѭ����������ֵ����������
void MasterCar_BackEnter(unsigned int mp);                      //�����������
void MasterCar_TaskRunThread(void);                             //��������ִ���߳�
void MasterCar_TaskReceiveThread(void);                         //������������߳�
void Set_Flag_Track(unsigned int status);                       //����ѭ����־����
void Set_Flag_Task(unsigned int task);                          //��������ѡ��
unsigned char Get_Flag_Track(void);                             //����ѭ����־��ȡ
unsigned char Get_Flag_Task(void);                              //���������־��ȡ
void SlaveCar_TaskRunThread(unsigned char *data);               //��������Zigbee���ո�������ָ��


/*********************************************************************/
void task_first(void);	  //��ʼ����
void task_second(void);   //��������
void task_ETC(void);	  //�ȴ�ETC��բ�����
void task_test(void);	  //��������
void task_wait(void);	  //�ȴ���������ָ��
void task_waitWifi(void); //wifi����ָ��
void task_RFID(void);	  //��RFID������
/*********************************************************************/
#endif
