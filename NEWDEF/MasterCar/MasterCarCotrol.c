/*
 * @Description:  主车操控
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 20:00:52
 * 
 * 短线1800 长线2250
 */
#include "sys.h"
#include "MasterCarCotrol.h"
#include "power_check.h"
#include "CanP_HostCom.h"
#include "can_user.h" //Zigbee_Rx_flag
#include "delay.h"
#include "drive.h"
#include "ultrasonic.h"
#include "bh1750.h"		   //读取光照度
#include "PID.h"		   //PID循迹
#include "Timer.h"		   //定时器
#include "roadway_check.h" //码盘同步
#include "infrared.h"	   //红外
//#include "Card.h"		   //RFID例程
//#include "rc522.h"		   //RFID
#include "RFIDCARD.h"
#include "OperationFlag.h"
#include "zigbee.h"	 //wifi接收数据
#include "syn7318.h" //syn7318语音识别
#include "OperationWiFi.h"
#include "MyString.h"
#include "TaskBoard.h" //任务版
#include "math.h"
#include "stdio.h"
#include "cba.h"		//蜂鸣器
#include "bkrc_voice.h" //小创语音
#include "CRC.h"

uint8_t debugbuf[99]; //缓存
uint8_t DebugKEY = 0; //0.关闭DebugKEY 1.开启

//unsigned char _Flag_Task = 0;  //任务标志
unsigned int MasterCar_GoSpeed = 40;   //主车默认循迹运行速度
unsigned int MasterCar_TrunSpeed = 90; //主车转弯速度
#define MasterCar_GoMpValue 340		   //移动时拐弯前的前进码盘值
#define MasterCar_GoMpValue2 365	   //停车在启动的前进码盘值
#define MasterCar_LeftMPV_45 407	   //410 //码盘左转45度
#define MasterCar_RightMPV_45 460	   //450 //码盘右转45度
#define MasterCar_LeftMPV_90 MasterCar_LeftMPV_45 * 2
#define MasterCar_RightMPV_90 MasterCar_RightMPV_45 * 2
#define MasterCar_LeftMPV_180 MasterCar_LeftMPV_45 * 4
#define MasterCar_RightMPV_180 MasterCar_RightMPV_45 * 4
/*主车利用循迹灯转弯，加上MP限制值 防止小车转弯跑飞 由于左右轮子存在偏差所以分别定义*/
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
 * @description: 停车并等待S2键在进行下一步操作
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
 * @description: 主车前进
 * @param {unsigned int} speed 前进速度 0~100
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
 * @description: 主车前进2（白卡十字路口可停止）
 * @param {unsigned int} speed 前进速度 0~100
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
 * @description: 主车前进码盘值
 * 参考值:
 * 1300(特殊地形标志物刚好循迹灯过白色线)
 * 1850-1900(特殊地形标志物刚好车位过线过白色线)
 * @param {unsigned int} speed 前进速度 0~100
 * @param {unsigned int} mp 欲前进码盘值
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
 * @description: 主车后退
 * @param {unsigned int} speed 后退速度 0~100
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
 * @description: 主车后退码盘值
 * @param {unsigned int} speed 后退速度 0~100
 * @param {unsigned int} mp 欲后退码盘值
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
 * @description: 主车左转码盘值+循迹灯判断 
 * @param {unsigned int} speed 后退速度 0~100
 * @param {unsigned int} mode 1.十字路口90度 2.直线180度 3.十字路口180度
 * @return {*}
 */
