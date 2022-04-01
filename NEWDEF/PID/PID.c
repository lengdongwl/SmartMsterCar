/*****************************************
2023/3/29  PID_TrackMP2.3.4优化寻卡判断
*****************************************/

#include "PID.h"
#include "CanP_HostCom.h"
//#include "MasterCarCotrol.h"
#define TRACK_MODE 1 //1.八灯循迹 0.七灯循迹
/*****PID循迹函数调整车身值计算*******
 * PID参考值
 * P=20,I=0,D=10  普通循迹
 * P=20,I=0,D=20  普通循迹防止倒退太快 适用于微调整循迹
 * P=50,I=0,D=0   有缓冲过读卡过地形
 * P=50,I=0,D=2000  有缓冲过读卡过地形 防止抖动过快
 * P=27极限不后退
 * 
****************************************/
static float Kp = 20, Ki = 0.06, Kd = 19;		  //PID弯道参数参数
static float R_Kp = 0, R_Ki = 0, R_Kd = 0;		  //保存set前的Kp,Ki,Kd数据 恢复使用
static float P = 0, I = 0, D = 0, PID_value = 0; //PID参考参数
static float previous_error = 0, previous_I = 0; //误差值
//循迹完需重置参数
void PID_reset(void)
{
	P = 0, I = 0, D = 0, PID_value = 0; //PID参考参数
	previous_error = 0, previous_I = 0; //误差值
}
//PID调参 设置kp/ki/kd参数;
void PID_Set(float K_P, float K_I, float K_D)
{
	R_Kd = Kd; //保存set前数据
	R_Ki = Ki;
	R_Kp = Kp;
	Kp = K_P;
	Ki = K_I;
	Kd = K_D;
}
//恢复PID_Set();前参数
void PID_Set_recovery(void)
{
	Kp = R_Kp;
	Ki = R_Ki;
	Kd = R_Kd;
}
//PID结果计算 @parm error:偏差程度值
void PID_Calc(float error)
{
	P = error;
	I = I + error;
	//I = I + previous_I;
	D = error - previous_error;
	PID_value = (Kp * P) + (Ki * I) + (Kd * D);
	//previous_I = I;
	previous_error = error;
}
//获取PID循迹偏差程度值 普通循迹 @return:PID计算结果
float PID_Track(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值
#if TRACK_MODE==1 //8灯循迹
	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xff: //全亮
		error = 0;
		break;
	case 0xE7: //11100111 车头在正中
		error = 0;
		break;
	case 0xEF: //11101111 车头在中偏左
		error = -1;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1;
		break;

	case 0xDF: //单灯偏左1
		error = -2;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;

	case 0xFB: //单灯偏右1
		error = 2;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;

	case 0xCF: //双灯偏左1
		error = -1.6;
		break;
	case 0x9F: //双灯偏左2
		error = -2.6;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 1.6;
		break;
	case 0xF9: //双灯偏右2
		error = 2.6;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;

	default: //0x00&0xff
		//Send_UpMotor(0, 0);
		error = 99;
		return 99;
	}
#else 
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0x7F: //全亮
		error = 0;
		break;
	case 0x77: //111 0111 车头在正中
		error = 0;
		break;
	case 0x6F: //110 1111 车头在中偏左
		error = -1;
		break;
	case 0x7B: //111 1011 车头在中偏右
		error = 1;
		break;

	case 0x5F: //单灯偏左1 101 1111
		error = -2;
		break;
	case 0x3F: //单灯偏左2 011 1111
		error = -3;
		break;

	case 0x7D: //单灯偏右1 111 1101
		error = 2;
		break;
	case 0x7E: //单灯偏右2 111 1110
		error = 3;
		break;

	case 0x67://双灯偏左1 110 0111
		error = -1.6;
		break;
	case 0x4F://双灯偏左2 100 1111
		error = -2.6;
		break;
	case 0x1F://双灯偏左3 001 1111
		error = -3;
		break;		
	case 0x73://双灯偏右1 111 0011
		error = 1.6;
		break;
	case 0x79://双灯偏右2 111 1001
		error = 2.6;
		break;
	case 0x7C://双灯偏右3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		//Send_UpMotor(0, 0);
		error = 99;
		return 99;
	}
#endif 	
	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{

		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}
//获取PID循迹偏差程度值 寻卡版(全亮直行 黑线返回99) @return:PID计算结果
float PID_Track2(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0:
		if (Get_Host_UpTrack(TRACK_Q7)==0x7f)
		{
			PID_reset();
			Send_UpMotor(speed, speed);
			return 99;
		}
		return 99;
	case 0xEF: //11101111 车头在中偏左
		error = -1;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1;
		break;
	case 0xDF: //单灯偏左1
		error = -2;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;
	case 0xFB: //单灯偏右1
		error = 2;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;
	case 0xCF: //双灯偏左1
		error = -2;
		break;
	case 0x9F: //双灯偏左2
		error = -3;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 2;
		break;
	case 0xF9: //双灯偏右2
		error = 3;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;
	default: //0x00&0xff
		error = 0;
		break;
	}
