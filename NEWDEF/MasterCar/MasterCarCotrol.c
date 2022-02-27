/*
 * @Description:  主车操控
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-22 16:55:48
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

//主车循迹 灭灯3个以上或全亮 停止循迹
void MasterCar_SmartRun3(unsigned int speed)
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
 * @description: 主车循迹运行（初始版本）
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
				task_ETC();
				OFlag_ETCflag=0;
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
		/*if (SLAVE_flag==0)
		{		
			SLAVE_flag=1;//从车启动主车标志
			task_wait(); //启动主车指令 taskID根据需求修改
		}*/
		if(OFlag_SLAVEflag)
		{
			task_wait(); //启动主车指令 taskID根据需求修改
			OFlag_SLAVEflag=0;
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


uint8_t *MO8= "A12345",*MO2,*MO4;
int  MO1 = 2,MO3=0,MO9=4,MO6=3;


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
		//getLeftMP();
		//task_wait();
		task_RFID();
		//task_test();
		//OWiFi_TFT('A',50);//请求TFTA识别
		//task_RFID();
		//Send_Debug_num2(RC_Card_final_P1P2(MasterCar_GoMpValue,2250,1,2));
		break;
	case 0x03:
		//getRightMP();
		//RC_Card_ALLFun1(MasterCar_GoMpValue,2250*2,RC_Get_address(6,1),K_A);
		//Send_Debug_num(RC_write(1,K_A,"-F2-D2-D4-B4-B7-"));
		//OFlag_DX_statusGet();
		
		//DebugKEY=!DebugKEY;
		//OWiFi_TFT('B',40);//请求TFTB识别
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
	case 0x06: //等待副车发送启动指令
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
	PID_Set(25, 0, 300);
	/*任务1*/
	OFlag_LED_show(2,0x20,0x21,0x12);//LED第2排显示数据
	delay_ms(100);
	OFlag_LED_time(2);	//LED显示清零
	OFlag_LED_time(2);
	delay_ms(100);
	OFlag_LED_time(1);	//LED显示倒计时开启
	OFlag_LED_time(1);
	delay_ms(200);


	/*D7→D6*/
	/*当前小车车头方向:北  将要行驶方向:北*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	OWiFi_JTlight('B');//交通灯默认等待20秒
	/*D6→F6*/
	/*当前小车车头方向:北  将要行驶方向:东*/
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_LeftMP(MasterCar_TrunSpeed,MasterCar_LeftMPV_45);
	if(OWiFi_TFT('A',35))//请求TFTA识别
	{
		OFlag_TFT_showHEX('A', 0xA,OWifi_TFTShape[0], 0XB,OWifi_TFTShape[1],0XC, OWifi_TFTShape[4]);
		MO1=OWifi_TFTJTFLAG;//接收于实际编号不符合
		/*
		直行编号 0x01 左转编号 0x02
		右转编号 0x03 掉头编号 0x04
		禁止直行编号 0x05 禁止通行编号 0x06
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
	/*F6→F4*/
	/*当前小车车头方向:东  将要行驶方向:北*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,600);
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_Stop();
	MO3=TaskBoard_WAVE();//超声波测距
	OFlag_LED_jl( MO3 );//LED显示距离模式

	if(MO3>350)//防止距离太远无法扫描二维码
	{
		dtbuf=1;
		MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
		if(OWiFi_QRCode1(15))//请求静态标志物1识别 结果存放于OWifi_CRCode
		{
			MO2=OWifi_CRCode;
		}
	}else
	{
		dtbuf=0;
		if(OWiFi_QRCode1(15))//请求静态标志物1识别 结果存放于OWifi_CRCode
		{
			MO2=OWifi_CRCode;
		}
	}
	calcdata=CRC24_BLE(MO2,4);//算法计算烽火台数据
	Send_Debug_num2(calcdata);
	openCode[0]=calcdata/(16*16*16*16*16);
	openCode[1]=(calcdata/(16*16*16*16))%16;
	openCode[2]=(calcdata/(16*16*16))%16;
	openCode[3]=(calcdata/(16*16))%16;
	openCode[4]=(calcdata/16)%16;
	openCode[5]=calcdata%16;
	OFlag_SlaveSendZigbee(0x0E,openCode[0],openCode[1],openCode[2]);//To副车 烽火台开启码1
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0F,openCode[3],openCode[4],openCode[5]);//To副车 烽火台开启码2
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0E,openCode[0],openCode[1],openCode[2]);//To副车 烽火台开启码1
	delay_ms(100);
	OFlag_SlaveSendZigbee(0x0F,openCode[3],openCode[4],openCode[5]);//To副车 烽火台开启码2

	if (dtbuf==0)//若为近距离扫二维码情况下在往前后在拐弯
	{
		MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);

	}
	delay_ms(100);
	MasterCar_Left(MasterCar_TrunSpeed,1);

	DebugKEY=1;
	OFlag_DZ_wait(5,"");
#if 0
	/*F4→F2*/
	/*当前小车车头方向:北  将要行驶方向:北*/
	
	MasterCar_SmartRunMP(MasterCar_GoSpeed,1800);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	OFlag_light2((MO1*111)%4+1);//路灯调节到指定档位并获取初始档位

	/*当前小车车头方向:北  将要行驶方向:西*/
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_BackMP(MasterCar_GoSpeed,300);

	MasterCar_SmartRunMP(MasterCar_GoSpeed,4800);
	/*当前小车车头方向:北  将要行驶方向:西*/
	MasterCar_Left(MasterCar_TrunSpeed,1);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,1800);
#else
	task_RFID();
#endif
	MasterCar_SmartRunMP(MasterCar_GoSpeed,500);

	MasterCar_Right(MasterCar_TrunSpeed,1);
	BKRC_Voice_Extern(0,3);//小创语音识别3次
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	OWiFi_Send(0XB9,CPpark[0],CPpark[1],CPpark[2]);//上传破损车牌数据
	OWiFi_Send(0XBA,CPpark[3],CPpark[4],CPpark[5]);
	if(OWiFi_TFT('B',40))//请求TFTB识别
	{
		MO4=OWifi_TFTCP;
	}

	MasterCar_Right(MasterCar_TrunSpeed,1);
	//立体显示
	MasterCar_RightMP(MasterCar_TrunSpeed,MasterCar_RightMPV_45);
	MasterCar_Right(MasterCar_TrunSpeed,1);



	/*B4→B6*/
	/*当前小车车头方向:南  将要行驶方向:南*/
	MasterCar_SmartRun(MasterCar_GoSpeed);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,MasterCar_GoMpValue);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_SmartRunMP(MasterCar_GoSpeed,650);
	MasterCar_Right(MasterCar_TrunSpeed,3);
	OFlag_SlaveRun();
	OFlag_SlaveRun_wait(10);
}

