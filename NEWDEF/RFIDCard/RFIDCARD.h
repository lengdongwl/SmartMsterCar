/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-11 10:56:30
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-03-14 15:23:47
 */
#ifndef __RFIDCARD_H
#define __RFIDCARD_H
#include "stdint.h"
extern uint8_t K_A[6]; // Ĭ��A��Կ

uint8_t RC_Card_checkCard(void);                                      //ͨ����ɫ��ֵ�ж��Ƿ�Ϊ�׿� ������̫��
uint8_t RC_Get_address(uint8_t S, uint8_t Block);                     //��ַ����
uint8_t RC_check(void);                                               //���RFID��
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY);           //���Ѽ��Ŀ�Ƭ���ݶ�ȡ
uint8_t *RC_Get_buffer(void);                                         //find��ϻ�����
uint8_t *RC_Get_bufferN(uint8_t position);                            //find��ϻ����� �࿨
void *RC_Set_bufferN(uint8_t position,char *data);                    //���û�������
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY);                //��ȡRFID������ Ѱ������ſɶ�ȡ����
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data); //��RFID��д����

void TIM_PIDTrack_Set(uint8_t flag);
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp); //��⵽����ʹ��

uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance); //��ָ�����뷶Χ�ڲ⿨
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance, float p, float i, float d);
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);                //��ָ�������ڶ���
uint8_t RC_Card_checkRangeReadN(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);               //��ָ�������ڶ�N�ſ�
uint8_t RC_Card_checkRangeReadPlus(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //��ָ�������ڶ�����ѭ���ư�
uint8_t RC_Card_checkRangeReadPlus2(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //��ָ�������ڶ���ѭ���ư�
uint8_t RC_Card_checkRangeReadPlus3(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //��ָ�������ڶ���ѭ���ư�
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);

#endif
