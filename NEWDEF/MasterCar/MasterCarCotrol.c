/*
 * @Description:  �����ٿ�
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-05 10:01:13
 * 
 * ����1800 ����2250
 */

#include "MasterCarCotrol.h"
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
#include "TaskBoard.h"//�����
#include "math.h"
#include "stdio.h"
#include "cba.h"//������
#include "bkrc_voice.h"//С������
#include "CRC.h"
unsigned char _Flag_Task = 0;  //�����־
unsigned int MasterCar_GoSpeed = 40;	//����Ĭ��ѭ�������ٶ�
unsigned int MasterCar_TrunSpeed = 90;	//����ת���ٶ�
#define MasterCar_GoMpValue     280 //�ƶ�ʱ����ǰ��ǰ������ֵ
#define MasterCar_GoMpValue2     360 //ͣ����������ǰ������ֵ
#define MasterCar_LeftMPV_45    410 //������ת45��
#define MasterCar_RightMPV_45   450 //������ת45��
#define MasterCar_LeftMPV_90    MasterCar_LeftMPV_45*2 
#define MasterCar_RightMPV_90   MasterCar_RightMPV_45*2
#define MasterCar_LeftMPV_180   MasterCar_LeftMPV_45*4
#define MasterCar_RightMPV_180  MasterCar_RightMPV_45*4
/*��������ѭ����ת�䣬����MP����ֵ ��ֹС��ת���ܷ� �����������Ӵ���ƫ�����Էֱ���*/
#define LeftMP_45_limit MasterCar_LeftMPV_45+50
#define RightMP_45_limit MasterCar_RightMPV_45+50
#define LeftMP_90_limit MasterCar_LeftMPV_90+50
#define RightMP_90_limit MasterCar_RightMPV_90+50
#define LeftMP_180_limit MasterCar_LeftMPV_180+50
#define RightMP_180_limit MasterCar_RightMPV_180+50


uint8_t debugbuf[99];//����
uint8_t DebugKEY=0; //0.�ر�DebugKEY 1.����
/*flag����*/
void CleanFalgAll(void)
{
	_Flag_Task = 0;
}
unsigned char Get_Flag_Task(void) { return _Flag_Task; }
void Set_Flag_Task(unsigned int task) { _Flag_Task = task; }

/**
 * @description: ͣ�����ȴ�S2���ڽ�����һ������
 * @param {*}
 * @return {*}
 */
void MasterCar_DebugKEY(void)
{
	
	if(DebugKEY==1)
	{
		Send_UpMotor(0,0);
		Send_Debug_string2("s2 wait");
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
	delay_ms(150);
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
	_mp+=Roadway_mp_Get();
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;
	Send_UpMotor(speed, speed);
	while (Roadway_mp_Get()<_mp )
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
	_mp += Roadway_mp_Get();
	if (speed > 100)
		speed = 100;
	if (speed < 5)
		speed = 5;
	Send_UpMotor(-speed, -speed);

	while (Roadway_mp_Get()<=_mp)
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
		while (Roadway_mp_Get()<MasterCar_LeftMPV_90);
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
	case 1://���̹���
		while (Roadway_mp_Get()<MasterCar_RightMPV_90);
		Send_UpMotor(0, 0);
		break;
	case 3://180��ѭ���ƹ���
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
	case 4://45�ȹջع��
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
	case 5://��ʮ�����̹���
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
	CleanFalgAll();
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
	MasterCar_BackMP(MasterCar_GoSpeed, 900); //��ǰ��������
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 500); //��ǰ��������
	MasterCar_BackMP(MasterCar_GoSpeed, mp);	  //����
	MasterCar_Stop();
	PID_Set_recovery();
}

//����ѭ������ ���3�����ϡ�ȫ����ȫ�� ֹͣѭ����Ѱ��ѭ����
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