void MasterCar_Left(unsigned int speed, unsigned int mode) //适用于90度
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
		//码盘
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
			else if (_mp > LeftMP_180_limit) //防止监测失败
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
			else if (_mp > LeftMP_45_limit) //防止监测失败
			{
				Send_UpMotor(0, 0);
				_break = 0;
				continue;
			}
		}
		break;
	case 5:
		//码盘+循迹灯
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
			else if (_mp > LeftMP_90_limit) //防止监测失败
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
 * @description: 主车右转码盘值+循迹灯判断  
 * @param {unsigned int} speed 右转速度 0~100
 * @param {unsigned int} mode 1.十字路口90度 2.直线180度 3.十字路口180度 4.45度循迹灯
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
	case 1: //码盘拐弯
		while (Roadway_mp_Get() < MasterCar_RightMPV_90)
			;
		Send_UpMotor(0, 0);
		break;
	case 3: //180度循迹灯拐弯
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
	case 4: //45度拐回轨道
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
	case 5: //九十度码盘拐弯
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
			else if (_mp > RightMP_90_limit) //防止监测失败
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
 * @description: 左转 码盘版
 * @param {unsigned int} speed 速度
 * @param {unsigned int} mp	码盘
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
 * @description: 右转 码盘版
 * @param {unsigned int} speed 速度
 * @param {unsigned int} mp	码盘
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
 * @description: 主车暂停
 * @param {*}
 * @return {*}
 */
void MasterCar_Stop(void)
{
	Send_UpMotor(0, 0);
	delay_ms(100);
}

/**
 * @description: 主车转弯倒车入库 
 * @param {unsigned int} mp 从调整车身位到车库的mp距离  参考值1800
 * @return {*}
 */
void MasterCar_BackEnter(unsigned int mp)
{
	PID_Set(25, 0, 300);
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 900); //往前调正车身
	MasterCar_BackMP(MasterCar_GoSpeed, 900);	   //往前调正车身
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 500); //往前调正车身
	MasterCar_BackMP(MasterCar_GoSpeed, mp);	   //倒车
	MasterCar_Stop();
	PID_Set_recovery();
}

/**
 * @description: 主车循迹运行 灭灯3个以上、全亮、全灭 停止循迹（寻卡循迹）用于十字路口存在白卡 注意接近了在使用 防止循迹灯误判
 * 可以配合MasterCar_SmartRunMP();使用 防止十字路口被磨白造成误判
 * @param {*}
 * @return {*}
 */

void MasterCar_SmartRun2(unsigned int speed)
{
	uint8_t _speed; //缓存MasterCar_GoSpeed 循迹完成恢复
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //重置PID叠加参数
	while (PID_Track5(speed) != 99)
	{
		delay_ms(1); //防止刷新太快循迹错乱
	}
	//等待循迹完成
	MasterCar_GoSpeed = _speed; //恢复速度
	PID_Set_recovery();			//恢复PID参数
}

/**
 * @description: 主车循迹运行（全黑线或白卡处于十字黑线处停车 注：白卡长的一面平行于行走的黑线 用于白卡处于黑线十字路口时 停车调正方向）
 * @param {unsigned int} speed 运行速度 0~100
 * @return {*}
 */
void MasterCar_SmartRun1(unsigned int speed)
{
	uint8_t _speed; //缓存MasterCar_GoSpeed 循迹完成恢复
	MasterCar_Go(speed);
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //重置PID叠加参数
	while (PID_Track(speed) != 99)
	{
		delay_ms(1); //防止刷新太快循迹错乱
	}
	//等待循迹完成
	MasterCar_GoSpeed = _speed; //恢复速度
	PID_Set_recovery();			//恢复PID参数
}

/**
 * @description: 主车循迹运行 普通循迹函数
 * @param {unsigned int} speed
 * @return {*}
 */
void MasterCar_SmartRun(unsigned int speed)
{
	uint8_t _speed; //缓存MasterCar_GoSpeed 循迹完成恢复
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;

	PID_reset(); //重置PID叠加参数
	while (PID_Track2(speed) != 99)
	{
		delay_ms(1); //防止刷新太快循迹错乱
	}
	//等待循迹完成
	MasterCar_GoSpeed = _speed; //恢复速度
	PID_Set_recovery();			//恢复PID参数
}

/**
 * @description: 主车循迹运行 码盘版
 * @param {unsigned int} speed 速度
 * @param {unsigned int} speed 码盘值
 * @return {*}
 */
