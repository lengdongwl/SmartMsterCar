/*
 * @Description: 
 * @Autor: 309 Mushroom
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-10-29 14:18:48
 */
#ifndef __FUNCTION_H
#define __FUNCTION_H
#include "sys.h"

#define east ((uint8_t)0x01)  //��
#define south ((uint8_t)0x02) //��
#define west ((uint8_t)0x03)  //��
#define north ((uint8_t)0x04) //��

/*����·������*/
#define B1 ((uint8_t)0xB1)
#define B2 ((uint8_t)0xB2)
#define B4 ((uint8_t)0xB4)
#define B6 ((uint8_t)0xB6)
#define B7 ((uint8_t)0xB7)

#define D1 ((uint8_t)0xD1)
#define D2 ((uint8_t)0xD2)
#define D4 ((uint8_t)0xD4)
#define D6 ((uint8_t)0xD6)
#define D7 ((uint8_t)0xD7)

#define F1 ((uint8_t)0xF1)
#define F2 ((uint8_t)0xF2)
#define F4 ((uint8_t)0xF4)
#define F6 ((uint8_t)0xF6)
#define F7 ((uint8_t)0xF7)
/***********************************/
extern uint8_t Two_car[16];      //�����ſ���־λ
extern uint8_t make_Yes;         //ȫ�Զ���־λ
extern uint16_t TIM3_Flag;       //��ʱ��3��־λ
extern uint8_t Read_Car_Address; //�����Ŀ��ַ
extern uint8_t KEY_Open[6];      //RFID��

void SYN_Play(char *Pst);                                                  //������������
uint8_t Dimming(uint8_t target);                                           //����
uint16_t Stadiometry(void);                                                //���
void ETC_detection(void);                                                  //ETC���
void Eliminate_Flag(void);                                                 //���wifi/zigbee������ɱ�־λ
uint8_t Light_djustment(uint8_t Sun_gear);                                 //�Զ�����
uint16_t measure_display(void);                                            //���������
void Hardware_Init(void);                                                  //Ӳ����ʼ��
void MP_Tracking(uint8_t sp, u16 mp);                                      //ȫ�Զ�����ѭ��
void Full_Tracking(uint8_t sp);                                            //ȫ�Զ�ѭ��
void Full_STOP(void);                                                      //ͣ����
void Full_RL_little(uint8_t RL, uint8_t JY);                               //΢��λ��
void Full_adjust(void);                                                    //ת���΢��
void Full_Left(u8 sp);                                                     //ȫ�Զ���ת
void Left(u8 sp);                                                          //��ת
void Full_Right(u8 sp);                                                    //ȫ�Զ���ת
void Right(u8 sp);                                                         //��ת
void Free_path(uint8_t start, uint8_t finish, uint8_t direction);          //����·�� 1.0A
void Free_path2(uint8_t start, uint8_t finish, uint8_t direction);         //����·�� 1.0B
void Head_on_adjustment(uint8_t direction, uint8_t need_direction);        //��ͷ�������
void Directional_X(int difference_X, int difference_Y, uint8_t direction); //�����ֵ��ͷ���������X�����ȣ�
void Directional_Y(int difference_X, int difference_Y, uint8_t direction); //�����ֵ��ͷ���������X�����ȣ�
void TIM3_Int_Init(u16 arr, u16 psc);                                      //��ʱ��3��ʼ��
void RFID_terrain_detection(uint16_t distance);                            //RFID���-���μ��
void Digital_tube_TEXT(void);                                              //��������
void RFID_detection_motion(void);                                          //RFID΢�����
uint16_t Full_Measure(void);                                               //��β�ຯ��
void Road_Test(uint16_t distance);                                         //·�����(����)
void Read_Car_single(uint16_t distance);                                   //���ζ���
void voice(uint16_t number, char *unt);                                    //�����������ּӵ�λ
void Running_indicator_light(void);                                        //����ָʾ��
void Battery_power_detection(void);                                        //��ص������
void Into_the_garage(void);                                                //�ó���ﵽ��һ��
void Garage(uint8_t Tier);                                                 //����������
void Beacon_change_password(uint8_t PassWord[6]);                          //���̨������
void Voice_control(void);                                                  //С������ʶ����

void Right_hand_bend(u8 sp);
void Left_hand_bend(u8 sp);

void Right_MP(u8 sp);
void Left_MP(u8 sp);

#endif