//����ѭ��ȫ����ֹͣѭ��
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
	delay_ms(100);//���ﲻ���ӳ�����ǰ�����̲��ȶ�
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
	delay_ms(100);//���ﲻ���ӳ�����ǰ�����̲��ȶ�
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
	PID_Set(25,0,300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Left(MasterCar_TrunSpeed,1);
	Send_Debug_num2(MasterCar_LeftMPV_90);
	PID_Set_recovery();
}
void getRightMP(void)
{
	PID_Set(25,0,300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	Send_Debug_num2(MasterCar_RightMPV_90);
	PID_Set_recovery();
}
void getStopGoMP(void)
{
	PID_Set(25,0,300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	Send_Debug_num2(MasterCar_GoMpValue2);
	PID_Set_recovery();
}












uint8_t Zigbee_receive_CarPlate[6] = {'A', '9', '9', '9', '9', '9'}; //���ո�����������
unsigned int Zigbee_receive_RFIDKey[6];								 //����RFID��Կ
uint8_t Wifi_receive_data[6] = {0, 0, 0, 0, 0, 0};					 //wifi�������ݴ�Ż�����
/**
 * @description: ��������Wifi Zigbee�����߳�
 * �������޲����»��밲׿��ͨ��ʱ���Զ����ø��߳�
 * Zigbee_Rx_flag & Wifi_Rx_flag�����������0
 * @param {*}
 * @return {*}
 */
void MasterCar_TaskReceiveThread(void)
{
	if (OFlag_SLAVEflag && TaskBoard_TimeStatus()==0)//�ȴ���������ʱ
	{
		OFlag_SLAVEflag=0;
		TaskBoard_TimeClose();//�رյ���ʱ���
		Send_Debug_string2("T->go");
		task_wait();//�����ȴ���������
	}
	
	/*wifi����*/
	if (Wifi_Rx_flag)
	{
		if (OWiFi_toZigbee() == 1) //WIFI��Zigbee����
		{
			//Agreement_Send(2, Wifi_Rx_Buf); //ZigbeeͨѶ
		} // wifiת����zigbee
		else
		{
			Operation_WiFi_Master(Wifi_Rx_Buf); //WIFI��������������
		}

		Wifi_Rx_flag = 0;
	}

	//zigbee����
	if (Zigbee_Rx_flag)
	{
		switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee�ڵ㷴��ѡ��
		{

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
			if(OFlag_ETCflag)
			{
				OFlag_ETCflag=0;
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
		if(OFlag_SLAVEflag)
		{
			OFlag_SLAVEflag=0;
			TaskBoard_TimeClose();//�رյ���ʱ���
			Send_Debug_string2("S->go");
			task_wait(); //��������ָ�� taskID���������޸�
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
	case 0xB0: //·�Ƴ�ʼ��λ
			   /*MO5=Zigb_Rx_Buf[3];
		Send_Debug_string("MO5=");
		Send_Debug_num(MO5);
		Send_Debug_string("\n");*/
		break;
	default:
		break;
	}
}


























/******************************��Ϣ���********************************/
#define e 2.71828
#define PI 3.14159


/**
 * @description: ������������ѡ�� 
 * �趨������ǰ�������� ͨ��Set_Flag_Task()ѡ������
 * @param {*}
 * @return {*}
 */
//int testv=200;
//char testdata[6]={'A','1','B','2',0,0};
int testv=26;
void MasterCar_TaskRunThread(void)
{
	unsigned char t = _Flag_Task;
	_Flag_Task = 0xff;
	switch (t)
	{
	case 0x01:
		task_first();
		break;
	case 0x02:
		PID_Set(25, 0, 300);
		task_RFID();
		break;
	case 0x03:
		PID_Set(25, 0, 300);
		task_wait();
		break;
	case 0x04:
		OFlag_SlaveRun();
		break;
/*
	case 0x05:
		//MasterCar_Right(MasterCar_TrunSpeed, test_buffer);
		break;
	case 0x06: //�ȴ�������������ָ��
		Send_Debug_string2("M->go");
		task_wait();
		break;
	case 0x07: //�ȴ�ETC
		if(OFlag_ETCflag)
		{
			task_ETC();
			OFlag_ETCflag=0;
		}
		break;
	case 0x08:
		task_waitWifi(); //�ȴ�wifi
		break;
	case 0x09:
		break;
*/
	default:
		_Flag_Task = 0xff;

		break;
	}
	t = 0xff;
}



/**
 * @description:************************��ʼ����********************************
 * @param {*}
 * @return {*}
 */

void task_first(void)
{
	OFlag_resetWaitFlag();
	PID_Set(25, 0, 300);
	
	/*����1*/
	delay_ms(100);
	OFlag_LED_time(2);	//LED��ʾ����
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1);	//LED��ʾ����ʱ����
	OFlag_LED_time(1);
	delay_ms(200);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,800);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	OFlag_LED_jl( TaskBoard_WAVE() );//LED��ʾ����ģʽ
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_Left(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	OFlag_light2(4);//·�Ƶ��ڵ�ָ����λ����ȡ��ʼ��λ
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	BKRC_Voice_Extern(0,3);//С������ʶ��3��
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	OFlag_ltDisplay_show("A12345","F1");//������ʾ������
	delay_ms(200);
	OFlag_ltDisplay_show("A12345","F1");//������ʾ������
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	task_RFID();
	
}
void task_second(void)
{
	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	OFlag_SlaveRun();//��������
	OFlag_SlaveRun_wait(30);
}

/**
 * @description: **********************�ȴ�����ָ��*************************
 * @param {*}
 * @return {*}
 */
void task_wait(void)
{
	PID_Set(25, 0, 300);
	
	OFlag_DZ_open("B98765");
	delay_ms(500);
	delay_ms(500);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	OFlag_alarm_open("123456");
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);

	MasterCar_BackEnter(1780);
	OFlag_LED_time(0);
	delay_ms(200);
	OFlag_LED_time(0);

	while (1)
	{
		/* code */
	}
}

/**
 * @description: ************************��������**************************
 * @param {*}
 * @return {*}
 */
void task_test(void)
{
	
}

/**
 * @description:  ************************ETC�ȴ�����**************************
 * @param {*}
 * @return {*}
 */
void task_ETC(void)
{
	task_second();
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
	Send_Debug_num2(RC_Card_checkRangeRead(MasterCar_GoMpValue2,900,RC_Get_address(0,1),K_A));
	Send_Debug_string2(RC_Get_buffer());
	Send_Debug_num2(RC_Card_checkRangeRead(0,900,RC_Get_address(0,1),K_A));
	Send_Debug_string2(RC_Get_buffer());
	MasterCar_BackMP(MasterCar_GoSpeed,600);
	MasterCar_SmartRun2(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_Left(MasterCar_TrunSpeed,1);
	Send_Debug_num2(RC_Card_checkRangeRead(MasterCar_GoMpValue2,1200,RC_Get_address(0,1),K_A));
	Send_Debug_string2(RC_Get_buffer());
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	OFlag_ETC_wait();
}