void MasterCar_SmartRunMP(unsigned int speed, unsigned int MP)
{

	int _mp = 0;
	uint8_t _speed; //缓存MasterCar_GoSpeed 循迹完成恢复
	delay_ms(100);	//这里不加延迟主车前进码盘不稳定
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;
	MasterCar_Go(speed);
	//_Flag_Track = 1;

	PID_reset(); //重置PID叠加参数

	while (_mp <= MP)
	{
		_mp = Roadway_mp_Get();
		PID_TrackMP(speed);
		if (_mp >= MP)
		{
			Send_UpMotor(0, 0);
			break;
		}
		delay_ms(1); //防止刷新太快循迹错乱

	} //等待循迹完成
	Send_UpMotor(0, 0);
	MasterCar_GoSpeed = _speed;
	delay_ms(100);
}

/**
 * @description: 主车循迹运行 码盘版 暴力校准车身
 * @param {unsigned int} speed 速度
 * @param {unsigned int} speed 码盘值
 * @return {*}
 */
void MasterCar_SmartRunMP2(unsigned int speed, unsigned int MP)
{

	int _mp = 0;
	uint8_t _speed; //缓存MasterCar_GoSpeed 循迹完成恢复
	delay_ms(100);	//这里不加延迟主车前进码盘不稳定
	Roadway_mp_syn();
	_mp = Roadway_mp_Get();
	_speed = MasterCar_GoSpeed;
	MasterCar_GoSpeed = speed;
	MasterCar_Go(speed);
	//_Flag_Track = 1;

	PID_reset(); //重置PID叠加参数

	while (_mp <= MP)
	{
		_mp = Roadway_mp_Get();
		PID_TrackMP2(speed);
		if (_mp >= MP)
		{
			Send_UpMotor(0, 0);
			break;
		}
		delay_ms(1); //防止刷新太快循迹错乱

	} //等待循迹完成
	Send_UpMotor(0, 0);
	MasterCar_GoSpeed = _speed;
	delay_ms(100);
}

//测试读码盘
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
uint8_t Zigbee_receive_alarm[7] = {'1', '2', '3', '4', '5', '6', '\0'}; //接收烽火台开启码
uint8_t Zigbee_receive_CarPlate[7] = {'A', '9', '9', '9', '9', '9','\0'};	//接收副车车牌数据
uint8_t Zigbee_receive_RFIDKey[6];								//接收RFID密钥
//uint8_t Wifi_receive_data[6] = {0, 0, 0, 0, 0, 0};						//wifi接收数据存放缓冲区
/**
 * @description: 主车接收Wifi Zigbee处理线程
 * 在主车无操作下或与安卓端通信时，自动启用该线程
 * Zigbee_Rx_flag & Wifi_Rx_flag接收完毕需置0
 * @param {*}
 * @return {*}
 */
int OFlag_SLAVEtask;
void MasterCar_TaskReceiveThread(void)
{
	if (OFlag_SLAVEflag && TaskBoard_TimeStatus() == 0) //等待副车防超时
	{
		Send_Debug_string2("time out");
		OFlag_SLAVEtask=OFlag_SLAVEflag;
		OFlag_SLAVEflag = 0;
		TaskBoard_TimeClose(); //关闭倒计时检测
		switch (OFlag_SLAVEtask)
		{
		case 0x01:
			task_wait();		   //启动等待副车任务
			break;
		case 0x02:
			task_wait2();		   //启动等待副车任务
			break;
		default:
			
			break;
		}
		
	}

	/*wifi数据*/
	if (Wifi_Rx_flag)
	{
		if (OWiFi_toZigbee() == 1) //WIFI的Zigbee请求
		{						   /*
			if (zNum_TFT_A == Wifi_Rx_Buf[1] && 0x10 == Wifi_Rx_Buf[2])
			{
				Send_Debug_num2(Wifi_Rx_Buf[3]);
			}*/

			//Agreement_Send(2, Wifi_Rx_Buf); //Zigbee通讯
		} // wifi转发至zigbee
		else
		{

			Operation_WiFi_Master(Wifi_Rx_Buf); //WIFI的主车操作请求
		}

		Wifi_Rx_flag = 0;
	}

	//zigbee数据
	if (Zigbee_Rx_flag && !(TIM4->CR1 & 0x01)) //防止与TIM4除了zigbee数据冲突
	{
		switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee节点反馈选择
		{
		case 0x00: //校验失败 通讯容易丢包
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
		case zNum_MasterCar: //来自从车向主车的指令
			SlaveCar_TaskRunThread(Zigb_Rx_Buf);
			break;

		case zNum_CK_A: //车库A
			break;
		case zNum_CK_B: //车库B
			break;
		case zNum_DZ: //道闸
			break;
		case zNum_JT_A: //智能交通灯A
			break;
		case zNum_JT_B: //智能交通灯B
			break;

		case zNum_ETC: //ETC
			if (OFlag_ETCflag == 0xf1)
			{
				OFlag_ETCflag = 0;
				task_ETC();
			}
			break;
		case zNum_DX: //特殊地形
			break;
		case zNum_YY:
			break;
		default: //未定义zigbee编号
			Send_Debug_string("undefine\n");
			break;
		}

		Zigbee_Rx_flag = 0;
	}
}