/*
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0:
		error = 99; //全灭
		return 99;
	case 0x77: //111 0111 车头在正中
		error = 0;
		break;
	case 0x6F: //110 1111 车头在中偏左
		error = -1;
		break;
	case 0x7B: //111 1011 车头在中偏右
		error = 1;
		break;

	case 0x5F: //单灯偏左1 101 1111
		error = -2;
		break;
	case 0x3F: //单灯偏左2 011 1111
		error = -3;
		break;

	case 0x7D: //单灯偏右1 111 1101
		error = 2;
		break;
	case 0x7E: //单灯偏右2 111 1110
		error = 3;
		break;

	case 0x67://双灯偏左1 110 0111
		error = -1.6;
		break;
	case 0x4F://双灯偏左2 100 1111
		error = -2.6;
		break;
	case 0x1F://双灯偏左3 001 1111
		error = -3;
		break;		
	case 0x73://双灯偏右1 111 0011
		error = 1.6;
		break;
	case 0x79://双灯偏右2 111 1001
		error = 2.6;
		break;
	case 0x7C://双灯偏右3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/
	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{

		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}

//获取PID循迹偏差程度值 寻卡版(全亮返回99 过特殊地形版) @return:PID计算结果
float PID_Track3(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值
	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xff:
		if (Get_Host_UpTrack(TRACK_Q7)==0x7f)
		{
			PID_reset();
			Send_UpMotor(speed, speed);
			return 99;
		}
		return 99;
	case 0xEF: //11101111 车头在中偏左
		error = -1;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1;
		break;
	case 0xDF: //单灯偏左1
		error = -2;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;
	case 0xFB: //单灯偏右1
		error = 2;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;
	case 0xCF: //双灯偏左1
		error = -2;
		break;
	case 0x9F: //双灯偏左2
		error = -3;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 2;
		break;
	case 0xF9: //双灯偏右2
		error = 3;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;
	default: //0x00&0xff
		error = 0;
		break;
	}
/*
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0x7F:
		error = 99;
		return 99;
	case 0x77: //111 0111 车头在正中
		error = 0;
		break;
	case 0x6F: //110 1111 车头在中偏左
		error = -1;
		break;
	case 0x7B: //111 1011 车头在中偏右
		error = 1;
		break;

	case 0x5F: //单灯偏左1 101 1111
		error = -2;
		break;
	case 0x3F: //单灯偏左2 011 1111
		error = -3;
		break;

	case 0x7D: //单灯偏右1 111 1101
		error = 2;
		break;
	case 0x7E: //单灯偏右2 111 1110
		error = 3;
		break;

	case 0x67://双灯偏左1 110 0111
		error = -1.6;
		break;
	case 0x4F://双灯偏左2 100 1111
		error = -2.6;
		break;
	case 0x1F://双灯偏左3 001 1111
		error = -3;
		break;		
	case 0x73://双灯偏右1 111 0011
		error = 1.6;
		break;
	case 0x79://双灯偏右2 111 1001
		error = 2.6;
		break;
	case 0x7C://双灯偏右3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/
	
	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}


//获取PID循迹偏差程度值 寻读卡版(全亮直行返回99) @return:PID计算结果
float PID_Track4(unsigned int speed)
{
	float error = 0;
	uint16_t h8,q7;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值
	h8 = Get_Host_UpTrack(TRACK_H8);
	switch (h8)
	{
	case 0xff://全亮
		if (Get_Host_UpTrack(TRACK_Q7)==0x7f)
		{
			PID_reset();
			Send_UpMotor(speed, speed);
			return 99;
		}
		
		
	case 0xEF: //11101111 车头在中偏左
		error = -1;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1;
		break;
	case 0xDF: //单灯偏左1
		error = -2;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;
	case 0xFB: //单灯偏右1
		error = 2;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;
	case 0xCF: //双灯偏左1
		error = -2;
		break;
	case 0x9F: //双灯偏左2
		error = -3;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 2;
		break;
	case 0xF9: //双灯偏右2
		error = 3;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;
	default: //0x00&0xff
		error = 0;
		break;
	}
/*
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0x7F:
		PID_reset();
		Send_UpMotor(speed, speed);
		return 99;
	case 0x77: //111 0111 车头在正中
		error = 0;
		break;
	case 0x6F: //110 1111 车头在中偏左
		error = -1;
		break;
	case 0x7B: //111 1011 车头在中偏右
		error = 1;
		break;

	case 0x5F: //单灯偏左1 101 1111
		error = -2;
		break;
	case 0x3F: //单灯偏左2 011 1111
		error = -3;
		break;

	case 0x7D: //单灯偏右1 111 1101
		error = 2;
		break;
	case 0x7E: //单灯偏右2 111 1110
		error = 3;
		break;

	case 0x67://双灯偏左1 110 0111
		error = -1.6;
		break;
	case 0x4F://双灯偏左2 100 1111
		error = -2.6;
		break;
	case 0x1F://双灯偏左3 001 1111
		error = -3;
		break;		
	case 0x73://双灯偏右1 111 0011
		error = 1.6;
		break;
	case 0x79://双灯偏右2 111 1001
		error = 2.6;
		break;
	case 0x7C://双灯偏右3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/

	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}



//获取PID循迹偏差程度值 循迹码盘版（非调整情况都为前进 适用于特殊地形） @return:PID计算结果
float PID_TrackMP(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xEF: //11101111 车头在中偏左
		error = -1.1;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1.1;
		break;

	case 0xDF: //单灯偏左1
		error = -2;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;

	case 0xFB: //单灯偏右1
		error = 2;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;

	case 0xCF: //双灯偏左1
		error = -1.6;
		break;
	case 0x9F: //双灯偏左2
		error = -2.6;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 1.6;
		break;
	case 0xF9: //双灯偏右2
		error = 2.6;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;

	default: //0x00&0xff
		error = 0;
	}
/*
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0x6F: //110 1111 车头在中偏左
		error = -1;
		break;
	case 0x7B: //111 1011 车头在中偏右
		error = 1;
		break;

	case 0x5F: //单灯偏左1 101 1111
		error = -2;
		break;
	case 0x3F: //单灯偏左2 011 1111
		error = -3;
		break;

	case 0x7D: //单灯偏右1 111 1101
		error = 2;
		break;
	case 0x7E: //单灯偏右2 111 1110
		error = 3;
		break;

	case 0x67://双灯偏左1 110 0111
		error = -1.6;
		break;
	case 0x4F://双灯偏左2 100 1111
		error = -2.6;
		break;
	case 0x1F://双灯偏左3 001 1111
		error = -3;
		break;		
	case 0x73://双灯偏右1 111 0011
		error = 1.6;
		break;
	case 0x79://双灯偏右2 111 1001
		error = 2.6;
		break;
	case 0x7C://双灯偏右3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/	
	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}


//获取PID循迹偏差程度值 十字路口可能存在白卡版(循迹版灭四灯以上或全亮则跳出本次循迹) @return:PID计算结果
float PID_Track5(unsigned int speed)
{
	float error = 0;

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xff://全亮
		error = 99;
		break;
	case 0x7E: // 0111 1110 白卡横放
		error = 99;
		break;	
	case 0xEF: //11101111 车头在中偏左
		error = -1.2;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 1.2;
		break;
	case 0xDF: //单灯偏左1
		error = -2.4;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;
	case 0xFB: //单灯偏右1
		error = 2.4;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;
	case 0xCF: //双灯偏左1
		error = -2.4;
		break;
	case 0x9F: //双灯偏左2
		error = -3;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 2.4;
		break;
	case 0xF9: //双灯偏右2
		error = 3;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;
	default: 
		if(Get_Host_TrackDieCount(TRACK_H8)>3)
		{
			error = 99;
		}else
		{
			error = 0;
		}
		break;
	}

	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}




//获取PID循迹偏差程度值 循迹码盘版（非调整情况都为前进 适用于特殊地形） @return:PID计算结果
float PID_TrackMP2(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID参考参数
	//previous_error = 0, previous_I = 0; //误差值
	//选择偏差程度值

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xEF: //11101111 车头在中偏左
		error = -2;
		break;
	case 0xF7: //11110111 车头在中偏右
		error = 2;
		break;

	case 0xDF: //单灯偏左1
		error = -2.5;
		break;
	case 0xBF: //单灯偏左2
		error = -3;
		break;
	case 0x7F: //单灯偏左3
		error = -3.2;
		break;

	case 0xFB: //单灯偏右1
		error = 2.5;
		break;
	case 0xFD: //单灯偏右2
		error = 3;
		break;
	case 0xFE: //单灯偏右3
		error = 3.2;
		break;

	case 0xCF: //双灯偏左1
		error = -2;
		break;
	case 0x9F: //双灯偏左2
		error = -2.6;
		break;
	case 0x3F: //双灯偏左3
		error = -3.2;
		break;
	case 0xF3: //双灯偏右1
		error = 2;
		break;
	case 0xF9: //双灯偏右2
		error = 2.6;
		break;
	case 0xFC: //双灯偏右3
		error = 3.2;
		break;

	default: //0x00&0xff
		error = 0;
	}
	if (error == 0) //回到轨道完毕重置参数
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //根据偏差程度 计算PID结果
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}