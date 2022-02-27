/**********************************************************************
 * ��������·�ϵ���غ���
 * 1.0 21/3/17    1837   2285
 

**********************************************************************/
#include "Card.h"
#include "function.h"
#include "canp_hostcom.h"
#include "roadway_check.h"
#include "delay.h"
#include "drive.h"
#include "Voluntarily.h"
#include "rc522.h"
#include "cba.h"
#include "stdio.h"

uint8_t Card_Speed = 36;									//ǰ���ٶ�ֵ
uint8_t RXRFID[16];											//RFID��⵽������
uint8_t RFID_RH8[8];										//�������ݸ�8λ
uint8_t RFID_RL8[8];										//�������ݵ�8λ
uint8_t TXRFID[16] = {"ASDASDASAADASAAA"};					//д������
uint8_t Read_Car_Address = 0;								//�����Ŀ��ַ
uint8_t KEY_Open[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //��Կ
uint8_t s = 0x01;											//RFIDд������λ
uint8_t read_card_quantity;									//RFID������
uint8_t RFID_ONE_detection = 0;								//��RFID��������

/*��Կ����0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x80,0x69,0xb0,0x01,0xb2,0xb3,0xb4,0xb5;//д������*/

uint16_t terrain;		//���μ�����
uint16_t MP_A;			//ǰ����ֵ
uint16_t MP_B;			//������ֵ
uint16_t detection;		//���μ��״̬λ
uint16_t MP_difference; //���̲�ֵ

/**********************************************************************
 * �� �� �� ��  ���μ�⣨·��-����-������				 ·��
 * ��    �� ��  ����·����								|
 * �� �� ֵ ��  ��λ��                                  |
 * ȫ�ֱ��� ��  ��                                 |���� ���� ����|
 * ��    ע ��  ��                                       ^  ^	
 * �޸ļ�¼ ��  V1                                       �� ��
*****************************************************************/
uint8_t Card_One(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8��֮һ�ξ���
	uint8_t number;							// ������λ
	terrain = 1;							// �����-���μ�� ����
	MP_A = CanHost_Mp;						// ��ȡ��ǰ����ֵ
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // ����ѭ��
		Full_STOP();		   // ͣ����
		delay_ms(200);
		MP_B = CanHost_Mp;						  // ��ȡ��ǰ����ֵ
		MP_difference = MP_B - MP_A;			  // ���̲�ֵ
												  // 						Send_Debug(MP_difference);						 // �ϴ�������Ϣ����Ļ��
		if (MP_difference < (short_distance * 3)) // ����
		{
			delay_ms(100);
			SYN_Play("[s10]·��һ[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //��
		{
			delay_ms(100);
			SYN_Play("[s10]��1[d]");
			RFID_detection_motion(); // �Զ�����
			Car_Go(Card_Speed, 250); // ����ǰ��
			number = 0x01;			 // ��1����ȡ��־λ
		}
		else if ((short_distance * 7) < MP_difference) //��2 �� ����
		{
			if (crossroad == 1) //��2
			{
				delay_ms(100);
				SYN_Play("[s10]��2[d]");
				RFID_detection_motion(); // �Զ�����
				terrain = 0;			 // RFID���-���μ�� �ر�
				return 0x02;			 // ��2����ȡ��־λ
			}
			else //����
			{
				break; // �˳�������״̬
			}
		}
	}
	crossroad = 0; // ʮ��·�ڼ�⵽�׿�״̬λ
	detection = 0; // ���μ��״̬λ
	terrain = 0;   // RFID���-���μ�� �ر�
	return number;
}

/**********************************************************************
 * �� �� �� �� 	Ѱ�յ�Ŀ�
 * ��    �� ��  ��										|---|
 * �� �� ֵ ��  ��                                          ^
 * ȫ�ֱ��� ��  ��                                          ��
 * ��    ע ��  û�п�����������ѭ��ֹͣ
*****************************************************************/
uint8_t Card_Two(void)
{
	terrain = 1;
	crossroad = 0;
	Car_Track(Card_Speed); // ����ѭ��
	Full_STOP();		   // ͣ����
	delay_ms(200);
	if (crossroad == 1)
	{
		RFID_detection_motion(); //RFID΢�����
		crossroad = 0;
		terrain = 0; // RFID���-���μ�� �ر�
		return 1;
	}
	terrain = 0;
	return 0;
}

/**********************************************************************
 * �� �� �� ��  ˫�μ�⣨·��-����-·��-����-·��-������		·�� ·�� ·�� 
 * ��    �� ��  ����·����                                     |   |    |
 * �� �� ֵ ��  ��λ��                                         |   |    |
 * ȫ�ֱ��� ��  ��                                         |���� ���� ����|���� ���� ����|
 * ��    ע ��  ��                                              ^   ^   ^   ^
 * �޸ļ�¼ ��  V1										        ��  ��   ��  ��
*****************************************************************/
uint8_t Card_Three(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8��֮һ�ξ���
	uint8_t number;							// ������λ
	terrain = 1;							// �����-���μ�� ����
	MP_A = CanHost_Mp;						// ��ȡ��ǰ����ֵ
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // ����ѭ��
		Full_STOP();		   // ͣ����
		delay_ms(200);
		MP_B = CanHost_Mp;						  // ��ȡ��ǰ����ֵ
		MP_difference = MP_B - MP_A;			  // ���̲�ֵ
												  // 						Send_Debug(MP_difference);						 // �ϴ�������Ϣ����Ļ��
		if (MP_difference < (short_distance * 3)) // ·��һ
		{
			delay_ms(100);
			SYN_Play("[s10]·��һ[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //����
		{
			delay_ms(100);
			SYN_Play("��һ");
			RFID_detection_motion(); // �Զ�����
			Car_Go(Card_Speed, 250); // ����ǰ��
			number = 0x01;			 // ��1����ȡ��־λ
		}
		else if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //·�϶�
		{
			delay_ms(100);
			SYN_Play("[s10]·�϶�[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 9)) //��2 �� ����
		{
			if (crossroad == 1) //��2
			{
				delay_ms(100);
				SYN_Play("[s10]����[d]");
				RFID_detection_motion(); // �Զ�����
				Car_Go(Card_Speed, 250); // ����ǰ��
				number = 0x02;			 // ��2����ȡ��־λ
			}
		}
		else if ((short_distance * 9) < MP_difference && MP_difference < (short_distance * 11)) //·����
		{
			delay_ms(100);
			SYN_Play("[s10]·����[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 11) < MP_difference && MP_difference < (short_distance * 13)) //����
		{
			delay_ms(100);
			SYN_Play("[s10]����[d]");
			RFID_detection_motion(); // �Զ�����
			Car_Go(Card_Speed, 250); // ����ǰ��
			number = 0x03;			 // ��1����ȡ��־λ
		}
		else if ((short_distance * 15) < MP_difference) //��3 �� ����
		{
			if (crossroad == 1) //��3
			{
				delay_ms(100);
				SYN_Play("[s10]����[d]");
				RFID_detection_motion(); // �Զ�����
				terrain = 0;			 // RFID���-���μ�� �ر�
				return 0x04;			 // ��2����ȡ��־λ
			}
			else //����
			{
				break; // �˳�������״̬
			}
		}
	}
	crossroad = 0; // ʮ��·�ڼ�⵽�׿�״̬λ
	detection = 0; // ���μ��״̬λ
	terrain = 0;   // RFID���-���μ�� �ر�
	return number;
}

/**********************************************************************
 * �� �� �� ��  ˫����·�ϼ�⣨·�ϣ�													·�� 
 * ��    �� ��  ����·����                                                               |    
 * �� �� ֵ ��  ��λ��                                                                   |    
 * ȫ�ֱ��� ��  ��                                                              |���� ���� ����|���� ���� ����|
 * ��    ע ��  ��		
 * �޸ļ�¼ ��  V1	
*****************************************************************/
uint8_t Card_Four(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8��֮һ�ξ���
	uint8_t number;							// ������λ
	terrain = 1;							// �����-���μ�� ����
	MP_A = CanHost_Mp;						// ��ȡ��ǰ����ֵ
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // ����ѭ��
		Full_STOP();		   // ͣ����
		delay_ms(200);
		MP_B = CanHost_Mp;																  // ��ȡ��ǰ����ֵ
		MP_difference = MP_B - MP_A;													  // ���̲�ֵ
																						  // 						Send_Debug(MP_difference);						 // �ϴ�������Ϣ����Ļ��
		if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //·�϶�
		{
			delay_ms(100);
			SYN_Play("[s10]·��[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 15) < MP_difference)
		{
			break; // �˳�������״̬
		}
	}
	crossroad = 0; // ʮ��·�ڼ�⵽�׿�״̬λ
	detection = 0; // ���μ��״̬λ
	terrain = 0;   // RFID���-���μ�� �ر�
	return number;
}

/**********************************************************************
 * �� �� �� ��  ���μ��ͣ��·��-������				 ·��
 * ��    �� ��  ����·����                           |
 * �� �� ֵ ��  ��λ��                               |
 * ȫ�ֱ��� ��  ��                              |���� ���� ����|
 * ��    ע ��  ��                                    ^   ^	
 * �޸ļ�¼ ��  V1                                    ��   ��
*****************************************************************/
uint8_t Card_Five(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8��֮һ�ξ���
	uint8_t number;							// ������λ
	terrain = 1;							// �����-���μ�� ����
	MP_A = CanHost_Mp;						// ��ȡ��ǰ����ֵ
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // ����ѭ��
		Full_STOP();		   // ͣ����
		delay_ms(200);
		MP_B = CanHost_Mp;						  // ��ȡ��ǰ����ֵ
		MP_difference = MP_B - MP_A;			  // ���̲�ֵ
												  // 						Send_Debug(MP_difference);						 // �ϴ�������Ϣ����Ļ��
		if (MP_difference < (short_distance * 3)) // ����
		{
			delay_ms(100);
			SYN_Play("[s10]·��[d]");
			Detected = 1; // �����Ƿ��⵽
			terrain = 0;  // RFID���-���μ�� �ر�
			return 0x00;
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //��
		{
			delay_ms(100);
			SYN_Play("[s10]��[d]");
			terrain = 0; // RFID���-���μ�� �ر�
			return 0x01; // ��1����ȡ��־λ
		}
		else if ((short_distance * 7) < MP_difference) //��2 �� ����
		{
			break; // �˳�������״̬
		}
	}
	crossroad = 0; // ʮ��·�ڼ�⵽�׿�״̬λ
	detection = 0; // ���μ��״̬λ
	terrain = 0;   // RFID���-���μ�� �ر�
	return number;
}

/**********************************************************************
 * �� �� �� ��  ˫�μ��·�ϲ����յ㿨��·��-������		    ·�� 
 * ��    �� ��  ����·����                                 |    
 * �� �� ֵ ��  ��λ��                                     |    
 * ȫ�ֱ��� ��  ��                                |���� ���� ����|���� ���� ����|
 * ��    ע ��  ��                                                  ^
 * �޸ļ�¼ ��  V1                                                  �����յ㿨ͣ�²�����
*****************************************************************/
uint8_t Card_SIX(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8��֮һ�ξ���
											//					uint8_t number;													 // ������λ
	terrain = 1;							// �����-���μ�� ����
	MP_A = CanHost_Mp;						// ��ȡ��ǰ����ֵ
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // ����ѭ��
		Full_STOP();		   // ͣ����
		delay_ms(200);
		MP_B = CanHost_Mp;																  // ��ȡ��ǰ����ֵ
		MP_difference = MP_B - MP_A;													  // ���̲�ֵ
																						  // 						Send_Debug(MP_difference);						 // �ϴ�������Ϣ����Ļ��
		if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //·�϶�
		{
			delay_ms(100);
			SYN_Play("[s10]·��[d]");
			Car_Go(Card_Speed, 1300); // ����ǰ��
			Detected = 1;			  // �����Ƿ��⵽
		}
		else if ((short_distance * 15) < MP_difference) //��3 �� ����
		{
			if (crossroad == 1) //��3
			{
				delay_ms(100);
				SYN_Play("[s10]������[d]");
				terrain = 0; // RFID���-���μ�� �ر�
				return 0x01; // ��2����ȡ��־λ
			}
			else //����
			{
				break; // �˳�������״̬
			}
		}
	}
	crossroad = 0; // ʮ��·�ڼ�⵽�׿�״̬λ
	detection = 0; // ���μ��״̬λ
	terrain = 0;   // RFID���-���μ�� �ر�
	return 0;
}

/**********************************************************************
 * �� �� �� ��  ��������·����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��ӡ����·���� 
*****************************************************************/
void Obtain_section_distance(void)
{
	uint16_t Parameter_A = 0, Parameter_B = 0;
	Car_Track(Card_Speed);				   //����ѭ��
	Parameter_A = CanHost_Mp;			   //��ȡ��ǰ����ֵ
	Car_Track(Card_Speed);				   //����ѭ��
	Parameter_B = CanHost_Mp;			   //��ȡ��ǰ����ֵ
	Send_Debug(Parameter_B - Parameter_A); //�ϴ�������Ϣ����Ļ��
}

/**********************************************************************
 * �� �� �� ��  RFID΢�����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void RFID_detection_motion(void)
{
	read_card_quantity = 0;
	while (Auto_Read_card(Read_Car_Address) == 0)
	{
		Car_Go(Card_Speed - 10, 50); //����ǰ��
		delay_ms(300);
		RFID_ONE_detection += 1;
		if (RFID_ONE_detection > 8)
		{
			//SYN_Play("[s10]����ʧ��[d]");
			Car_Back(30, RFID_ONE_detection * 50); //��������
			RFID_ONE_detection = 0;
			read_card_quantity += 1;
			if (read_card_quantity >= 3)
			{
				break;
			}
		}
	}
	delay_ms(200);
	Car_Back(30, RFID_ONE_detection * 50); //��������
	RFID_ONE_detection = 0;
}

/**********************************************************************
 * �� �� �� ��  ����
 * ��    �� ��  ��
 * �� �� ֵ ��  ����״̬
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ���ַ���㣨����*4+�飿��
*****************************************************************/
uint8_t RFID_Card_One[16] = {0};   //�������ݻ����� 1
uint8_t RFID_Card_Two[16] = {0};   //�������ݻ����� 2
uint8_t RFID_Card_Three[16] = {0}; //�������ݻ����� 3
uint8_t Read_Flag[3] = {0};		   //������־λ
uint8_t Card_Fifo[8] = {0};		   //������Ϣ����
uint8_t Auto_Read_card(uint8_t Block_address)
{

	if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
	{
		if (PcdAnticoll(SN) == MI_OK) // ����ײ�ɹ�
		{
			if (PcdSelect(SN) == MI_OK) //ѡ���˿�
			{
				if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY_Open, SN) == MI_OK) //��֤��Կ
				{
					if (PcdRead(Block_address, RXRFID) == MI_OK) //����
					{
						if (Read_Flag[0] == 0) //��1
						{
							//sprintf((char*)Card_Fifo,"Sq:%d ",Read_Car_Address);//��ӡ�������ݿ��ַ
							Send_Debug_Info(Card_Fifo, 8);
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_One[i] = RXRFID[i];
							}
							Read_Flag[0] = 1;
							Send_Debug_string("Card 1\n");
							//Send_Debug_Info((uint8_t*)"Card 1\n",8);
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_One[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_One[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
							//Send_Debug_Info((uint8_t*)"\n",1);
						}
						else if (Read_Flag[1] == 0) //��2
						{
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_Two[i] = RXRFID[i];
							}
							Read_Flag[1] = 1;
							Send_Debug_string("Card 2\n");
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_Two[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_Two[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
						}
						else if (Read_Flag[2] == 0) //��3
						{
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_Three[i] = RXRFID[i];
							}
							Read_Flag[2] = 1;
							Send_Debug_string("Card 3\n");
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_Three[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_Three[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
						}
						/*��ʾ��*/
						MP_SPK = 1; //��������
						delay_ms(200);
						MP_SPK = 0; //��������
						SYN_Play("[s10]�����ɹ�[d]");
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

/**********************************************************************
 * �� �� �� ��  д��
 * ��    �� ��  ���ַ(0~63)
 * �� �� ֵ ��  д��״̬
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ���ַ���㣨����*4+�飿��
*****************************************************************/
uint8_t Auto_write_card(uint8_t Block_address)
{

	if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
	{
		if (PcdAnticoll(SN) == MI_OK) // ����ײ�ɹ�
		{
			if (PcdSelect(SN) == MI_OK) //ѡ���˿�
			{
				if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY_Open, SN) == MI_OK) //��֤����0����ԿA
				{
					if (PcdWrite(Block_address, TXRFID) == MI_OK) //���� ���ַ1
					{
						SYN_Play("[s10]д���ɹ�[d]");
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