/**
 * @description: 主车处理Zigbee接收副车控制指令
 * @param {unsigned char*} id 数据帧
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
			TaskBoard_TimeClose(); // 关闭倒计时检测
			Send_Debug_string2("slave...");
			task_wait(); //启动主车指令 taskID根据需求修改
			
			break;
		case 0x02:
			OFlag_SLAVEflag = 0;
			TaskBoard_TimeClose(); // 关闭倒计时检测
			Send_Debug_string2("slave2...");
			task_wait2(); //启动主车指令 taskID根据需求修改
			
			break;
		default:
			break;
		} 

		break;
	case 0x03:
		//Send_Debug_string("dz\n");
		OFlag_DZ_open("");
		//OFlag_DZ_open(Master_CarLicensePlate); //打开道闸
		break;
	case 0x0A:
		Zigbee_receive_RFIDKey[0] = OFlag_GetCmd1(data);
		Zigbee_receive_RFIDKey[1] = OFlag_GetCmd2(data);
		Zigbee_receive_RFIDKey[2] = OFlag_GetCmd3(data);
		//RFID密钥1
		break;
	case 0x0B:
		Zigbee_receive_RFIDKey[3] = OFlag_GetCmd1(data);
		Zigbee_receive_RFIDKey[4] = OFlag_GetCmd2(data);
		Zigbee_receive_RFIDKey[5] = OFlag_GetCmd3(data);
		//RFID密钥2
		break;
	case 0x0C:
		Zigbee_receive_CarPlate[0] = OFlag_GetCmd1(data);
		Zigbee_receive_CarPlate[1] = OFlag_GetCmd2(data);
		Zigbee_receive_CarPlate[2] = OFlag_GetCmd3(data);
		//车牌数据1
		break;
	case 0x0D:
		Zigbee_receive_CarPlate[3] = OFlag_GetCmd1(data);
		Zigbee_receive_CarPlate[4] = OFlag_GetCmd2(data);
		Zigbee_receive_CarPlate[5] = OFlag_GetCmd3(data);
		//车牌数据2
		break;
	case 0x08:
		Zigbee_receive_alarm[0] = OFlag_GetCmd1(data);
		Zigbee_receive_alarm[1] = OFlag_GetCmd2(data);
		Zigbee_receive_alarm[2] = OFlag_GetCmd3(data);
		//烽火台数据1
		break;
	case 0x09:
		Zigbee_receive_alarm[3] = OFlag_GetCmd1(data);
		Zigbee_receive_alarm[4] = OFlag_GetCmd2(data);
		Zigbee_receive_alarm[5] = OFlag_GetCmd3(data);
		//烽火台数据2
		break;
	case 0xB0: //路灯初始档位
		Zigbee_receive_light = OFlag_GetCmd1(data);
		/*MO5=Zigb_Rx_Buf[3];
		Send_Debug_string("MO5=");
		Send_Debug_num(MO5);
		Send_Debug_string("\n");*/
	case 0xFE: // 接收副车发送的数值
		if (Zigbee_Number_len > OFlag_GetCmd1(data))//防止数组越界
		{
			Zigbee_receive_Number[OFlag_GetCmd1(data)] = OFlag_GetCmd2(data);
		}
		break;
	default:
		break;
	}
}

