#ifndef __CARD_H
#define __CARD_H
#include "sys.h"

extern uint8_t RFID_Card_One[16];   //�������ݻ����� 1
extern uint8_t RFID_Card_Two[16];	  //�������ݻ����� 2
extern uint8_t RFID_Card_Three[16];	//�������ݻ����� 3
extern uint8_t Read_Flag[3];	      //������־λ


uint8_t Card_One(uint16_t distance);		// ���μ��(·��-����-����)
uint8_t Card_Two(void);									// Ѱ�յ㿨(����)
uint8_t Card_Three(uint16_t distance);	// ˫�μ�⣨·��-����-·��-����-·��-������
uint8_t Card_Four(uint16_t distance);		// ˫����·�ϼ�⣨·�ϣ�
uint8_t Card_Five(uint16_t distance);		// ���μ��ͣ��·��-������
uint8_t Card_SIX(uint16_t distance);		// ˫�μ��·�ϲ����յ㿨��·��-������







void Obtain_section_distance(void);							//�������ξ���
void RFID_detection_motion(void);								//΢����⿨
uint8_t Auto_write_card(uint8_t Block_address); //д��
uint8_t Auto_Read_card(uint8_t Block_address);	//����

#endif
