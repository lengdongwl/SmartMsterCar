#include "Voluntarily.h"
#include "function.h"
#include "drive.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "infrared.h"
#include "cba.h"
#include "ultrasonic.h"
#include "canp_hostcom.h"
#include "hard_can.h"
#include "bh1750.h"
#include "power_check.h"
#include "can_user.h"
#include "data_base.h"
#include "roadway_check.h"
#include "data_base.h"
#include "swopt_drv.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "delay.h"
#include "can_user.h"
#include "Timer.h"
#include "Rc522.h"
#include "string.h"
#include "Zigbee.h"
#include "NEW_Task_board.h"
#include "new_syn.h"
#include "rng.h"
#include "Card.h"


#include "OperationFlag.h"
//#include "CRC.h"

static uint8_t Go_Speed = 36;	 // ȫ��ǰ���ٶ�ֵ Go_Speed
static uint8_t wheel_Speed = 90; // ȫ��ת���ٶ�ֵ wheel_Speed
static uint16_t Go_Temp = 420;	 // ǰ������ֵ Go_Temp

uint8_t Overtime_Flag = 0; // ��ʱ��־λ
uint8_t timing_time = 2;   // ��ʱ����ʱʱ������
uint8_t Exit_position;	   // �˳����е�λ��
uint8_t Operation_Flag;	   // ��ʱ�ص���λ��
//RFID����
uint8_t RFID_Flag = 0;										   // RFID����־λ
uint8_t Terrain_Flag = 0;									   // ���μ���־λ
uint8_t make = 0;											   // ȫ�Զ���ʻ��־λ
uint8_t RFID_addr = 0;										   // RFID��Ч���ݿ��ַ
uint8_t Stereo_Base[6] = {0xff, 0x11, 0x00, 0x00, 0x00, 0x00}; //������ʾ���������

uint8_t Fifo[8];	  //������Ϣ����
uint8_t Detected = 0; //�����Ƿ��⵽

uint8_t Car_location;		   //����λ��
uint8_t Car_location_ASCll[2]; //����λ��ASCll

uint16_t Distance_Data = 0; //����

uint8_t From_Car_Position[3] = {0}; //��������λ�����ݻ���
//��Ϣ����
//uint16_t MOV1;				  //��Ϣ����1(  )
//uint16_t MOV2;				  //��Ϣ����2(  )
//uint16_t MOV3;				  //��Ϣ����3(  )
//uint16_t MOV4;				  //��Ϣ����4(  )
//uint16_t MOV5;				  //��Ϣ����5(  )
//uint16_t MOV6;				  //��Ϣ����6(  )
//uint16_t MOV7;				  //��Ϣ����7(  )
//uint16_t MOV8;				  //��Ϣ����8(  )
//uint16_t MOV9;				  //��Ϣ����9(  )
uint8_t Card_Temp = 0;

uint16_t Tempp = 0, Temppp = 0;

uint8_t quantity(char *data_temp)
{
	uint8_t temp = 0;
	for (int i = 0; i < 16; i++)
	{
		if (data_temp[i] == 'A')
		{
			temp += 1;
		}
	}
	return temp;
}

/*************************************************************************
 * �� �� �� ��  ���ַ�����������С���֣�ASCLL������µ�����
 * ��    �� ��  chepai����������
 * �� �� ֵ ��  ����
 * ��    ע ��  1ABC5D ->	15��ʮ���ƣ�
*************************************************************************/
uint8_t extract(char *chepai)
{
	char temp;
	uint8_t TP[6];
	uint8_t Big = 0;
	for (int i = 0; i < 6; i++)
	{
		TP[i] = chepai[i];
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[i] < '0' || TP[i] > '9')
		{
			TP[i] = 0xFF;
		}
		else
		{
			TP[i] -= '0';
		}
	}
	for (int i = 0; i < 6; i++)
	{
		for (int ii = 0; ii < 5; ii++)
		{
			if (TP[ii] > TP[ii + 1])
			{
				temp = TP[ii];
				TP[ii] = TP[ii + 1];
				TP[ii + 1] = temp;
			}
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[i] != 0xff)
		{
			Big = TP[i] * 10;
			break;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (TP[5 - i] != 0xff)
		{
			Big += TP[5 - i];
			break;
		}
	}
	return Big;
}

/**********************************************************************
 * �� �� �� ��  ��ʱɨ��
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Interrupt_flag_bit_query(void)
{

	/*�������ʾ����ʱ*/
	//Digital_Display((timing_time-TIM3_Flag)/10,(timing_time-TIM3_Flag)%10);

	if (TIM3_Flag >= timing_time)
	{
		TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
		SYN_Play("��ʱ");
		make_Yes = Operation_Flag; // ��ʱ��ص��ĵط�
		TIM3_Flag = 0;			   // �����ʱ����־λ
		Overtime_Flag = 1;		   // ��ʱ��־λ
	}
}

