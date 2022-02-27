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

#include "Voluntarily.h"
#include "rng.h"
#include "Card.h"
/***************************��---��---��***********************************/
//������־λ
static uint8_t Go_Speed = 36; // ȫ��ǰ���ٶ�ֵ
//static uint8_t wheel_Speed = 90;            // ȫ��ת���ٶ�ֵ
//static uint16_t Go_Temp = 420;              // ǰ������ֵ
//TIM3��־λ
uint16_t TIM3_Flag = 0; //��ʱ��3��־λ
//RFID���͵��μ��
//uint8_t Testing_number=1;//RFID����μ�����
//uint8_t tracking_card; //ѭ�������ݣ��׿���
//RFID����
//��������
uint8_t Fifo_B[8];			// ������Ϣ����B
uint8_t ETC_Flag;			// ETC����־λ
uint8_t LED_OUT = 0;		// LED��˸λ
uint32_t Power_check_times; // �����������
uint32_t LED_twinkle_times; // LED��˸����
/**************************************************************************/

/**********************************************************************
 * �� �� �� ��  ������������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ���Կ��� 
*****************************************************************/
void SYN_Play(char *Pst)
{
#if Voice == 0
	SYN7318_Play(Pst);
#endif
#if Voice == 1
	delay_ms(100);
	MP_SPK = 1; //��������
	delay_ms(100);
	MP_SPK = 0; //��������
	delay_ms(100);
	MP_SPK = 1; //��������
	delay_ms(100);
	MP_SPK = 0; //��������
#endif
}

/**********************************************************************
 * �� �� �� ��  ����
 * ��    �� ��  Ŀ�굵λ
 * �� �� ֵ ��  ��ʼ��λ
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �� 
*****************************************************************/
uint8_t Dimming(uint8_t target)
{
	uint8_t MOV_Fifo;
	for (int CT = 0; CT < 4; CT++) // ���û�м�⵽�����¼��3��
	{
		delay_ms(100);
		MOV_Fifo = Light_djustment(target); //����̽��
		delay_ms(50);
		if (MOV_Fifo > 0)
		{
			delay_ms(50);
			sprintf((char *)Fifo_B, "L:%dT:%d\n", MOV_Fifo, target);
			Send_Debug_Info(Fifo_B, 8);
			delay_ms(100);
			break;
		}
	}
	return MOV_Fifo;
}

/**********************************************************************
 * �� �� �� ��  ���
 * ��    �� ��  ��
 * �� �� ֵ ��  ����
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �� 
*****************************************************************/
uint16_t Stadiometry(void)
{
	uint16_t DIS_Fifo;
	for (int i2 = 0; i2 < 4; i2++)
	{
		DIS_Fifo = Full_Measure(); //׼ȷ����
								   //				DIS_Fifo=measure_display();//С����
		if (DIS_Fifo > 0)
		{
			SYN_Play("[s10]���ɹ�[d]");
			break;
		}
		else
		{
			SYN_Play("[s10]���ʧ��[d]");
		}
	}
	/*sprintf((char *)Fifo_B, "%dmm\n", DIS_Fifo);
	Send_Debug_Info(Fifo_B, 8);*/
	return DIS_Fifo;
	//			LED_Dis_Zigbee(DIS_Fifo);														//����LED��־����ʾ����
	//			LED_Dis_Zigbee(DIS_Fifo);														//����LED��־����ʾ����
}

/**********************************************************************
 * �� �� �� ��  ETC���
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �� 
*****************************************************************/
void ETC_detection(void)
{
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	for (uint8_t i = 0; i < 6; i++)
	{
		if (Zigbee_Rx_flag == 1)
		{
			if (Receive_ETC_detection() == Receive_succeed) //ETC���ռ��
			{
				SYN_Play("ETC���ɹ�");
				Zigbee_Rx_flag = 0;
				break;
			}
		}
		if (measure_display() > 200)
		{
			break;
		}
		Car_Back(Go_Speed, 200); //��������
		delay_ms(500);
		Car_Go(Go_Speed, 200); // ����ǰ��
		delay_ms(500);
		if (i == 5)
		{
			SYN_Play("ETC���ʧ��");
		}
	}
}

