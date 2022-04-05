/*
 * @Description:  主车操控
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-05 10:01:13
 * 
 * 短线1800 长线2250
 */

#include "MasterCarCotrol.h"
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
#include "TaskBoard.h"//任务版
#include "math.h"
#include "stdio.h"
#include "cba.h"//蜂鸣器
#include "bkrc_voice.h"//小创语音
#include "CRC.h"
unsigned char _Flag_Task = 0;  //任务标志
unsigned int MasterCar_GoSpeed = 40;	//主车默认循迹运行速度
unsigned int MasterCar_TrunSpeed = 90;	//主车转弯速度
#define MasterCar_GoMpValue     280 //移动时拐弯前的前进码盘值
#define MasterCar_GoMpValue2     360 //停车在启动的前进码盘值
#define MasterCar_LeftMPV_45    410 //码盘左转45度
#define MasterCar_RightMPV_45   450 //码盘右转45度
#define MasterCar_LeftMPV_90    MasterCar_LeftMPV_45*2 
#define MasterCar_RightMPV_90   MasterCar_RightMPV_45*2
#define MasterCar_LeftMPV_180   MasterCar_LeftMPV_45*4
#define MasterCar_RightMPV_180  MasterCar_RightMPV_45*4
/*主车利用循迹灯转弯，加上MP限制值 防止小车转弯跑飞 由于左右轮子存在偏差所以分别定义*/
#define LeftMP_45_limit MasterCar_LeftMPV_45+50
#define RightMP_45_limit MasterCar_RightMPV_45+50
#define LeftMP_90_limit MasterCar_LeftMPV_90+50
#define RightMP_90_limit MasterCar_RightMPV_90+50
#define LeftMP_180_limit MasterCar_LeftMPV_180+50
#define RightMP_180_limit MasterCar_RightMPV_180+50


uint8_t debugbuf[99];//缓存
uint8_t DebugKEY=0; //0.关闭DebugKEY 1.开启
/*flag操作*/
void CleanFalgAll(void)
{
	_Flag_Task = 0;
}
unsigned char Get_Flag_Task(void) { return _Flag_Task; }
void Set_Flag_Task(unsigned int task) { _Flag_Task = task; }

/**
 * @description: 停车并等待S2键在进行下一步操作
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
	delay_ms(150);
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
	case 1://码盘拐弯
		while (Roadway_mp_Get()<MasterCar_RightMPV_90);
		Send_UpMotor(0, 0);
		break;
	case 3://180度循迹灯拐弯
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
	case 4://45度拐回轨道
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
	case 5://九十度码盘拐弯
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
	CleanFalgAll();
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
	MasterCar_BackMP(MasterCar_GoSpeed, 900); //往前调正车身
	MasterCar_SmartRunMP2(MasterCar_GoSpeed, 500); //往前调正车身
	MasterCar_BackMP(MasterCar_GoSpeed, mp);	  //倒车
	MasterCar_Stop();
	PID_Set_recovery();
}

//主车循迹运行 灭灯3个以上、全亮、全灭 停止循迹（寻卡循迹）
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

//主车循迹全黑线停止循迹
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
	delay_ms(100);//这里不加延迟主车前进码盘不稳定
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
	delay_ms(100);//这里不加延迟主车前进码盘不稳定
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












uint8_t Zigbee_receive_CarPlate[6] = {'A', '9', '9', '9', '9', '9'}; //接收副车车牌数据
unsigned int Zigbee_receive_RFIDKey[6];								 //接收RFID密钥
uint8_t Wifi_receive_data[6] = {0, 0, 0, 0, 0, 0};					 //wifi接收数据存放缓冲区
/**
 * @description: 主车接收Wifi Zigbee处理线程
 * 在主车无操作下或与安卓端通信时，自动启用该线程
 * Zigbee_Rx_flag & Wifi_Rx_flag接收完毕需置0
 * @param {*}
 * @return {*}
 */
void MasterCar_TaskReceiveThread(void)
{
	if (OFlag_SLAVEflag && TaskBoard_TimeStatus()==0)//等待副车防超时
	{
		OFlag_SLAVEflag=0;
		TaskBoard_TimeClose();//关闭倒计时检测
		Send_Debug_string2("T->go");
		task_wait();//启动等待副车任务
	}
	
	/*wifi数据*/
	if (Wifi_Rx_flag)
	{
		if (OWiFi_toZigbee() == 1) //WIFI的Zigbee请求
		{
			//Agreement_Send(2, Wifi_Rx_Buf); //Zigbee通讯
		} // wifi转发至zigbee
		else
		{
			Operation_WiFi_Master(Wifi_Rx_Buf); //WIFI的主车操作请求
		}

		Wifi_Rx_flag = 0;
	}

	//zigbee数据
	if (Zigbee_Rx_flag)
	{
		switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee节点反馈选择
		{

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
			if(OFlag_ETCflag)
			{
				OFlag_ETCflag=0;
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
		if(OFlag_SLAVEflag)
		{
			OFlag_SLAVEflag=0;
			TaskBoard_TimeClose();//关闭倒计时检测
			Send_Debug_string2("S->go");
			task_wait(); //启动主车指令 taskID根据需求修改
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
	case 0xB0: //路灯初始档位
			   /*MO5=Zigb_Rx_Buf[3];
		Send_Debug_string("MO5=");
		Send_Debug_num(MO5);
		Send_Debug_string("\n");*/
		break;
	default:
		break;
	}
}


























/******************************信息编号********************************/
#define e 2.71828
#define PI 3.14159


/**
 * @description: 主车任务运行选择 
 * 设定主车当前运行任务 通过Set_Flag_Task()选择任务
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
	case 0x06: //等待副车发送启动指令
		Send_Debug_string2("M->go");
		task_wait();
		break;
	case 0x07: //等待ETC
		if(OFlag_ETCflag)
		{
			task_ETC();
			OFlag_ETCflag=0;
		}
		break;
	case 0x08:
		task_waitWifi(); //等待wifi
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
 * @description:************************初始任务********************************
 * @param {*}
 * @return {*}
 */

void task_first(void)
{
	OFlag_resetWaitFlag();
	PID_Set(25, 0, 300);
	
	/*任务1*/
	delay_ms(100);
	OFlag_LED_time(2);	//LED显示清零
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1);	//LED显示倒计时开启
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
	OFlag_LED_jl( TaskBoard_WAVE() );//LED显示距离模式
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_Left(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	OFlag_light2(4);//路灯调节到指定档位并获取初始档位
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue2);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	BKRC_Voice_Extern(0,3);//小创语音识别3次
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	OFlag_ltDisplay_show("A12345","F1");//车牌显示及坐标
	delay_ms(200);
	OFlag_ltDisplay_show("A12345","F1");//车牌显示及坐标
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	task_RFID();
	
}
void task_second(void)
{
	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	OFlag_SlaveRun();//启动副车
	OFlag_SlaveRun_wait(30);
}

/**
 * @description: **********************等待副车指令*************************
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
 * @description: ************************测试任务**************************
 * @param {*}
 * @return {*}
 */
void task_test(void)
{
	
}

/**
 * @description:  ************************ETC等待任务**************************
 * @param {*}
 * @return {*}
 */
void task_ETC(void)
{
	task_second();
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
