#include "delay.h"
#include "cba.h"
#include "canp_hostcom.h"
#include "power_check.h"
#include "function.h"
#include "Voluntarily.h"
#include "MasterCarCotrol.H"
#include "Timer.h"
#include "CanP_HostCom.h"
#include "Hard_Can.h"
#include "syn7318.h"
#include "bkrc_voice.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "Timer.h"
#include "rc522.h"
#include "TaskBoard.h"
void keyScan(void)
{
	if (S1 == 0)
	{
		delay_ms(20);
		if (S1 == 0)
		{
			LED1 = SET;
			while (!S1)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x01);
		}
	}
	if (S2 == 0)
	{
		delay_ms(20);
		if (S2 == 0)
		{
			LED2 = SET;
			while (!S2)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x02);
		}
	}
	if (S3 == 0)
	{
		delay_ms(20);
		if (S3 == 0)
		{
			LED3 = SET;
			while (!S3)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x03);
		}
	}
	if (S4 == 0)
	{
		delay_ms(20);
		if (S4 == 0)
		{
			LED4 = SET;
			while (!S4)
				;
			delay_ms(500);
			delay_ms(500);
			MasterCar_TaskRunThread(0x04);
		}
	}
}

void HardInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); // �жϷ���

	/*
	Task_board_Init();								// ������ʼ��
	Infrared_Init();								// �����ʼ��
	
	BH1750_Configure();								// BH1750��ʼ������
	roadway_check_TimInit(999, 167);				// ·�����
	*/
	Ultrasonic_Init();		// ��������ʼ��
	delay_init(168);		// ��ʱ��ʼ��
	Cba_Init();				// ���İ��ʼ��
	Hard_Can_Init();		// CAN���߳�ʼ��
	Electricity_Init();		// ��������ʼ��
	UartA72_Init();			// A72Ӳ������ͨѶ��ʼ��
	Can_check_Init(7, 83);	// CAN���߶�ʱ����ʼ�� TIM7
	Timer_Init(999, 167);	// ��������ͨѶʱ��֡ TIM10
	Readcard_daivce_Init(); // RFID��ʼ��
	//TIM3_Int_Init(10000 - 1, 8400 - 1);				// ��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1s
	//TIM4_Int_Init(100 - 1, 8400 - 1);				// ��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����100��Ϊ10ms
	//while(RNG_Init());	 																// ��ʼ�������������
	//SYN_Init();										// ����ʶ���ʼ��
	//SYN7318_Init();//syn7318����ģ��
	BKRC_Voice_Init();

	Tim5_Init(1, 839);	 //WIFI����ʶ�����ݳ�ʼ��
	Tim2_Init(20, 8399); //�����ʹ��
	Tim4_Init(1, 8399);	 //zigbee���մ���
	Tim8_Init(10,8399); //����ѭ��
	TaskBoard_Init();
	/*add*/
}

int main(void)
{
	/*Hardware_Init();	   // Ӳ����ʼ��
	
	//Tim2_Init(1, 8399); //���������ж�
	*/

	HardInit(); //Ӳ����ʼ��

	MasterCar_Stop(); //ֹͣС��

	MP_SPK = 1; //���İ������
	delay_ms(200);
	MP_SPK = 0;
	delay_ms(200);
	MP_SPK = 1;
	delay_ms(200);
	MP_SPK = 0;
	Send_Debug_string("First\n"); // ��λ�ɹ���ʶ

	while (1)
	{
		keyScan();					   //�������
		Running_indicator_light();	   // ����ָʾ��
		//MasterCar_TaskRunThread();	   //����ִ�������߳�
		MasterCar_TaskReceiveThread(); //������������߳�
		Battery_power_detection();	   // ��ص������
	}
}

#if 0
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		/*if (Get_Flag_Track()) //��ѯ�Ƿ�ѭ���з�ֹ�ܷ�
		{
			if (MasterCar_SmartRun_CheckH8(Get_Host_UpTrack(TRACK_H8)) == 0 || MasterCar_SmartRun_CheckQ7(Get_Host_UpTrack(TRACK_Q7)) == 0)
			{
				MasterCar_Stop();
			}
		}*/
		//Zigbee_Wifi_receive();		// Zigbee�������ݴ���
		//Interrupt_flag_bit_query(); // �жϱ�־λ��ѯ TIM3�ж�
		//MasterCar_TaskReceiveThread(); //������������߳�  ������
		Battery_power_detection(); // ��ص������
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
#endif