/**********************************************************************
 * �� �� �� ��  ������ʱ���
 * ��    �� ��  Time����ʱʱ�� position:�˳����е�λ�� Return_run����ʱ�ص���λ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Timeout_detection(uint16_t Time, uint8_t position, uint8_t Return_run)
{
	for (int i = 0; i < 8; i++) //�������
	{
		Wifi_Rx_Buf[i] = 0;
	}
	Wifi_Rx_flag = 0;			 // ���Wifi���ձ�־λ+
	Zigbee_Rx_flag = 0;			 // ���Zigbee���ձ�־λ
	TIM3_Flag = 0;				 // �������
	Overtime_Flag = 0;			 // �����ʱ��־λ
	timing_time = Time;			 // ��ʱʱ������
	make_Yes = position;		 // �˳����е�λ������
	Exit_position = position;	 // �˳����е�λ������
	Operation_Flag = Return_run; // ��ʱ��ص���λ������
	TIM_Cmd(TIM3, ENABLE);		 // ʹ�ܶ�ʱ��3��������ʱ��⣩
	/*�ϴ�������Ϣ*/
	Send_Debug_Info((uint8_t *)"OpenTIM:", 8);
	sprintf((char *)Fifo, "%d\n", Time);
	Send_Debug_Info(Fifo, 3);
	delay_ms(100);
}