/**
 * @description: **********************等待副车指令*************************
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
	OFlag_CK_cmd('B',MO6);//车库A到达第1层

	OFlag_LED_time(0); //LED倒计时关闭
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
 * @description: ************************测试任务**************************
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
	uint8_t cardbuffer[10][99],success_count=0;
	type_CardRBuf  type_cardRBuf;
	type_cardRBuf.total_count=0;//记录读取次数
    type_cardRBuf.success_count=0;//记录成功次数


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
	OFlag_light2((MO1*111)%4+1);//路灯调节到指定档位并获取初始档位
	MasterCar_Right(MasterCar_TrunSpeed,1);
	MasterCar_Right(MasterCar_TrunSpeed,1);
	PID_Set_recovery();
	
	success_count=type_cardRBuf.success_count;//第一次读卡成功的个数
	if (RC_Card_oneFunc4(MasterCar_GoMpValue,2250,10,K_A)==2)
	{
		sprintf(cardbuffer[1],"%s",RC_Get_buffer());
		success_count+=1;//第一次读卡成功的个数
	}
	
	MasterCar_BackMP(MasterCar_GoSpeed+10,300);
	
	if(RC_Card_ALLFun1(-300,MasterCar_GoMpValue,10,K_A,1).success_position>0)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//第2次读卡成功的个数
	}

	if (RC_Card_oneFunc4(MasterCar_GoMpValue,2250,10,K_A)==2)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//第3次读卡成功的个数
	}
	MasterCar_BackMP(MasterCar_GoSpeed,300);
	type_cardRBuf.success_count=0;
	type_cardRBuf=RC_Card_ALLFun1(0,MasterCar_GoMpValue+300,10,K_A,1);
	if (type_cardRBuf.success_count>0)
	{
		sprintf(cardbuffer[success_count],"%s",RC_Get_buffer());
		success_count+=1;//第4次读卡成功的个数
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
		success_count+=1;//第5次读卡成功的个数
	}
	
	for (int i = 0; i < 5; i++)
	{
		Send_Debug_string2(cardbuffer[i]);
	}
	

}