/**********************************************************************
 * �� �� �� ��  ����������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ������ͣ���˲ŵ���������� ������������������������
*****************************************************************/
void Garage(uint8_t Tier)
{
	switch (Tier)
	{
	case 1:
	{
		break;
	}
	case 2:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 3:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To3, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To3, 8);
		for (int i = 0; i < 15; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 4:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To4, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To4, 8);
		for (int i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	default:
	{
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To2, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	}
}

/**********************************************************************
 * �� �� �� ��  ���wifi/zigbee������ɱ�־λ
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Eliminate_Flag(void)
{
	Wifi_Rx_flag = 0;	//wifi�������ݱ�־λ���
	Zigbee_Rx_flag = 0; //zigbee�������ݱ�־λ���
}

/**********************************************************************
 * �� �� �� ��  ΢��λ��
 * ��    �� ��  RL������
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Full_RL_little(uint8_t RL, uint8_t JY)
{
	Roadway_nav_syn();	  //�Ƕ�ͬ��
	Roadway_mp_syn();	  //����ͬ��
	Roadway_Flag_clean(); //�����־λ
	wheel_Nav_Flag = 1;
	if (RL == 1)
		temp_Nav = JY; //����ֵ
	else
		temp_Nav = JY; //
	Car_Spend = 80;
	if (RL == 1)
	{
		Send_UpMotor(-Car_Spend, Car_Spend); //��ת
	}
	else
	{
		Send_UpMotor(Car_Spend, -Car_Spend); //��ת
	}
	while (Stop_Flag != 2)
		;
}

/**********************************************************************
 * �� �� �� ��  ת���΢��
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Full_adjust(void)
{
	uint8_t zw;
	zw = Get_Host_UpTrack(TRACK_H8);
	if (zw == 0XFB || zw == 0XF9 || zw == 0XFD || zw == 0XFC || zw == 0XFE)
	{
		Full_RL_little(2, 90);
	}
	else if (zw == 0X9F || zw == 0XDF || zw == 0X3F || zw == 0XBF || zw == 0X7F)
	{
		Full_RL_little(1, 90);
	}
	delay_ms(100);
}

/****************************************************************
 * �� �� �� ��ȫ�Զ�����ѭ��
 * ��    �� ��sp���ٶ�  mp������
 * �� �� ֵ ��	��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void MP_Tracking(uint8_t sp, u16 mp)
{
	u16 MP = 0;
	Roadway_mp_syn(); //����ͬ��
	Roadway_Flag_clean();
	Track_Flag = 1;
	temp_MP = mp;
	Car_Spend = sp;
	while ((MP < mp) && (Stop_Flag != 0x01))
	{
		MP = Roadway_mp_Get();
	}
	Full_STOP(); // ֹͣ
	delay_ms(100);
}

/****************************************************************
 * �� �� �� ��  ֹͣ
 * ��    �� ��	��
 * �� �� ֵ ��	��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Full_STOP(void)
{
	Roadway_Flag_clean(); //������б�־λ
	Send_UpMotor(0, 0);	  // ֹͣ
}
/**********************************************************************
 * �� �� �� ��  2021���°�ȫ�Զ�ѭ���Ƽ�������ת
 * ��    �� ��  sp �ٶ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Left_hand_bend(u8 sp)
{
	uint8_t T8;
	int16_t Mpa;
	Roadway_mp_syn(); // ����ͬ��
	Roadway_Flag_clean();
	Control(-sp, sp);
	Mpa = CanHost_Mp;
	delay_ms(200);
	while (1)
	{
		T8 = Get_Host_UpTrack(TRACK_H8);
		T8 &= 0x18;
		if (T8 == 0x00 || T8 == 0X10)
		{
			Send_UpMotor(0, 0);
			break;
		}
		if (Mpa <= 0 && CanHost_Mp > 0)
		{
			if ((Mpa - (-32768) + 32768 - CanHost_Mp) > 930)
			{
				/* ��ת-32768��32768�ٽ���ж� */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (Mpa - CanHost_Mp > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * �� �� �� ��  2021���°�ȫ�Զ�ѭ���Ƽ�������ת
 * ��    �� ��  sp �ٶ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Right_hand_bend(u8 sp)
{
	uint8_t T8;
	int16_t Mpa;
	Roadway_mp_syn(); // ����ͬ��
	Roadway_Flag_clean();
	Control(sp, -sp);
	Mpa = CanHost_Mp;
	delay_ms(300);
	while (1)
	{
		T8 = Get_Host_UpTrack(TRACK_H8);
		T8 &= 0x18;
		if (T8 == 0x00 || T8 == 0X08)
		{
			Send_UpMotor(0, 0);
			break;
		}
		if (Mpa >= 0 && CanHost_Mp < 0)
		{
			if ((32768 - Mpa) + (CanHost_Mp - (-32768)) > 930)
			{
				/* ��ת32768��-32768 �ٽ���ж� */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (CanHost_Mp - Mpa > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * �� �� �� ��  ������ת
 * ��    �� ��  sp �ٶ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Right_MP(u8 sp)
{
	int16_t Mpa;
	Roadway_mp_syn(); // ����ͬ��
	Roadway_Flag_clean();
	Control(sp, -sp);
	Mpa = CanHost_Mp;
	while (1)
	{
		if (Mpa >= 0 && CanHost_Mp < 0)
		{
			if ((32768 - Mpa) + (CanHost_Mp - (-32768)) > 930)
			{
				/* ��ת32768��-32768 �ٽ���ж� */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (CanHost_Mp - Mpa > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}
/**********************************************************************
 * �� �� �� ��  ������ת
 * ��    �� ��  sp �ٶ�
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Left_MP(u8 sp)
{
	int16_t Mpa;
	Roadway_mp_syn(); // ����ͬ��
	Roadway_Flag_clean();
	Control(-sp, sp);
	Mpa = CanHost_Mp;
	while (1)
	{
		if (Mpa <= 0 && CanHost_Mp > 0)
		{
			if ((Mpa - (-32768) + 32768 - CanHost_Mp) > 930)
			{
				/* ��ת-32768��32768�ٽ���ж� */
				Send_UpMotor(0, 0);
				break;
			}
		}
		else
		{
			if (Mpa - CanHost_Mp > 930)
			{
				Send_UpMotor(0, 0);
				break;
			}
		}
	}
	delay_ms(100);
}

void Full_Right(u8 sp)
{
	Right_hand_bend(sp);
	//	Right_MP(sp);
}
void Full_Left(u8 sp)
{
	Left_hand_bend(sp);
	//	Left_MP(sp);
}

/**********************************************************************
 * �� �� �� ��  Ӳ����ʼ������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); // �жϷ���
	delay_init(168);								// ��ʱ��ʼ��
	
	Task_board_Init();								// ������ʼ��
	Infrared_Init();								// �����ʼ��
	Cba_Init();										// ���İ��ʼ��
	Ultrasonic_Init();								// ��������ʼ��
	Hard_Can_Init();								// CAN���߳�ʼ��
	BH1750_Configure();								// BH1750��ʼ������
	Electricity_Init();								// ��������ʼ��
	UartA72_Init();									// A72Ӳ������ͨѶ��ʼ��
	Can_check_Init(7, 83);							// CAN���߶�ʱ����ʼ��
	roadway_check_TimInit(999, 167);				// ·�����
	Timer_Init(999, 167);							// ��������ͨѶʱ��֡
	Readcard_daivce_Init();							// RFID��ʼ��
	TIM3_Int_Init(10000 - 1, 8400 - 1);				// ��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s
	TIM4_Int_Init(100 - 1, 8400 - 1);				// ��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����100��Ϊ10ms
	//while(RNG_Init());	 																// ��ʼ�������������
	//SYN_Init();										// ����ʶ���ʼ��
	SYN7318_Init();
	Tim5_Init(1,8399);		//WIFI����ʶ�����ݳ�ʼ��
	
	LED_twinkle_times = gt_get() + 50;
	Power_check_times = gt_get() + 200;
}

/**********************************************************************
 * �� �� �� ��  �Զ�����
 * ��    �� ��  Sun_gear��Ŀ�굵λֵ
 * �� �� ֵ ��  ��ʼ��λֵ
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
uint8_t Light_djustment(uint8_t Sun_gear)
{
	uint8_t i = 4;
	uint8_t gears_Value = 0;
	u16 Light_Value[5];
	Get_Bh_Value();
	delay_ms(500);
	for (i = 0; i < 4; i++)
	{
		Digital_tube_Infrared_Send(Light_plus1, 4); //��Դ��λ��1
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Light_Value[i] = Get_Bh_Value(); //����
#if Test_Daat == 1
		Send_Debug(Light_Value[i]); //����
#endif
		delay_ms(500);
	}
	if (Light_Value[0] > Light_Value[1]) //�жϵ���ǰ��λΪ������
	{
		gears_Value = 3;
	}
	else if (Light_Value[1] > Light_Value[2]) //�жϵ���ǰ��λΪ�ڶ���
	{
		gears_Value = 2;
	}
	else if (Light_Value[2] > Light_Value[3]) //�жϵ���ǰ��λΪ��һ��
	{
		gears_Value = 1;
	}
	else //�жϵ���ǰ��λΪ���ĵ�
	{
		gears_Value = 4;
	}
	delay_ms(500);
	if (Sun_gear != 0)
	{
		switch (Sun_gear) //������Ƶ�λ
		{
		case 1: //��1��
		{
			switch (gears_Value) //״̬��ѯ
			{
			case 2: //��ǰ��λΪ2
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //��Դ��λ��1
				break;
			}
			case 3: //��ǰ��λΪ3
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //��Դ��λ��1
				break;
			}
			case 4: //��ǰ��λΪ4
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //��Դ��λ��1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 2: //��2��
		{
			switch (gears_Value) //״̬��ѯ
			{
			case 3: //��ǰ��λΪ3
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //��Դ��λ��1
				break;
			}
			case 4: //��ǰ��λΪ4
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //��Դ��λ��1
				break;
			}
			case 1: //��ǰ��λΪ1
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //��Դ��λ��1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 3: //��3��
		{
			switch (gears_Value) //״̬��ѯ
			{
			case 4: //��ǰ��λΪ4
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //��Դ��λ��1
				break;
			}
			case 1: //��ǰ��λΪ1
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //��Դ��λ��1
				break;
			}
			case 2: //��ǰ��λΪ2
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //��Դ��λ��1
				break;
			}
			default:
				break;
			}
			break;
		}
		case 4: //��4��
		{
			switch (gears_Value) //״̬��ѯ
			{
			case 1: //��ǰ��λΪ1
			{
				Digital_tube_Infrared_Send(Light_plus3, 4); //��Դ��λ��1
				break;
			}
			case 2: //��ǰ��λΪ2
			{
				Digital_tube_Infrared_Send(Light_plus2, 4); //��Դ��λ��1
				break;
			}
			case 3: //��ǰ��λΪ3
			{
				Digital_tube_Infrared_Send(Light_plus1, 4); //��Դ��λ��1
				break;
			}
			default:
				break;
			}
			break;
		}
		}
	}
	return gears_Value;
}

/****************************************************************
�������ܣ���ຯ��//Ƶ��31.6
��	  ����Full_Measure();				Measure()��
����  ֵ����
*****************************************************************/
uint16_t Full_Measure(void)
{
	int i, j, temp, disss;
	float brr[8] = {0};
	for (i = 0; i < 8; i++)
	{
		LED2 = !LED2;
		measure_display();
		delay_ms(100);
#if Test_Daat == 1
		Send_Debug(dis);
#endif
		brr[i] = dis;
	}
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 7 - i; j++)
		{
			if (brr[j] > brr[j + 1])
			{
				temp = brr[j];
				brr[j] = brr[j + 1];
				brr[j + 1] = temp;
			}
		}
	}
	//delay_ms(500);
	disss = (u16)(((brr[3] + brr[4]) / 2));
	delay_ms(200);
	return disss;
	//return measure_display();
}
/**********************************************************************
 * �� �� �� ��  ��������ࣨ��Σ�
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
uint16_t measure_display(void)
{
	int i, j, temp;
	float arr[8] = {0};

	for (i = 0; i < 8; i++)
	{
		Ultrasonic_Ranging(); //���������

		delay_ms(100);
		arr[i] = dis;
	}

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 7 - i; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	//dis=(u16)(((arr[5]+arr[6])/2)-22);			//���1:	-22
	dis = (u16)(((arr[5] + arr[6]) / 2) - 12); //���1:	-12

	//				TFT_Dis_Zigbee('A',dis);	  // TFT��ʾ������Ϣ
	//delay_ms(200);
	return dis;
}

/**********************************************************************
 * �� �� �� ��  ͨ�ö�ʱ��3�жϳ�ʼ��
 * ��    �� ��  arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us. Ft=��ʱ������Ƶ��,��λ:Mhz
*****************************************************************/
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ��TIM3ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr;						//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //��ʼ��TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //����ʱ��3�����ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;	  //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************
 * �� �� �� ��  ��ʱ��3�жϷ�����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
	{
		TIM3_Flag += 1;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ
}
/**********************************************************************
 * �� �� �� ��  ��������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Digital_tube_TEXT(void)
{
	/*����������*/
	Task_board_buzzer(SET); // ��λ�ɹ�ָʾ
	delay_ms(500);
	delay_ms(500);
	Task_board_buzzer(RESET);

	/*����ģ�鷢������������*/
	//SYN_Extern(); // ����ʶ��

	/*ת��ƹ��ܲ���*/
	Task_board_turn_light(R_LED, SET); // ���� ��
	delay_ms(500);
	Task_board_turn_light(R_LED, RESET); // ���� ��
	Task_board_turn_light(L_LED, SET);	 // ���� ��
	delay_ms(500);
	Task_board_turn_light(L_LED, RESET); // ���� ��
	Task_board_turn_light(R_LED, SET);	 // ���� ��
	delay_ms(500);
	Task_board_turn_light(R_LED, RESET); // ���� ��
	Task_board_turn_light(L_LED, SET);	 // ���� ��
	delay_ms(500);
	Task_board_turn_light(L_LED, RESET); // ���� ��

	/*����ܼ�74HC595����*/
	for (uint8_t i = 0; i < 10; i++)
	{
		Digital_Display(i, i); // �������ʾ
		delay_ms(200);
	}
	/*������ģ�����*/
	Stadiometry(); // ���

	/*�������*/
	Rotate_Dis_Inf(200); // ������ʾ��־����ʾ������Ϣ����λ��mm��
	Rotate_Dis_Inf(200); // ������ʾ��־����ʾ������Ϣ����λ��mm��

	/*BH1750���մ���������*/
	Send_Debug(Light_djustment(4)); // ����̽��
	SYN_Play("���");
}

/**********************************************************************
 * �� �� �� ��  �������ţ�����+���֣����� 
 * ��    �� ��  number�����ֲ���  unt�����ֲ�����
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ���ֲ��ִ�С��0~65535  ���֣�0~3����
*****************************************************************/
void voice(uint16_t number, char *unt)
{
	uint8_t Size_Flag;
	char Send_voice[12];
	//���ֲ��ּ���ϳ�
	Send_voice[0] = (number % 100000 / 10000) + 0x30;
	Send_voice[1] = (number % 10000 / 1000) + 0x30;
	Send_voice[2] = (number % 1000 / 100) + 0x30;
	Send_voice[3] = (number % 100 / 10) + 0x30;
	Send_voice[4] = (number % 10) + 0x30;
	//���ֲ��ּ���ϳ�
	Send_voice[5] = unt[0];
	Send_voice[6] = unt[1];
	Send_voice[7] = unt[2];
	Send_voice[8] = unt[3];
	Send_voice[9] = unt[4];
	Send_voice[10] = unt[5];
	Send_voice[11] = unt[6];
	for (int i = 0; i < 5; i++) //�ж�Ϊ0��0033->��2����
	{
		if (Send_voice[i] != 0x30)
		{
			Size_Flag = i;
			if (Send_voice[i] > '9')
			{
				Size_Flag = 0;
			}
			break;
		}
	}
	if (Size_Flag != 0) //��ǰ���� ��0011 -> 11��
	{
		for (int i2 = 0; i2 < Size_Flag; i2++)
		{
			for (int i3 = 0; i3 < 11; i3++)
			{
				Send_voice[i3] = Send_voice[i3 + 1];
			}
		}
	}
	SYN_Play(Send_voice); //�����ϳɲ��ź���
}

/**********************************************************************
 * �� �� �� ��  ����ָʾ�� 
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Running_indicator_light(void)
{
	if (gt_get_sub(LED_twinkle_times) == 0) // ����ָʾ��
	{
		LED_twinkle_times = gt_get() + 50; // LED4����״ָ̬ʾ��
		if (LED_OUT == 0)
		{
			LED4 = RESET;
			LED1 = SET;
			LED_OUT = 1;
		}
		else if (LED_OUT == 1)
		{
			LED1 = RESET;
			LED2 = SET;
			LED_OUT = 2;
		}
		else if (LED_OUT == 2)
		{
			LED2 = RESET;
			LED3 = SET;
			LED_OUT = 3;
		}
		else if (LED_OUT == 3)
		{
			LED3 = RESET;
			LED4 = SET;
			LED_OUT = 0;
		}
	}
}

/**********************************************************************
 * �� �� �� ��  ��ص������ 
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Battery_power_detection(void)
{
	if (gt_get_sub(Power_check_times) == 0) // ��ص������
	{
		Power_check_times = gt_get() + 200;
		Power_Check();
	}
}

uint8_t Garage_Tier = 0; // ������־λ
/**********************************************************************
 * �� �� �� ��  �ó���ﵽ��һ��
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  �Լ�����
*****************************************************************/
void Into_the_garage(void)
{
	uint8_t i = 0;
	Send_ZigbeeData_To_Fifo(GarageA_Get_Floor, 8); // ���峵��A ���󷵻س���λ�ڵڼ���
	Send_ZigbeeData_To_Fifo(GarageA_Get_Floor, 8); // ���峵��A ���󷵻س���λ�ڵڼ���
	delay_ms(100);
	while (1)
	{							 //55 0D 03 01 01 00 05 BB
								 //55 0D 03 01 02 00 05 BB
		if (Zigbee_Rx_flag == 1) // zigbee������Ϣ
		{
			if ((Zigb_Rx_Buf[0] == 0x55) && (Zigb_Rx_Buf[1] == 0X0D))
			{
				if ((Zigb_Rx_Buf[2] == 0X03) && (Zigb_Rx_Buf[7] == 0xBB) && (Zigb_Rx_Buf[3] == 0X01))
				{
					Garage_Tier = Zigb_Rx_Buf[4];
					break;
				}
			}
			Zigbee_Rx_flag = 0;
		}
		delay_ms(100);
		i += 1;
		if (i >= 30) //3S��û�н��յ�
		{
			Garage_Tier = 0;
			break;
		}
	}
	switch (Garage_Tier)
	{
	case 0:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (int i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 1:
	{
		Garage_Tier = 0;
		break;
	}
	case 2:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (int i = 0; i < 8; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 3:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (uint8_t i = 0; i < 15; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	case 4:
	{
		Garage_Tier = 0;
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		Send_ZigbeeData_To_Fifo(GarageA_To1, 8);
		for (uint8_t i = 0; i < 22; i++)
		{
			delay_ms(500);
			delay_ms(500);
		}
		break;
	}
	}
}

/**********************************************************************
 * �� �� �� ��  ���̨������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ���޸Ĳ��˾Ͱ��·��̨�����S4�������ĵ���ʼ������ڽ����޸��µ�����
*****************************************************************/
void Beacon_change_password(uint8_t PassWord[6])
{
	uint8_t password[] = {0x55, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};
	password[2] = 0x10;
	password[3] = PassWord[0];
	password[4] = PassWord[1];
	password[5] = PassWord[2];
	password[6] = (password[2] + password[3] + password[4] + password[5]) % 0xFF;
	Send_ZigbeeData_To_Fifo(password, 8);
	delay_ms(100);
	password[2] = 0x11;
	password[3] = PassWord[3];
	password[4] = PassWord[4];
	password[5] = PassWord[5];
	password[6] = (password[2] + password[3] + password[4] + password[5]) % 0xFF;
	Send_ZigbeeData_To_Fifo(password, 8);
}
/**********************************************************************
 * �� �� �� ��  �鿴��123����������
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Check_the_cache(void)
{
	uint8_t RFL[8], RFH[8];
	Send_Debug_Info((uint8_t *)"Card 1\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_One[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_One[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);

	Send_Debug_Info((uint8_t *)"Card 2\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_Two[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_Two[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);

	Send_Debug_Info((uint8_t *)"Card 3\n", 8);
	for (int i = 0; i < 8; i++)
	{
		RFL[i] = RFID_Card_Three[i];
	}
	for (int i = 0; i < 8; i++)
	{
		RFH[i] = RFID_Card_Three[i + 8];
	}
	Send_Debug_Info(RFL, 8);
	Send_Debug_Info(RFH, 8);
	Send_Debug_Info((uint8_t *)"\n", 1);
}

/**********************************************************************
 * �� �� �� ��  С������ʶ����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void Voice_control(void)
{
#if Voice == 1
	if (USART6_Flag)
	{
		if (USART6_Back[0] == 0x55 && USART6_Back[1] == 0x02)
		{
			switch (USART6_Back[2])
			{
			case 0x06:
			{
				Car_Go(Go_Speed, Go_Temp); // ����ǰ��
				break;
			}
			case 0x07:
			{
				Car_Back(Go_Speed, Go_Temp); // ��������
				break;
			}
			case 0x08:
			{
				Left_MP(wheel_Speed); // ������ת
				break;
			}
			case 0x09:
			{
				Right_MP(wheel_Speed); // ������ת
				break;
			}
			case 0x0A:
			{
				Car_Track(Go_Speed); // ����ѭ��
				break;
			}
			}
		}
		USART6_Flag = 0;
	}
#endif
}