/**********************************************************************
 * �� �� �� ��  Zigbee+WIFI���ո������ݴ���
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  1.Zigbee_Rx_flag 2.Wifi_Rx_flag ����can_user
 * ��    ע ��  Zigbee_Rx_flag(Zigbee���ձ�־) Wifi_Rx_flag (Wifi���ձ�־)
*****************************************************************/
void Zigbee_Wifi_receive(void)
{
	/*С������*/
	//Voice_control();
	/*WIFI*/
	if (Wifi_Rx_flag == 1)
	{
		if (Receive_Traffic_light_identification() == Receive_succeed) // ��ͨ��ʶ��
		{
			TIM_Cmd(TIM3, DISABLE);		   // ʧ�ܶ�ʱ��3
			if (make_Yes == Exit_position) // �ж��Ƿ�����ȫ�Զ�
			{
				make_Yes = Operation_Flag; // ʶ��ɹ��ص���λ��
			}
		}
		if (transmit_wifi() == 1)
		{
		} // wifiת����zigbee
		else
		{
			Receive_Operation();								// ƽ�����
																//			Receive_Static_marker_1_A();
			if (Receive_Static_marker_1_A() == Receive_succeed) // ��̬��־��1����ά�룩
			{
				TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
				SYN_Play("[s10]��ά��ʶ��ɹ�[d]");
				if (make_Yes == Exit_position) // �ж��Ƿ�����ȫ�Զ�
				{
					make_Yes = Operation_Flag; // ʶ��ɹ��ص���λ��
				}
				/*�ϴ�������Ϣ*/
				Send_Debug_Info((uint8_t *)"QR Code\n", 8);
				Send_Debug_HEX(Static_marker_1[0]);
				Send_Debug_HEX(Static_marker_1[1]);
				Send_Debug_HEX(Static_marker_1[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				/*���ݿ�*/
				Read_Car_Address = (Static_marker_1[0] + 1) * 4 + (Static_marker_1[1] + 1); // X*4+N(ʮ����)

				//								Send_Debug_HEX(Static_marker_1[3]);
				//								Send_Debug_HEX(Static_marker_1[4]);
				//								Send_Debug_HEX(Static_marker_1[5]);
				//								Send_Debug_Info((uint8_t*)"\n",2);
				//								for(uint8_t i=0;i<6;i++)
				//								{
				//									if(Static_marker_1[i]==0x00)
				//									{
				//											SSIZE=i;
				//											break;
				//									}
				//								}
				//								sprintf((char*)Fifo,"%X\n",CRC24_BLE((char*)Static_marker_1,SSIZE));
				//								Send_Debug_Info(Fifo,8);
			}
			else if (Receive_flat_computer_TFT() == Receive_succeed) // ����ʶ��
			{
				TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
				SYN_Play("[s10]����ʶ��ɹ�[d]");
				if (make_Yes == Exit_position) // �ж��Ƿ�����ȫ�Զ�
				{
					make_Yes = Operation_Flag; // ʶ��ɹ��ص���λ��
				}
				//								TFT_Show_Zigbee('B',(char*)(TFT_license_plate));	// TFT��ʾ����
				//								TFT_Show_Zigbee('B',(char*)(TFT_license_plate));	// TFT��ʾ����
				TFT_Show_Zigbee('A', (char *)TFT_license_plate);
				TFT_Show_Zigbee('A', (char *)TFT_license_plate);

				/*�ϴ�������Ϣ*/
				Send_Debug_Info((uint8_t *)"P:", 2);
				Send_Debug_Info(TFT_license_plate, 8);
				Send_Debug_Info((uint8_t *)"\n", 2);
				//
				//								for (uint8_t i = 0; i < 6; i++)
				//								{
				//									if (TFT_license_plate[i] >= '0' && TFT_license_plate[i] <= '9')
				//									{
				//										Oupt += TFT_license_plate[i] - '0';
				//									}
				//								}
			}
			else if (Receive_flat_computer_graph() == Receive_succeed) // ͼ��ʶ��
			{
				TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
				SYN_Play("[s10]ͼ��ʶ��ɹ�[d]");
				/*��Graph[0]������ ����Graph[1]��Բ�� ����Graph[2]�����ǣ�
												��Graph[3]: ���Σ���Graph[4]������ǣ�*/
				TFT_Test_Zigbee('B', 0x40, 0xA0 + Graph[0], 0xB0 + Graph[4], 0xC0 + Graph[1]); //TFTA��ʾ
				TFT_Test_Zigbee('B', 0x40, 0xA0 + Graph[0], 0xB0 + Graph[4], 0xC0 + Graph[1]); //TFTA��ʾ

				if (make_Yes == Exit_position) // �ж��Ƿ�����ȫ�Զ�
				{
					make_Yes = Operation_Flag;
				}
			}
			else if (Receive_flat_computer_first_start() == Receive_succeed) // �������ƣ�ƽ��������
			{
				make_Yes = 0x01;
			}
		}
		Wifi_Rx_flag = 0;
	}

	/*Zigbee*/
	if (Zigbee_Rx_flag == 1) // zigbee������Ϣ
	{
		if (transmit() == 1)
		{
		} // zigbeeת����wifi
		else
		{
			Receive_barrier_gate(); // ���տ�����բָ��

			if (Receive_QR_Code_B() == Receive_succeed) // ���ո�����ά������B
			{
				TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
				SYN_Play("[s10]�յ�С����ά��[d]");
				Send_Debug_Info((uint8_t *)"QR Code\n", 8);
				Send_Debug_HEX(Vice_car_QR[0]);
				Send_Debug_HEX(Vice_car_QR[1]);
				Send_Debug_HEX(Vice_car_QR[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				Send_Debug_HEX(Vice_car_QR[3]);
				Send_Debug_HEX(Vice_car_QR[4]);
				Send_Debug_HEX(Vice_car_QR[5]);
				Send_Debug_Info((uint8_t *)"\n", 2);
			}
			else if (Receive_start_instruction() == Receive_succeed) // ���ո�����������ָ��
			{
				TIM_Cmd(TIM3, DISABLE); // ʧ�ܶ�ʱ��3
				From_Car_Position[0] = Zigb_Rx_Buf[3];
				From_Car_Position[1] = Zigb_Rx_Buf[4];
				From_Car_Position[2] = Zigb_Rx_Buf[5];
				Send_Debug_Info((uint8_t *)"Car\n", 8);
				Send_Debug_HEX(From_Car_Position[0]);
				Send_Debug_HEX(From_Car_Position[1]);
				Send_Debug_HEX(From_Car_Position[2]);
				Send_Debug_Info((uint8_t *)"\n", 2);
				SYN_Play("[s10]�յ�С�������[d]");
				if (make_Yes == Exit_position) // ��ʱ�ص��ĵط�
				{
					make_Yes = Operation_Flag;
				}
			}

			//Send_Debug_num(OFlag_DZ_status(Zigb_Rx_Buf));
			//Send_Debug_string(Zigb_Rx_Buf);
			//Send_Debug(OFlag_CK_status(Zigb_Rx_Buf,1,3));
			//Send_Debug_num(OFlag_check(Zigb_Rx_Buf));
		}
		Zigbee_Rx_flag = 0; // ���Zigbee���ձ�־λ
	}
}

/**********************************************************************
 * �� �� �� ��  �����߳����к��� 
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  make_Yes //ȫ�Զ���־λ ��function.h�ض���
 * ��    ע ��  �� 
*****************************************************************/
void Car_Thread(void)
{

	switch (make_Yes)
	{
	case 0x01:
		Car_Go(Go_Speed, Go_Temp);
		break;
	case 0x02:
		Car_Back(Go_Speed, Go_Temp);
		break;
	case 0x03:
		Car_Back(Go_Speed, Go_Temp);
		break;
	case 0x04:
		Car_Back(Go_Speed, Go_Temp);
		break;
	}
}
