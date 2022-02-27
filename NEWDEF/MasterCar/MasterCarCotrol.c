/*
 * @Description:  �����ٿ�
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-22 16:55:48
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

//����ѭ�� ���3�����ϻ�ȫ�� ֹͣѭ��
void MasterCar_SmartRun3(unsigned int speed)
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
 * @description: ����ѭ�����У���ʼ�汾��
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
				task_ETC();
				OFlag_ETCflag=0;
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
		/*if (SLAVE_flag==0)
		{		
			SLAVE_flag=1;//�ӳ�����������־
			task_wait(); //��������ָ�� taskID���������޸�
		}*/
		if(OFlag_SLAVEflag)
		{
			task_wait(); //��������ָ�� taskID���������޸�
			OFlag_SLAVEflag=0;
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


uint8_t *MO8= "A12345",*MO2,*MO4;
int  MO1 = 2,MO3=0,MO9=4,MO6=3;


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
		//getLeftMP();
		//task_wait();
		task_RFID();
		//task_test();
		//OWiFi_TFT('A',50);//����TFTAʶ��
		//task_RFID();
		//Send_Debug_num2(RC_Card_final_P1P2(MasterCar_GoMpValue,2250,1,2));
		break;
	case 0x03:
		//getRightMP();
		//RC_Card_ALLFun1(MasterCar_GoMpValue,2250*2,RC_Get_address(6,1),K_A);
		//Send_Debug_num(RC_write(1,K_A,"-F2-D2-D4-B4-B7-"));
		//OFlag_DX_statusGet();
		
		//DebugKEY=!DebugKEY;
		//OWiFi_TFT('B',40);//����TFTBʶ��
		//Send_Debug_string(OWifi_TFTCP);
		//OFlag_SlaveRun();
		//task_wait();
		/*OWiFi_TFT('B',30);
		Send_Debug_string(OWifi_TFTCP);*/
		break;
	case 0x04:
		//getStopGoMP();
		OFlag_SlaveRun_wait(10);
		//Send_Debug_num2 (CRC24_BLE("SH20",4));
		//RC_write(41,K_A,"A5#3");
		
		/*Send_Debug_HEX(OWifi_TFTShape[0]);
		Send_Debug_HEX(OWifi_TFTShape[1]);
		Send_Debug_HEX(OWifi_TFTShape[2]);
		Send_Debug_HEX(OWifi_TFTShape[3]);
		Send_Debug_HEX(OWifi_TFTShape[4]);
		Send_Debug_HEX(OWifi_TFTShape[5]);*/
		break;

	case 0x05:
		//MasterCar_Right(MasterCar_TrunSpeed, test_buffer);
		break;
	case 0x06: //�ȴ�������������ָ��
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
	PID_Set(25, 0, 300);
	/*����1*/
	OFlag_LED_show(2,0x20,0x21,0x12);//LED��2����ʾ����
	delay_ms(100);
	OFlag_LED_time(2);	//LED��ʾ����
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1);	//LED��ʾ����ʱ����
	OFlag_LED_time(1);
	delay_ms(200);


	/*D7��D6*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	OWiFi_JTlight('B');//��ͨ��Ĭ�ϵȴ�20��
	/*D6��F6*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_LeftMP(MasterCar_TrunSpeed,MasterCar_LeftMPV_45);
	if(OWiFi_TFT('A',35))//����TFTAʶ��
	{
		OFlag_TFT_showHEX('A', 0xA,OWifi_TFTShape[0], 0XB,OWifi_TFTShape[1],0XC, OWifi_TFTShape[4]);
		MO1=OWifi_TFTJTFLAG;//������ʵ�ʱ�Ų�����
		/*
		ֱ�б�� 0x01 ��ת��� 0x02
		��ת��� 0x03 ��ͷ��� 0x04
		��ֱֹ�б�� 0x05 ��ֹͨ�б�� 0x06
		*/
	}
	MasterCar_LeftMP(MasterCar_TrunSpeed,MasterCar_LeftMPV_45);
	OFlag_ETC_wait();
}
void task_second(void)
{
	uint8_t dtbuf=0,CPpark[6]={0,0,0,0,0,0};
	uint32_t calcdata=0;
	uint8_t openCode[6];
	/*F6��F4*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,600);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	MO3=TaskBoard_WAVE();//���������
	OFlag_LED_jl( MO3 );//LED��ʾ����ģʽ

	if(MO3>350)//��ֹ����̫Զ�޷�ɨ���ά��
	{
		dtbuf=1;
		MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
		if(OWiFi_QRCode1(15))//����̬��־��1ʶ�� ��������OWifi_CRCode
		{
			MO2=OWifi_CRCode;
		}
	}else
	{
		dtbuf=0;
		if(OWiFi_QRCode1(15))//����̬��־��1ʶ�� ��������OWifi_CRCode
		{
			MO2=OWifi_CRCode;
		}
	}
	calcdata=CRC24_BLE(MO2,4);//�㷨������̨����
	Send_Debug_num2(calcdata);
	openCode[0]=calcdata/(16*16*16*16*16);
	openCode[1]=(calcdata/(16*16*16*16))%16;
	openCode[2]=(calcdata/(16*16*16))%16;
	openCode[3]=(calcdata/(16*16))%16;
	openCode[4]=(calcdata/16)%16;
	openCode[5]=calcdata%16;
	OFlag_SlaveSendZigbee(0x0E,openCode[0],openCode[1],openCode[2]);//To���� ���̨������1
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0F,openCode[3],openCode[4],openCode[5]);//To���� ���̨������2
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0E,openCode[0],openCode[1],openCode[2]);//To���� ���̨������1
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0F,openCode[3],openCode[4],openCode[5]);//To���� ���̨������2

	if (dtbuf==0)//��Ϊ������ɨ��ά�����������ǰ���ڹ���
	{
		MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);

	}
	delay_ms(100);
	MasterCar_Left(MasterCar_TrunSpeed,1);

	DebugKEY=1;
	OFlag_DZ_wait(5,"");
#if 0
	/*F4��F2*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,1800);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	OFlag_light2((MO1*111)%4+1);//·�Ƶ��ڵ�ָ����λ����ȡ��ʼ��λ

	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,300);

	MasterCar_SmartRunMP(MasterCar_GoSpeed,4800);
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_Left(MasterCar_TrunSpeed,1);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,1800);
#else
	task_RFID();
#endif
	MasterCar_SmartRunMP(MasterCar_GoSpeed,500);

	MasterCar_Right(MasterCar_TrunSpeed,1);
	BKRC_Voice_Extern(0,3);//С������ʶ��3��
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	OWiFi_Send(0XB9,CPpark[0],CPpark[1],CPpark[2]);//�ϴ�����������
	OWiFi_Send(0XBA,CPpark[3],CPpark[4],CPpark[5]);
	if(OWiFi_TFT('B',40))//����TFTBʶ��
	{
		MO4=OWifi_TFTCP;
	}

	MasterCar_Right(MasterCar_TrunSpeed,1);
	//������ʾ
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	MasterCar_Right(MasterCar_TrunSpeed,1);



	/*B4��B6*/
	/*��ǰС����ͷ����:��  ��Ҫ��ʻ����:��*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,650);
	MasterCar_Right(MasterCar_TrunSpeed,3);
	OFlag_SlaveRun();
	OFlag_SlaveRun_wait(10);
}

/**
 * @description: **********************�ȴ�����ָ��*************************
 * @param {*}
 * @return {*}
 */
void task_wait(void)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Left(MasterCar_TrunSpeed,1);	
	MasterCar_BackEnter(1800);
	MasterCar_DebugKEY();
	OFlag_CK_cmd('B',MO6);//����A�����1��

	OFlag_LED_time(0); //LED����ʱ�ر�
	delay_ms(200);
	OFlag_LED_time(0);
	delay_ms(200);
	OFlag_LED_time(0);

	OFlag_WX_open();
	delay_ms(200);
	OFlag_WX_open();
	delay_ms(200);
	OFlag_WX_open();

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
	if (RC_Card_oneFunc4(MasterCar_GoMpValue,2250,1,K_A)==2)
	{
		Send_Debug_string2(RC_Get_buffer());
	}
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
	uint8_t cardbuffer[10][99],success_count=0;
	type_CardRBuf  type_cardRBuf;
	type_cardRBuf.total_count=0;//��¼��ȡ����
    type_cardRBuf.success_count=0;//��¼�ɹ�����


	DebugKEY=1;


	type_cardRBuf=RC_Card_ALLFun1(MasterCar_GoMpValue,1800,1,K_A,1);
	if(type_cardRBuf.success_count>0)
	{
		sprintf(cardbuffer[0],"%s",RC_Get_bufferN(0));
	}
	MasterCar_BackMP(MasterCar_GoSpeed,300);
	MasterCar_SmartRun3(MasterCar_GoSpeed);
	MasterCar_DebugKEY();
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_DebugKEY();
	MasterCar_Right(MasterCar_TrunSpeed,1);
	OFlag_light2((MO1*111)%4+1);//·�Ƶ��ڵ�ָ����λ����ȡ��ʼ��λ
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	PID_Set_recovery();
	
	success_count=type_cardRBuf.success_count;//��һ�ζ����ɹ��ĸ���
	if (RC_Card_oneFunc4(MasterCar_GoMpValue,2250,10,K_A)==2)
	{
		sprintf(cardbuffer[1],"%s",RC_Get_buffer());
		success_count+=1;//��һ�ζ����ɹ��ĸ���
	}
	
	MasterCar_BackMP(MasterCar_GoSpeed+10,300);
	
	if(RC_Card_ALLFun1(-300,MasterCar_GoMpValue,10,K_A,1).success_position>0)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//��2�ζ����ɹ��ĸ���
	}

	if (RC_Card_oneFunc4(MasterCar_GoMpValue,2250,10,K_A)==2)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//��3�ζ����ɹ��ĸ���
	}
	MasterCar_BackMP(MasterCar_GoSpeed,300);
	type_cardRBuf.success_count=0;
	type_cardRBuf=RC_Card_ALLFun1(0,MasterCar_GoMpValue+300,10,K_A,1);
	if (type_cardRBuf.success_count>0)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//��4�ζ����ɹ��ĸ���
	}

	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,600);
	MasterCar_Right(MasterCar_TrunSpeed,3);
	MasterCar_SmartRun3(MasterCar_GoSpeed);
	MasterCar_BackMP(MasterCar_GoSpeed,600);
	MasterCar_SmartRun3(MasterCar_GoSpeed);
	PID_Set_recovery();
	if (RC_Card_oneFunc4(0,1800,10,K_A)==2)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//��5�ζ����ɹ��ĸ���
	}
	
	for (int i = 0; i < 5; i++)
	{
		Send_Debug_string2(cardbuffer[i]);
	}
	

}
