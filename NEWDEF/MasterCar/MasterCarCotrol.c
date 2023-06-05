/*
 * @Description:  �����ٿ�
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 20:00:52
 * 
 * ����1800 ����2250
 */
#include "sys.h"
#include "MasterCarCotrol.h"
#include "power_check.h"
#include "CanP_HostCom.h"
#include "can_user.h" //Zigbee_Rx_flag
#include "delay.h"
#include "drive.h"
#include "ultrasonic.h"
#include "bh1750.h"		   //��ȡ���ն�
#include "PID.h"		   //PIDѭ��
#include "Timer.h"		   //��ʱ��
#include "roadway_check.h" //����ͬ��
#include "infrared.h"	   //����
//#include "Card.h"		   //RFID����
//#include "rc522.h"		   //RFID
#include "RFIDCARD.h"
#include "OperationFlag.h"
#include "zigbee.h"	 //wifi��������
#include "syn7318.h" //syn7318����ʶ��
#include "OperationWiFi.h"
#include "MyString.h"
#include "TaskBoard.h" //�����
#include "math.h"
#include "stdio.h"
#include "cba.h"		//������
#include "bkrc_voice.h" //С������
#include "CRC.h"

uint8_t debugbuf[99]; //����
uint8_t DebugKEY = 0; //0.�ر�DebugKEY 1.����

//unsigned char _Flag_Task = 0;  //�����־
unsigned int MasterCar_GoSpeed = 40;   //����Ĭ��ѭ�������ٶ�
unsigned int MasterCar_TrunSpeed = 90; //����ת���ٶ�
#define MasterCar_GoMpValue 340		   //�ƶ�ʱ����ǰ��ǰ������ֵ
#define MasterCar_GoMpValue2 365	   //ͣ����������ǰ������ֵ
#define MasterCar_LeftMPV_45 407	   //410 //������ת45��
#define MasterCar_RightMPV_45 460	   //450 //������ת45��
#define MasterCar_LeftMPV_90 MasterCar_LeftMPV_45 * 2
#define MasterCar_RightMPV_90 MasterCar_RightMPV_45 * 2
#define MasterCar_LeftMPV_180 MasterCar_LeftMPV_45 * 4
#define MasterCar_RightMPV_180 MasterCar_RightMPV_45 * 4
/*��������ѭ����ת�䣬����MP����ֵ ��ֹС��ת���ܷ� �����������Ӵ���ƫ�����Էֱ���*/
#define LeftMP_45_limit MasterCar_LeftMPV_45 + 50
#define RightMP_45_limit MasterCar_RightMPV_45 + 50
#define LeftMP_90_limit MasterCar_LeftMPV_90 + 50
#define RightMP_90_limit MasterCar_RightMPV_90 + 50
#define LeftMP_180_limit MasterCar_LeftMPV_180 + 50
#define RightMP_180_limit MasterCar_RightMPV_180 + 50

void default_PID()
{
	PID_Set(20, 0, 300);
}

/**
 * @description: ͣ�����ȴ�S2���ڽ�����һ������
 * @param {*}
 * @return {*}
 */

void MasterCar_DebugKEY(void)
{
	if (DebugKEY)
	{
		Send_UpMotor(0, 0);
		Send_Debug_string2("s2 wait");
		delay_ms(200);
		while (1)
		{
			if (S2 == 0)
			{
				delay_ms(20);
				if (S2 == 0)
				{
					LED2 = SET;
					while (!S2)
						;
					break;
				}
			}
		}
		Send_Debug_string2("s2 run");
		delay_ms(200);
	}
}
/**
 * @description: ����ǰ��
 * @param {unsigned int} speed ǰ���ٶ� 0~100
 * @return {*}
 */
void MasterCar_Go(unsigned int speed)
{
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;

	Send_UpMotor(speed, speed);
	//delay_ms(150);
}

/**
 * @description: ����ǰ��2���׿�ʮ��·�ڿ�ֹͣ��
 * @param {unsigned int} speed ǰ���ٶ� 0~100
 * @return {*}
 */
void MasterCar_Go2(unsigned int speed)
{
	uint16_t data7, data8, data8L;
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;
	Send_UpMotor(speed, speed);
	while (1)
	{
		data8 = Get_Host_UpTrack(TRACK_H8);
		data7 = Get_Host_UpTrack(TRACK_Q7);
		data8L = Get_Host_TrackDieCount(TRACK_H8);
		if (data8L > 3 || data7 == 0x3e || data8 == 0x7c | data8 == 0x3E || data8 == 0x7E)
		{
			break;
		}
		delay_ms(1);
	}
}

