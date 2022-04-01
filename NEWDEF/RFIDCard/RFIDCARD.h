/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-11 10:56:30
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-19 20:22:45
 */
#ifndef __RFIDCARD_H
#define __RFIDCARD_H
#include "stdint.h"
extern uint8_t K_A[6]; // Ĭ��A��Կ
typedef struct 
{
    uint8_t success_count;//�����ɹ���������
    uint8_t total_count;//��ȡ���ܴ���(�����ڱ�������������)
    uint8_t success_position[50];//�����ɹ���λ��
    /*uint8_t **key_data;//KEY����
    uint8_t *block_address_data;//������*/
    //uint8_t **ref_data;//��������(�ÿ������ɹ���ֵ���)
}type_CardRBuf;//�������ؽ������

uint8_t RC_Card_checkCard(void);                  //ͨ����ɫ��ֵ�ж��Ƿ�Ϊ�׿�
uint8_t RC_Get_address(uint8_t S, uint8_t Block); //��ַ����
uint8_t RC_check(void);                           //���RFID��
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY);
uint8_t *RC_Get_buffer(void);                                         //find��ϻ�����
uint8_t *RC_Get_bufferN(uint8_t position);                             //find��ϻ����� �࿨
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY);            //��ȡRFID������ Ѱ������ſɶ�ȡ����
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data); //��RFID��д����

uint8_t MasterCar_findCar(uint8_t Block_address, uint8_t *KEY);               //����ֱ��ѭ��ѭ��Ѱ��
uint8_t MasterCar_findCar2(uint8_t Block_address, uint8_t *KEY);              //����ֱ��ѭ��ѭ��Ѱ��(����ֹͣ��)
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp); //����ֱ��ѭ�����̰� ���Ժڰ���

uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance); //��ָ�����뷶Χ�ڲ⿨
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance,float p,float i,float d);
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);//��ָ�������ڶ���
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);
uint8_t RC_Card_checkCardOrDX(int16_t init_distance, uint16_t distance);//ָ����Χ�� ����м�����Ϊ������·�� ������������ʾ1/2����

/*�������� 
final �п�:��ȡ�ɹ��˳� �޿�: ��ʻ�����ֵ
one ����
two ˫��
*/
uint8_t RC_Card_final_P1P2(int16_t init_distance,uint16_t distance,uint8_t Block_address1,uint8_t Block_address2);//2020����ʡ���ֱ����1.��2
uint8_t RC_Card_final_P1P2_2(uint16_t distance,uint8_t Block_address1,uint8_t Block_address2,uint8_t *KEY1,uint8_t *KEY2);
type_CardRBuf RC_Card_oneFunc1(uint16_t distance,uint8_t *Block_address,uint8_t *KEY);//2020����ʡ�����ζ����ο������ַΪ�����,��ͨ�����ؽ����ȡ�������ɹ���λ��
uint8_t RC_Card_oneFunc2(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//���ζ�˫�����ж϶�����λ����Ϣ
type_CardRBuf RC_Card_oneFunc3(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//��˫�Σ������ζ����ο�
uint8_t RC_Card_oneFunc4(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);

type_CardRBuf RC_Card_twoFunc1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//˫���ж�·�϶���
type_CardRBuf RC_Card_ALLFun1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY,uint8_t count);

#endif