/*
福建省省赛过特殊弯道
MasterCar_SmartRun(MasterCar_GoSpeed);
MasterCar_Stop();
MasterCar_BackMP(MasterCar_GoSpeed,250);
delay_ms(200);
MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45/2);
MasterCar_GoMP(MasterCar_GoSpeed,700);
MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45/2+MasterCar_RightMPV_45);
*/

/******************************信息编号********************************/
#define e 2.71828
#define PI 3.14159

char alarmKey[] = {'1', '2', '3', '4', '5', '6'};

/**
 * @description: 主车任务运行选择 
 * 设定主车当前运行任务
 * @param {*}
 * @return {*}
 */

int testp = 15;
int testi;
static char *slave_p = NULL; // 从车的坐标
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
		if (OWiFi_TFT('A', 60)) //请求TFTA识别
		{

			//图像颜色个数
			Send_Debug_num2(OWifi_TFTShape[0]);
			Send_Debug_num2(OWifi_TFTShape[1]);
			Send_Debug_num2(OWifi_TFTShape[2]);

			Send_Debug_string2("JTFlag:");
			Send_Debug_num2(OWifi_TFTJTFLAG);
			OFlag_TFT_showHEX('A', 0xF, OWifi_TFTShape[0], 0xF, OWifi_TFTShape[1], 0xF, OWifi_TFTShape[2]); //HEX显示
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

		//OFlag_alarm_change("123456");//烽火台修改开启码

		//Send_Debug_num( OFlag_alarm_getP(10 ));//烽火台获取随机救援坐标

		break;
	case 0x05:
		//MasterCar_Right(MasterCar_TrunSpeed, test_buffer);
		break;
	case 0x06: //等待副车发送启动指令
		//task_wait();
		break;
	case 0x07: //等待ETC
		/*
		if(OFlag_ETCflag)
		{
			task_ETC();
			OFlag_ETCflag=0;
		}*/
		break;
	case 0x08:
		task_waitWifi(); //等待wifi
		break;
	case 0x09:
		break;

	default:

		break;
	}
}

int carRead = 0;	 //判断是否成功读卡
uint8_t carData[16]; //正确的卡数据
uint32_t jl = 0;	 //超声波测距的值

uint8_t buf[50];
/**
 * @description:************************初始任务********************************
 * @param {*}
 * @return {*}
 */
#define HIDE 0
void task_first(void)
{
	Send_Debug_string2("task_first");
	default_PID();

	/*任务1*/
	delay_ms(100);
	OFlag_LED_time(2); //LED显示清零
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1); //LED显示倒计时开启
	OFlag_LED_time(1);
	delay_ms(200);

	/*D7→D6*/
	/*当前小车车头方向:北  将要行驶方向:北*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	/*D6→F6*/
	/*当前小车车头方向:北  将要行驶方向:东*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);

	MasterCar_Right(MasterCar_TrunSpeed, 1);
	MasterCar_BackMP(MasterCar_GoSpeed, 800);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();

	jl = TaskBoard_WAVE(); //超声波测距
	OFlag_TFT_jl('B', jl); //TFTB显示



	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue2);
	MasterCar_Left(MasterCar_TrunSpeed, 1);
#if HIDE
	delay_ms(500);
	delay_ms(500);
#else
	int mb = ((jl / 100) % 4) + 1;
	int cs = OFlag_light2(mb);//路灯调节到1档并获取初始档位

#endif

	/*F6→F4*/
	/*当前小车车头方向:东  将要行驶方向:北*/
	MasterCar_Left(MasterCar_TrunSpeed, 1);

	MasterCar_SmartRunMP(MasterCar_GoSpeed, 1000);