/**
 * @description: ����ǰ������ֵ
 * �ο�ֵ:
 * 1300(������α�־��պ�ѭ���ƹ���ɫ��)
 * 1850-1900(������α�־��պó�λ���߹���ɫ��)
 * @param {unsigned int} speed ǰ���ٶ� 0~100
 * @param {unsigned int} mp ��ǰ������ֵ
 * @return {*}
 */
void MasterCar_GoMP(unsigned int speed, unsigned int mp)
{
	int _mp = mp;
	delay_ms(100);
	Roadway_mp_syn();
	//_mp+=Roadway_mp_Get();
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;
	Send_UpMotor(speed, speed);
	while (Roadway_mp_Get() < _mp)
	{
		delay_ms(1);
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}

/**
 * @description: ��������
 * @param {unsigned int} speed �����ٶ� 0~100
 * @return {*}
 */
void MasterCar_Back(unsigned int speed)
{
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;

	Send_UpMotor(-speed, -speed);
	delay_ms(100);
}

/**
 * @description: ������������ֵ
 * @param {unsigned int} speed �����ٶ� 0~100
 * @param {unsigned int} mp ����������ֵ
 * @return {*}
 */
void MasterCar_BackMP(unsigned int speed, unsigned int mp)
{
	int _mp = mp;
	delay_ms(100);
	Roadway_mp_syn();
	//_mp += Roadway_mp_Get();

	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;
	Send_UpMotor(-speed, -speed);

	while (Roadway_mp_Get() <= _mp)
	{
		delay_ms(1);
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}

#if 1
/**
 * @description: ������ת����ֵ+ѭ�����ж� 
 * @param {unsigned int} speed �����ٶ� 0~100
 * @param {unsigned int} mode 1.ʮ��·��90�� 2.ֱ��180�� 3.ʮ��·��180��
 * @return {*}
 */
void MasterCar_Left(unsigned int speed, unsigned int mode) //������90��
{
	int _Track, _mp, _break = 1;
	delay_ms(100);
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	Send_UpMotor(-speed, speed);
	switch (mode)
	{
	case 0:
		break;
	case 1:
		//����
		while (Roadway_mp_Get() < MasterCar_LeftMPV_90)
			;
		Send_UpMotor(0, 0);
		break;
	case 3:
		while (_break)
		{
			_mp = Roadway_mp_Get();
			if (1700 < _mp && _mp < LeftMP_180_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > LeftMP_180_limit) //��ֹ���ʧ��
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	case 4:
		while (_break)
		{
			_mp = Roadway_mp_Get();
			if (400 < _mp && _mp < LeftMP_45_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > LeftMP_45_limit) //��ֹ���ʧ��
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	case 5:
		//����+ѭ����
		while (_break)
		{
			_mp = Roadway_mp_Get();
			if (700 < _mp && _mp < LeftMP_90_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					MasterCar_LeftMP(speed, 15);
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > LeftMP_90_limit) //��ֹ���ʧ��
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	default:
		break;
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}

/**
 * @description: ������ת����ֵ+ѭ�����ж�  
 * @param {unsigned int} speed ��ת�ٶ� 0~100
 * @param {unsigned int} mode 1.ʮ��·��90�� 2.ֱ��180�� 3.ʮ��·��180�� 4.45��ѭ����
 * @return {*}
 */
void MasterCar_Right(unsigned int speed, unsigned int mode)
{
	int _Track, _mp, _break = 1;
	delay_ms(100);
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	Send_UpMotor(speed, -speed);

	switch (mode)
	{
	case 0:
		break;
	case 1: //���̹���
		while (Roadway_mp_Get() < MasterCar_RightMPV_90)
			;
		Send_UpMotor(0, 0);
		break;
	case 3: //180��ѭ���ƹ���
		while (_break)
		{

			_mp = Roadway_mp_Get();
			if (1700 < _mp && _mp < RightMP_180_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > RightMP_180_limit)
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	case 4: //45�ȹջع��
		while (_break)
		{

			_mp = Roadway_mp_Get();
			if (400 < _mp && _mp < RightMP_45_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > RightMP_45_limit)
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	case 5: //��ʮ�����̹���
		while (_break)
		{
			_mp = Roadway_mp_Get();
			if (700 < _mp && _mp < RightMP_90_limit)
			{
				_Track = Get_Host_UpTrack(TRACK_H8);
				if (_Track == 0xEF || _Track == 0xF7)
				{
					MasterCar_RightMP(speed, 15);
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
				if (_Track == 0xE7)
				{
					Send_UpMotor(0, 0);
					_break = 0;
					continue;
				}
			}
			else if (_mp > RightMP_90_limit) //��ֹ���ʧ��
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	default:
		break;
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}
#else

#endif

/**
 * @description: ��ת ���̰�
 * @param {unsigned int} speed �ٶ�
 * @param {unsigned int} mp	����
 * @return {*}
 */
void MasterCar_LeftMP(unsigned int speed, unsigned int mp)
{
	int _mp = 0;
	delay_ms(100);
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	Send_UpMotor(-speed, +speed);
	while (_mp <= mp)
	{
		_mp = Roadway_mp_Get();
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}
/**
 * @description: ��ת ���̰�
 * @param {unsigned int} speed �ٶ�
 * @param {unsigned int} mp	����
 * @return {*}
 */
void MasterCar_RightMP(unsigned int speed, unsigned int mp)
{
	int _mp = 0;
	delay_ms(100);
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	Send_UpMotor(speed, -speed);
	while (_mp <= mp)
	{
		_mp = Roadway_mp_Get();
	}
	Send_UpMotor(0, 0);
	delay_ms(100);
}

/**
 * @description: ������ͣ
 * @param {*}
 * @return {*}
 */
void MasterCar_Stop(void)
{
	Send_UpMotor(0, 0);
	delay_ms(100);
}

/**
 * @description: ����ת�䵹����� 
 * @param {unsigned int} mp �ӵ�������λ�������mp����  �ο�ֵ1800
 * @return {*}
 */
void MasterCar_BackEnter(unsigned int mp)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 900); //��ǰ��������
	MasterCar_BackMP(MasterCar_GoSpeed, 900);	   //��ǰ��������
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 500); //��ǰ��������
	MasterCar_BackMP(MasterCar_GoSpeed, mp);	   //����
	MasterCar_Stop();
	PID_Set_recovery();
}

/**
 * @description: ����ѭ������ ���3�����ϡ�ȫ����ȫ�� ֹͣѭ����Ѱ��ѭ��������ʮ��·�ڴ��ڰ׿� ע��ӽ�����ʹ�� ��ֹѭ��������
 * �������MasterCar_SmartRunMP();ʹ�� ��ֹʮ��·�ڱ�ĥ���������
 * @param {*}
 * @return {*}
 */

void MasterCar_SmartRun2(unsigned int speed)
{
	uint8_t _speed; //����MasterCar_GoSpeed ѭ����ɻָ�
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //����PID���Ӳ���
	while (PID_Track5(speed) != 99)
	{
		delay_ms(1); //��ֹˢ��̫��ѭ������
	}
	//�ȴ�ѭ�����
	MasterCar_GoSpeed = _speed; //�ָ��ٶ�
	PID_Set_recovery();			//�ָ�PID����
}

/**
 * @description: ����ѭ�����У�ȫ���߻�׿�����ʮ�ֺ��ߴ�ͣ�� ע���׿�����һ��ƽ�������ߵĺ��� ���ڰ׿����ں���ʮ��·��ʱ ͣ����������
 * @param {unsigned int} speed �����ٶ� 0~100
 * @return {*}
 */
void MasterCar_SmartRun1(unsigned int speed)
{
	uint8_t _speed; //����MasterCar_GoSpeed ѭ����ɻָ�
	MasterCar_Go(speed);
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //����PID���Ӳ���
	while (PID_Track(speed) != 99)
	{
		delay_ms(1); //��ֹˢ��̫��ѭ������
	}
	//�ȴ�ѭ�����
	MasterCar_GoSpeed = _speed; //�ָ��ٶ�
	PID_Set_recovery();			//�ָ�PID����
}

/**
 * @description: ����ѭ������ ��ͨѭ������
 * @param {unsigned int} speed
 * @return {*}
 */
void MasterCar_SmartRun(unsigned int speed)
{
	uint8_t _speed; //����MasterCar_GoSpeed ѭ����ɻָ�
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //����PID���Ӳ���
	while (PID_Track2(speed) != 99)
	{
		delay_ms(1); //��ֹˢ��̫��ѭ������
	}
	//�ȴ�ѭ�����
	MasterCar_GoSpeed = _speed; //�ָ��ٶ�
	PID_Set_recovery();			//�ָ�PID����
}

/**
 * @description: ����ѭ������ ���̰�
 * @param {unsigned int} speed �ٶ�
 * @param {unsigned int} speed ����ֵ
 * @return {*}
 */
void MasterCar_SmartRunMP(unsigned int speed, unsigned int MP)
{

	int _mp = 0;
	uint8_t _speed; //����MasterCar_GoSpeed ѭ����ɻָ�
	delay_ms(100);	//���ﲻ���ӳ�����ǰ�����̲��ȶ�
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;
	MasterCar_Go(speed);
	//_Flag_Track = 1;

	PID_reset(); //����PID���Ӳ���

	while (_mp <= MP)
	{
		_mp = Roadway_mp_Get();
		PID_TrackMP(speed);
		if (_mp >= MP)
		{
			Send_UpMotor(0, 0);
			break;
		}
		delay_ms(1); //��ֹˢ��̫��ѭ������

	} //�ȴ�ѭ�����
	Send_UpMotor(0, 0);
	MasterCar_GoSpeed = _speed;
	delay_ms(100);
}

/**
 * @description: ����ѭ������ ���̰� ����У׼����
 * @param {unsigned int} speed �ٶ�
 * @param {unsigned int} speed ����ֵ
 * @return {*}
 */
void MasterCar_SmartRunMP2(unsigned int speed, unsigned int MP)
{

	int _mp = 0;
	uint8_t _speed; //����MasterCar_GoSpeed ѭ����ɻָ�
	delay_ms(100);	//���ﲻ���ӳ�����ǰ�����̲��ȶ�
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;
	MasterCar_Go(speed);
	//_Flag_Track = 1;

	PID_reset(); //����PID���Ӳ���

	while (_mp <= MP)
	{
		_mp = Roadway_mp_Get();
		PID_TrackMP2(speed);
		if (_mp >= MP)
		{
			Send_UpMotor(0, 0);
			break;
		}
		delay_ms(1); //��ֹˢ��̫��ѭ������

	} //�ȴ�ѭ�����
	Send_UpMotor(0, 0);
	MasterCar_GoSpeed = _speed;
	delay_ms(100);
}

//���Զ�����
void getLeftMP(void)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	MasterCar_LeftMP(MasterCar_TrunSpeed, MasterCar_LeftMPV_90);
	Send_Debug_num2(MasterCar_LeftMPV_90);
	PID_Set_recovery();
}
void getRightMP(void)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	MasterCar_RightMP(MasterCar_TrunSpeed, MasterCar_RightMPV_90);

	Send_Debug_num2(MasterCar_RightMPV_90);
	PID_Set_recovery();
}
void getStopGoMP(void)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue2);
	Send_Debug_num2(MasterCar_GoMpValue2);
	PID_Set_recovery();
}
#define Zigbee_Number_len 0xff+1
static uint8_t Zigbee_receive_Number[Zigbee_Number_len];
uint8_t Zigbee_receive_light = 0;
uint8_t Zigbee_receive_alarm[7] = {'1', '2', '3', '4', '5', '6', '\0'}; //���շ��̨������
uint8_t Zigbee_receive_CarPlate[7] = {'A', '9', '9', '9', '9', '9','\0'};	//���ո�����������
uint8_t Zigbee_receive_RFIDKey[6];								//����RFID��Կ
//uint8_t Wifi_receive_data[6] = {0, 0, 0, 0, 0, 0};						//wifi�������ݴ�Ż�����
/**
 * @description: ��������Wifi Zigbee�����߳�
 * �������޲����»��밲׿��ͨ��ʱ���Զ����ø��߳�
 * Zigbee_Rx_flag & Wifi_Rx_flag�����������0
 * @param {*}
 * @return {*}
 */
int OFlag_SLAVEtask;
void MasterCar_TaskReceiveThread(void)
{
	if (OFlag_SLAVEflag && TaskBoard_TimeStatus() == 0) //�ȴ���������ʱ
	{
		Send_Debug_string2("time out");
		OFlag_SLAVEtask=OFlag_SLAVEflag;
		OFlag_SLAVEflag = 0;
		TaskBoard_TimeClose(); //�رյ���ʱ���
		switch (OFlag_SLAVEtask)
		{
		case 0x01:
			task_wait();		   //�����ȴ���������
			break;
		case 0x02:
			task_wait2();		   //�����ȴ���������
			break;
		default:
			
			break;
		}
		
	}

	/*wifi����*/
	if (Wifi_Rx_flag)
	{
		if (OWiFi_toZigbee() == 1) //WIFI��Zigbee����
		{						   /*
			if (zNum_TFT_A == Wifi_Rx_Buf[1] && 0x10 == Wifi_Rx_Buf[2])
			{
				Send_Debug_num2(Wifi_Rx_Buf[3]);
			}*/

			//Agreement_Send(2, Wifi_Rx_Buf); //ZigbeeͨѶ
		} // wifiת����zigbee
		else
		{

			Operation_WiFi_Master(Wifi_Rx_Buf); //WIFI��������������
		}

		Wifi_Rx_flag = 0;
	}

	//zigbee����
	if (Zigbee_Rx_flag && !(TIM4->CR1 & 0x01)) //��ֹ��TIM4����zigbee���ݳ�ͻ
	{
		switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee�ڵ㷴��ѡ��
		{
		case 0x00: //У��ʧ�� ͨѶ���׶���
			/*
			Send_Debug_HEX(Zigb_Rx_Buf[0]);
			Send_Debug_HEX(Zigb_Rx_Buf[1]);
			Send_Debug_HEX(Zigb_Rx_Buf[2]);
			Send_Debug_HEX(Zigb_Rx_Buf[3]);
			Send_Debug_HEX(Zigb_Rx_Buf[4]);
			Send_Debug_HEX(Zigb_Rx_Buf[5]);
			Send_Debug_HEX(Zigb_Rx_Buf[6]);
			Send_Debug_HEX(Zigb_Rx_Buf[7]);
			Send_Debug_string2("check_error");
		*/

			break;
		case zNum_MasterCar: //���Դӳ���������ָ��
			SlaveCar_TaskRunThread(Zigb_Rx_Buf);
			break;

		case zNum_CK_A: //����A
			break;
		case zNum_CK_B: //����B
			break;
		case zNum_DZ: //��բ
			break;
		case zNum_JT_A: //���ܽ�ͨ��A
			break;
		case zNum_JT_B: //���ܽ�ͨ��B
			break;

		case zNum_ETC: //ETC
			if (OFlag_ETCflag == 0xf1)
			{
				OFlag_ETCflag = 0;
				task_ETC();
			}
			break;
		case zNum_DX: //�������
			break;
		case zNum_YY:
			break;
		default: //δ����zigbee���
			Send_Debug_string("undefine\n");
			break;
		}

		Zigbee_Rx_flag = 0;
	}
}

/**
 * @description: ��������Zigbee���ո�������ָ��
 * @param {unsigned char*} id ����֡
 * @return {*}
 */
void SlaveCar_TaskRunThread(unsigned char *data)
{

	switch (OFlag_GetCmd(data))
	{
	case 0x01:
		switch (OFlag_SLAVEflag)
		{
		case 0x01:
			OFlag_SLAVEflag = 0;
			TaskBoard_TimeClose(); // �رյ���ʱ���
			Send_Debug_string2("slave...");
			task_wait(); //��������ָ�� taskID���������޸�
			
			break;
		case 0x02:
			OFlag_SLAVEflag = 0;
			TaskBoard_TimeClose(); // �رյ���ʱ���
			Send_Debug_string2("slave2...");
			task_wait2(); //��������ָ�� taskID���������޸�
			
			break;
		default:
			break;
		} 

		break;
	case 0x03:
		//Send_Debug_string("dz\n");
		OFlag_DZ_open("");
		//OFlag_DZ_open(Master_CarLicensePlate); //�򿪵�բ
		break;
	case 0x0A:
		Zigbee_receive_RFIDKey[0] = OFlag_GetCmd1(data);
		Zigbee_receive_RFIDKey[1] = OFlag_GetCmd2(data);
		Zigbee_receive_RFIDKey[2] = OFlag_GetCmd3(data);
		//RFID��Կ1
		break;
	case 0x0B:
		Zigbee_receive_RFIDKey[3] = OFlag_GetCmd1(data);
		Zigbee_receive_RFIDKey[4] = OFlag_GetCmd2(data);
		Zigbee_receive_RFIDKey[5] = OFlag_GetCmd3(data);
		//RFID��Կ2
		break;
	case 0x0C:
		Zigbee_receive_CarPlate[0] = OFlag_GetCmd1(data);
		Zigbee_receive_CarPlate[1] = OFlag_GetCmd2(data);
		Zigbee_receive_CarPlate[2] = OFlag_GetCmd3(data);
		//��������1
		break;
	case 0x0D:
		Zigbee_receive_CarPlate[3] = OFlag_GetCmd1(data);
		Zigbee_receive_CarPlate[4] = OFlag_GetCmd2(data);
		Zigbee_receive_CarPlate[5] = OFlag_GetCmd3(data);
		//��������2
		break;
	case 0x08:
		Zigbee_receive_alarm[0] = OFlag_GetCmd1(data);
		Zigbee_receive_alarm[1] = OFlag_GetCmd2(data);
		Zigbee_receive_alarm[2] = OFlag_GetCmd3(data);
		//���̨����1
		break;
	case 0x09:
		Zigbee_receive_alarm[3] = OFlag_GetCmd1(data);
		Zigbee_receive_alarm[4] = OFlag_GetCmd2(data);
		Zigbee_receive_alarm[5] = OFlag_GetCmd3(data);
		//���̨����2
		break;
	case 0xB0: //·�Ƴ�ʼ��λ
		Zigbee_receive_light = OFlag_GetCmd1(data);
		/*MO5=Zigb_Rx_Buf[3];
		Send_Debug_string("MO5=");
		Send_Debug_num(MO5);
		Send_Debug_string("\n");*/
	case 0xFE: // ���ո������͵���ֵ
		if (Zigbee_Number_len > OFlag_GetCmd1(data))//��ֹ����Խ��
		{
			Zigbee_receive_Number[OFlag_GetCmd1(data)] = OFlag_GetCmd2(data);
		}
		break;
	default:
		break;
	}
}

/*
����ʡʡ�����������
MasterCar_SmartRun(MasterCar_GoSpeed);
MasterCar_Stop();
MasterCar_BackMP(MasterCar_GoSpeed,250);
delay_ms(200);
MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45/2);
MasterCar_GoMP(MasterCar_GoSpeed,700);
MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45/2+MasterCar_RightMPV_45);
*/

/******************************��Ϣ���********************************/
#define e 2.71828
#define PI 3.14159

char alarmKey[] = {'1', '2', '3', '4', '5', '6'};

/**
 * @description: ������������ѡ�� 
 * �趨������ǰ��������
 * @param {*}
 * @return {*}
 */

int testp = 15;
int testi;
static char *slave_p = NULL; // �ӳ�������
uint8_t *testarr;
void MasterCar_TaskRunThread(unsigned char mode)
{
	switch (mode)
	{
	case 0x01:
		/*DebugKEY=1;
	Send_Debug_string("debug=");
	Send_Debug_num2(DebugKEY);
*/

		task_first();
		break;
	case 0x02:
		DebugKEY = 1;
		OFlag_WX_open("");
		/*
		if (OWiFi_TFT('A', 60)) //����TFTAʶ��
		{

			//ͼ����ɫ����
			Send_Debug_num2(OWifi_TFTShape[0]);
			Send_Debug_num2(OWifi_TFTShape[1]);
			Send_Debug_num2(OWifi_TFTShape[2]);

			Send_Debug_string2("JTFlag:");
			Send_Debug_num2(OWifi_TFTJTFLAG);
			OFlag_TFT_showHEX('A', 0xF, OWifi_TFTShape[0], 0xF, OWifi_TFTShape[1], 0xF, OWifi_TFTShape[2]); //HEX��ʾ
		}
		else
		{
			Send_Debug_string2("error");
		}

		Send_Debug_num2(OWifi_TFTShape[0]);
		Send_Debug_num2(OWifi_TFTShape[1]);
		Send_Debug_num2(OWifi_TFTShape[2]);

		Send_Debug_string2("JTFlag:");
		Send_Debug_num2(OWifi_TFTJTFLAG);
*/
		break;
	case 0x03:
		DebugKEY = 1;
		OFlag_WX_change("159");

		//Send_Debug_num2(TaskBoard_BH());
		/*
		if(RC_check())
		{
			Send_Debug_num2(1);
			Send_Debug_num2(RC_Get_address ( 5, 2));
			Send_Debug_string2(RC_Get_buffer());
		}else{
			Send_Debug_num2(0);
		}*/
		break;
	case 0x04:
		//DebugKEY=1;
		DebugKEY = 1;
		OFlag_WX_open("159");

		//OFlag_alarm_change("123456");//���̨�޸Ŀ�����

		//Send_Debug_num( OFlag_alarm_getP(10 ));//���̨��ȡ�����Ԯ����

		break;
	case 0x05:
		//MasterCar_Right(MasterCar_TrunSpeed, test_buffer);
		break;
	case 0x06: //�ȴ�������������ָ��
		//task_wait();
		break;
	case 0x07: //�ȴ�ETC
		/*
		if(OFlag_ETCflag)
		{
			task_ETC();
			OFlag_ETCflag=0;
		}*/
		break;
	case 0x08:
		task_waitWifi(); //�ȴ�wifi
		break;
	case 0x09:
		break;

	default:

		break;
	}
}

int carRead = 0;	 //�ж��Ƿ�ɹ�����
uint8_t carData[16]; //��ȷ�Ŀ�����
uint32_t jl = 0;	 //����������ֵ

uint8_t buf[50];
/**
 * @description:************************��ʼ����********************************
 * @param {*}
 * @return {*}
 */
#define HIDE 0
void task_first(void)
{
	Send_Debug_string2("task_first");
	default_PID();

	/*����1*/
	delay_ms(100);
	OFlag_LED_time(2); //LED��ʾ����
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1); //LED��ʾ����ʱ����
	OFlag_LED_time(1);
	delay_ms(200);

	/*D7��D6*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	/*D6��F6*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);

	MasterCar_Right(MasterCar_TrunSpeed, 1);
	MasterCar_BackMP(MasterCar_GoSpeed, 800);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();

	jl = TaskBoard_WAVE(); //���������
	OFlag_TFT_jl('B', jl); //TFTB��ʾ



	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue2);
	MasterCar_Left(MasterCar_TrunSpeed, 1);
#if HIDE
	delay_ms(500);
	delay_ms(500);
#else
	int mb = ((jl / 100) % 4) + 1;
	int cs = OFlag_light2(mb);//·�Ƶ��ڵ�1������ȡ��ʼ��λ

#endif

	/*F6��F4*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Left(MasterCar_TrunSpeed, 1);

	MasterCar_SmartRunMP(MasterCar_GoSpeed, 1000);

#if 1
	RC_Card_checkRangeReadPlus2(MasterCar_GoMpValue, 1000, RC_Get_address(2, 2), K_A, 1, 0); //��ָ�������ڶ���ѭ���ư�
	MasterCar_BackMP(MasterCar_GoSpeed, 700);
	MasterCar_SmartRun2(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
#else
	RC_Card_checkRangeReadPlus2(0, 800, RC_Get_address(2, 2), K_A, 1, 0); //��ָ�������ڶ���ѭ���ư�
#endif

	MasterCar_Right(MasterCar_TrunSpeed, 1);

#if HIDE
	delay_ms(500);
	delay_ms(500);
#else
	Send_Debug_string("YY:");
	Send_Debug_num2(BKRC_Voice_Extern(0, 3)); //С������ʶ��3��
#endif
	MasterCar_Left(MasterCar_TrunSpeed, 1);
	/*F4��D4*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Left(MasterCar_TrunSpeed, 1);

	OFlag_ETC_wait(); //�ȴ�ETC����

	/*

	MasterCar_BackEnter(1300);

	OFlag_LED_time(0);	//LED����ʱ�ر�
	delay_ms(200);
	OFlag_LED_time(0);
	delay_ms(200);
	OFlag_LED_time(0);

	OFlag_CK_Wait('A',2,30);
	OFlag_WX_open();
	delay_ms(200);
	OFlag_WX_open();

	while (1)
	{
		
	}
*/
}

/**
 * @description: **********************�ڶ�������*************************
 * @param {*}
 * @return {*}
 */
int f1, f2, jtfalg, data_m = 1, data_n = 1;
void task_second(void)
{
	Send_Debug_string2("task_second");
	if (DebugKEY)
	{
		default_PID();
	}

#if HIDE
	delay_ms(500);
	delay_ms(500);
#else
	Send_Debug_string2(Zigbee_receive_CarPlate);
	OFlag_DZ_open(Zigbee_receive_CarPlate);
	OFlag_DZ_open(Zigbee_receive_CarPlate);
	OFlag_DZ_open(Zigbee_receive_CarPlate);
	delay_ms(500);
	delay_ms(500);
#endif

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	/*B2��D2*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	OFlag_DX_carGo(35, 1250);

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);

	OFlag_SlaveRun_wait_flag( 20 ,0x02);//�ȴ����� 50s��ʱ
	
}

/**
 * @description: **********************�ȴ�����ָ��*************************
 * @param {*}
 * @return {*}
 */
void task_wait(void)
{
	Send_Debug_string2("task_wait");
	if (DebugKEY)
	{
		default_PID();
	}

	MasterCar_SmartRun2(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);

	/*B4��B2*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	task_second();
}

/**
 * @description: **********************�ȴ�����ָ��*************************
 * @param {*}
 * @return {*}
 */
void task_wait2(void)
{
	Send_Debug_string2("task_wait2");
	if (DebugKEY)
	{
		default_PID();
	}

	Send_Debug_HEX(Zigbee_receive_alarm[0]);
	Send_Debug_HEX(Zigbee_receive_alarm[1]);
	Send_Debug_HEX(Zigbee_receive_alarm[2]);
	Send_Debug_HEX(Zigbee_receive_alarm[3]);
	Send_Debug_HEX(Zigbee_receive_alarm[4]);
	Send_Debug_HEX(Zigbee_receive_alarm[5]);
	
	MasterCar_LeftMP(MasterCar_TrunSpeed, MasterCar_LeftMPV_45);
	delay_ms(200);
	OFlag_alarm_open(Zigbee_receive_alarm); // ���Ϳ����뿪�����̨
	delay_ms(200);
	OFlag_alarm_open(Zigbee_receive_alarm); // ���Ϳ����뿪�����̨
	delay_ms(200);
	OFlag_alarm_open(Zigbee_receive_alarm); // ���Ϳ����뿪�����̨
	delay_ms(200);
	MasterCar_RightMP(MasterCar_TrunSpeed, MasterCar_RightMPV_45);

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	MasterCar_BackEnter(1500);

	OFlag_LED_time(0); //LED����ʱ�ر�
	delay_ms(200);
	OFlag_LED_time(0);
	delay_ms(200);
	OFlag_LED_time(0);

	OFlag_WX_open("");
	delay_ms(200);
	OFlag_WX_open("");

	while(1);

	
}

/**
 * @description: ************************��������**************************
 * @param {*}
 * @return {*}
 */
void task_test(void)
{
	Send_Debug_string2("task_test");
	if (DebugKEY)
	{
		default_PID();
	}

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,350);
	MasterCar_Left(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,370);//ֱ�ӿ��ں�����

	PID_Set(25, 0, 300); //�ϵ���ǰ���ٵ�������
	#if HIDE
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	#else
	//���
	#endif

	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,600);
	PID_Set_recovery();


	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);

	OFlag_DX_carGo(36,1250);//��·��
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
}

/**
 * @description:  ************************ETC�ȴ�����**************************
 * @param {*}
 * @return {*}
 */
void task_ETC(void)
{
	Send_Debug_string2("task_ETC");
	if (DebugKEY)
	{
		default_PID();
	}

	task_RFID();

	/*
	
MasterCar_BackEnter(1400);

	OFlag_LED_time(0);	//LED����ʱ�ر�
	delay_ms(200);
	OFlag_LED_time(0);
	delay_ms(200);
	OFlag_LED_time(0);

	OFlag_CK_Wait('A',2,30);
	OFlag_WX_open();
	delay_ms(200);
	OFlag_WX_open();

	while (1)
	{
		
	}
*/
}

/**
 * @description:  ************************wifi��������ָ��**************************
 * @param {*}
 * @return {*}
 */
void task_waitWifi(void)
{
	task_first();
	//task_wait();
}

/**
 * @description: ************************��RFID������**************************
 * @param {*}
 * @return {*}
 */
void task_RFID(void)
{
	int i = 0, carId = 0, carOK = 0, S = 5, B = 3, card_flag = 0;
	static char *path,buf[50];
	RC_Card_checkRangeReadPlus2(0, 2250 * 2, RC_Get_address(2, 2), K_A, 2, 1); //��ָ�������ڶ���ѭ���ư�

	for ( i = 0; i < 3; i++)
	{
		if(RC_Get_bufferN(i)!=NULL)
		{
			path = get_path_one(RC_Get_bufferN(i),"B4","B6",buf);
			if(path!=NULL)
			{
				Send_Debug_string2(path);
				Send_Debug_string2(buf);
				
				if (slave_p == NULL) // ���Լ����ó�ʼ���� ����ʹ�ö�ȡ����
				{
					slave_p = path;
				}
				//���͸���������·��
				delay_ms(200);
				OFlag_SlaveSendZigbee( 0x08,  get_charToHEX(buf[0])*16+get_charToHEX(buf[1]),  
				get_charToHEX(buf[2])*16+get_charToHEX(buf[3]),  
				get_charToHEX(buf[4])*16+get_charToHEX(buf[5]));
				delay_ms(200);
				OFlag_SlaveSendZigbee( 0x09,  get_charToHEX(buf[6])*16+get_charToHEX(buf[7]),  
				get_charToHEX(buf[8])*16+get_charToHEX(buf[9]),  
				get_charToHEX(buf[10])*16+get_charToHEX(buf[11]));
				delay_ms(200);
				OFlag_SlaveSendZigbee( 0x08,  get_charToHEX(buf[0])*16+get_charToHEX(buf[1]),  
				get_charToHEX(buf[2])*16+get_charToHEX(buf[3]),  
				get_charToHEX(buf[4])*16+get_charToHEX(buf[5]));
				delay_ms(200);
				OFlag_SlaveSendZigbee( 0x09,  get_charToHEX(buf[6])*16+get_charToHEX(buf[7]),  
				get_charToHEX(buf[8])*16+get_charToHEX(buf[9]),  
				get_charToHEX(buf[10])*16+get_charToHEX(buf[11]));
			}
		}
	}
	Send_Debug_string2(slave_p);

	OFlag_SlaveSendZigbee(0x06, slave_p[0], slave_p[1], 0); // ASCII
	delay_ms(200);
	OFlag_SlaveSendZigbee(0x06, slave_p[0], slave_p[1], 0); // ASCII
	delay_ms(200);
	OFlag_SlaveSendZigbee(0x06, slave_p[0], slave_p[1], 0); // ASCII

	MasterCar_BackMP(MasterCar_GoSpeed, 800);

	MasterCar_SmartRun2(MasterCar_GoSpeed);
	MasterCar_Stop();
	MasterCar_BackMP(MasterCar_GoSpeed, 600);

	OFlag_SlaveRun();		 //��������
	OFlag_SlaveRun_wait(60); //�ȴ����� 50s��ʱ
}

/*
?? ?? //ֱ����תֱ�ӹ�·��
?? ?? MasterCar_SmartRun2(MasterCar_GoSpeed);//����ʮ��·������� ������MasterCar_SmartRun(MasterCar_GoSpeed);
?? ?? MasterCar_SmartRunMP(MasterCar_GoSpeed,360);//��������ֵ
?? ?? MasterCar_Right(MasterCar_TrunSpeed,1);
?? ?? MasterCar_SmartRun2(MasterCar_GoSpeed);
?? ?? MasterCar_BackMP(MasterCar_GoSpeed,500);
?? ?? OFlag_DX_carGo(36,1250);//��·��
??*/