#if 1
	RC_Card_checkRangeReadPlus2(MasterCar_GoMpValue, 1000, RC_Get_address(2, 2), K_A, 1, 0); //在指定距离内读卡循迹灯版
	MasterCar_BackMP(MasterCar_GoSpeed, 700);
	MasterCar_SmartRun2(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
#else
	RC_Card_checkRangeReadPlus2(0, 800, RC_Get_address(2, 2), K_A, 1, 0); //在指定距离内读卡循迹灯版
#endif

	MasterCar_Right(MasterCar_TrunSpeed, 1);

#if HIDE
	delay_ms(500);
	delay_ms(500);
#else
	Send_Debug_string("YY:");
	Send_Debug_num2(BKRC_Voice_Extern(0, 3)); //小创语音识别3次
#endif
	MasterCar_Left(MasterCar_TrunSpeed, 1);
	/*F4→D4*/
	/*当前小车车头方向:北  将要行驶方向:西*/
	MasterCar_Left(MasterCar_TrunSpeed, 1);

	OFlag_ETC_wait(); //等待ETC开启

	/*

	MasterCar_BackEnter(1300);

	OFlag_LED_time(0);	//LED倒计时关闭
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
 * @description: **********************第二段任务*************************
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
	/*B2→D2*/
	/*当前小车车头方向:北  将要行驶方向:东*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	OFlag_DX_carGo(35, 1250);

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);

	OFlag_SlaveRun_wait_flag( 20 ,0x02);//等待副车 50s超时
	
}

/**
 * @description: **********************等待副车指令*************************
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

	/*B4→B2*/
	/*当前小车车头方向:西  将要行驶方向:北*/
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	task_second();
}

/**
 * @description: **********************等待副车指令*************************
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
	OFlag_alarm_open(Zigbee_receive_alarm); // 发送开启码开启烽火台
	delay_ms(200);
	OFlag_alarm_open(Zigbee_receive_alarm); // 发送开启码开启烽火台
	delay_ms(200);
	OFlag_alarm_open(Zigbee_receive_alarm); // 发送开启码开启烽火台
	delay_ms(200);
	MasterCar_RightMP(MasterCar_TrunSpeed, MasterCar_RightMPV_45);

	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed, MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed, 1);

	MasterCar_BackEnter(1500);

	OFlag_LED_time(0); //LED倒计时关闭
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
 * @description: ************************测试任务**************************
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
	MasterCar_BackMP(MasterCar_GoSpeed,370);//直接卡在黑线上

	PID_Set(25, 0, 300); //上地形前快速调整车身
	#if HIDE
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	#else
	//测距
	#endif

	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,600);
	PID_Set_recovery();


	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);

	OFlag_DX_carGo(36,1250);//过路障
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
}

/**
 * @description:  ************************ETC等待任务**************************
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

	OFlag_LED_time(0);	//LED倒计时关闭
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
 * @description:  ************************wifi启动主车指令**************************
 * @param {*}
 * @return {*}
 */
void task_waitWifi(void)
{
	task_first();
	//task_wait();
}

/**
 * @description: ************************读RFID卡任务**************************
 * @param {*}
 * @return {*}
 */
void task_RFID(void)
{
	int i = 0, carId = 0, carOK = 0, S = 5, B = 3, card_flag = 0;
	static char *path,buf[50];
	RC_Card_checkRangeReadPlus2(0, 2250 * 2, RC_Get_address(2, 2), K_A, 2, 1); //在指定距离内读卡循迹灯版

	for ( i = 0; i < 3; i++)
	{
		if(RC_Get_bufferN(i)!=NULL)
		{
			path = get_path_one(RC_Get_bufferN(i),"B4","B6",buf);
			if(path!=NULL)
			{
				Send_Debug_string2(path);
				Send_Debug_string2(buf);
				
				if (slave_p == NULL) // 若以及设置初始坐标 则不再使用读取坐标
				{
					slave_p = path;
				}
				//发送给副车自由路径
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

	OFlag_SlaveRun();		 //启动副车
	OFlag_SlaveRun_wait(60); //等待副车 50s超时
}

/*
?? ?? //直行右转直接过路障
?? ?? MasterCar_SmartRun2(MasterCar_GoSpeed);//卡在十字路口用这个 否则用MasterCar_SmartRun(MasterCar_GoSpeed);
?? ?? MasterCar_SmartRunMP(MasterCar_GoSpeed,360);//特殊设置值
?? ?? MasterCar_Right(MasterCar_TrunSpeed,1);
?? ?? MasterCar_SmartRun2(MasterCar_GoSpeed);
?? ?? MasterCar_BackMP(MasterCar_GoSpeed,500);
?? ?? OFlag_DX_carGo(36,1250);//过路障
??*/