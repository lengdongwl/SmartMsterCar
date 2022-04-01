#include "PID.h"
#include "CanP_HostCom.h"
//#include "MasterCarCotrol.h"
#define TRACK_MODE 1 //1.�˵�ѭ�� 0.�ߵ�ѭ��
/*****PIDѭ��������������ֵ����*******
 * PID�ο�ֵ
 * P=20,I=0,D=10  ��ͨѭ��
 * P=20,I=0,D=20  ��ͨѭ����ֹ����̫�� ������΢����ѭ��
 * P=50,I=0,D=0   �л��������������
 * P=50,I=0,D=2000  �л�������������� ��ֹ��������
 * P=27���޲�����
 * 
****************************************/
static float Kp = 20, Ki = 0.06, Kd = 19;		  //PID�����������
static float R_Kp = 0, R_Ki = 0, R_Kd = 0;		  //����setǰ��Kp,Ki,Kd���� �ָ�ʹ��
static float P = 0, I = 0, D = 0, PID_value = 0; //PID�ο�����
static float previous_error = 0, previous_I = 0; //���ֵ
//ѭ���������ò���
void PID_reset(void)
{
	P = 0, I = 0, D = 0, PID_value = 0; //PID�ο�����
	previous_error = 0, previous_I = 0; //���ֵ
}
//PID���� ����kp/ki/kd����;
void PID_Set(float K_P, float K_I, float K_D)
{
	R_Kd = Kd; //����setǰ����
	R_Ki = Ki;
	R_Kp = Kp;
	Kp = K_P;
	Ki = K_I;
	Kd = K_D;
}
//�ָ�PID_Set();ǰ����
void PID_Set_recovery(void)
{
	Kp = R_Kp;
	Ki = R_Ki;
	Kd = R_Kd;
}
//PID������� @parm error:ƫ��̶�ֵ
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
//��ȡPIDѭ��ƫ��̶�ֵ Ϩ�����������ϲŻ���8ȫ��ֹͣ @return:PID������
float PID_Track(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ
	uint16_t data8 = Get_Host_UpTrack(TRACK_H8);
	switch (data8)
	{
	case 0: //ȫ��
		error = 99;
		return 99;
	case 0xE7: //11100111 ��ͷ������
		error = 0;
		break;
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1;
		break;

	case 0xDF: //����ƫ��1
		error = -2;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;

	case 0xFB: //����ƫ��1
		error = 2;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;

	case 0xCF: //˫��ƫ��1
		error = -1.6;
		break;
	case 0x9F: //˫��ƫ��2
		error = -2.6;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 1.6;
		break;
	case 0xF9: //˫��ƫ��2
		error = 2.6;
		break;
	case 0xFC: //˫��ƫ��3
		error = 3.2;
		break;

	default:
		if(Get_Host_TrackDieCount(TRACK_H8)>3)
		{
			error = 99;
			return 99;
		}else
		{
			error = 0;
		}
	}
	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{

		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}
//��ȡPIDѭ��ƫ��̶�ֵ ����ѭ��ȫ����ֹͣѭ��(ȫ��ֱ�� ���߷���99) @return:PID������
float PID_Track2(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0:
		error = 99;
		return 99;
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1;
		break;
	case 0xDF: //����ƫ��1
		error = -2;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;
	case 0xFB: //����ƫ��1
		error = 2;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;
	case 0xCF: //˫��ƫ��1
		error = -2;
		break;
	case 0x9F: //˫��ƫ��2
		error = -3;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 2;
		break;
	case 0xF9: //˫��ƫ��2
		error = 3;
		break;
	case 0xFC: //˫��ƫ��3
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
		error = 99; //ȫ��
		return 99;
	case 0x77: //111 0111 ��ͷ������
		error = 0;
		break;
	case 0x6F: //110 1111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0x7B: //111 1011 ��ͷ����ƫ��
		error = 1;
		break;

	case 0x5F: //����ƫ��1 101 1111
		error = -2;
		break;
	case 0x3F: //����ƫ��2 011 1111
		error = -3;
		break;

	case 0x7D: //����ƫ��1 111 1101
		error = 2;
		break;
	case 0x7E: //����ƫ��2 111 1110
		error = 3;
		break;

	case 0x67://˫��ƫ��1 110 0111
		error = -1.6;
		break;
	case 0x4F://˫��ƫ��2 100 1111
		error = -2.6;
		break;
	case 0x1F://˫��ƫ��3 001 1111
		error = -3;
		break;		
	case 0x73://˫��ƫ��1 111 0011
		error = 1.6;
		break;
	case 0x79://˫��ƫ��2 111 1001
		error = 2.6;
		break;
	case 0x7C://˫��ƫ��3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/
	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{

		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}

//��ȡPIDѭ��ƫ��̶�ֵ Ѱ����(ȫ������99 ��������ΰ�) @return:PID������
float PID_Track3(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ
	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xff:
		error = 99;
		return 99;
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1;
		break;
	case 0xDF: //����ƫ��1
		error = -2;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;
	case 0xFB: //����ƫ��1
		error = 2;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;
	case 0xCF: //˫��ƫ��1
		error = -2;
		break;
	case 0x9F: //˫��ƫ��2
		error = -3;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 2;
		break;
	case 0xF9: //˫��ƫ��2
		error = 3;
		break;
	case 0xFC: //˫��ƫ��3
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
	case 0x77: //111 0111 ��ͷ������
		error = 0;
		break;
	case 0x6F: //110 1111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0x7B: //111 1011 ��ͷ����ƫ��
		error = 1;
		break;

	case 0x5F: //����ƫ��1 101 1111
		error = -2;
		break;
	case 0x3F: //����ƫ��2 011 1111
		error = -3;
		break;

	case 0x7D: //����ƫ��1 111 1101
		error = 2;
		break;
	case 0x7E: //����ƫ��2 111 1110
		error = 3;
		break;

	case 0x67://˫��ƫ��1 110 0111
		error = -1.6;
		break;
	case 0x4F://˫��ƫ��2 100 1111
		error = -2.6;
		break;
	case 0x1F://˫��ƫ��3 001 1111
		error = -3;
		break;		
	case 0x73://˫��ƫ��1 111 0011
		error = 1.6;
		break;
	case 0x79://˫��ƫ��2 111 1001
		error = 2.6;
		break;
	case 0x7C://˫��ƫ��3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/
	
	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}


//��ȡPIDѭ��ƫ��̶�ֵ Ѱ������(ȫ��ֱ�з���99) @return:PID������
float PID_Track4(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xff://ȫ��
		PID_reset();
		Send_UpMotor(speed, speed);
		return 99;
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1;
		break;
	case 0xDF: //����ƫ��1
		error = -2;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;
	case 0xFB: //����ƫ��1
		error = 2;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;
	case 0xCF: //˫��ƫ��1
		error = -2;
		break;
	case 0x9F: //˫��ƫ��2
		error = -3;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 2;
		break;
	case 0xF9: //˫��ƫ��2
		error = 3;
		break;
	case 0xFC: //˫��ƫ��3
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
	case 0x77: //111 0111 ��ͷ������
		error = 0;
		break;
	case 0x6F: //110 1111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0x7B: //111 1011 ��ͷ����ƫ��
		error = 1;
		break;

	case 0x5F: //����ƫ��1 101 1111
		error = -2;
		break;
	case 0x3F: //����ƫ��2 011 1111
		error = -3;
		break;

	case 0x7D: //����ƫ��1 111 1101
		error = 2;
		break;
	case 0x7E: //����ƫ��2 111 1110
		error = 3;
		break;

	case 0x67://˫��ƫ��1 110 0111
		error = -1.6;
		break;
	case 0x4F://˫��ƫ��2 100 1111
		error = -2.6;
		break;
	case 0x1F://˫��ƫ��3 001 1111
		error = -3;
		break;		
	case 0x73://˫��ƫ��1 111 0011
		error = 1.6;
		break;
	case 0x79://˫��ƫ��2 111 1001
		error = 2.6;
		break;
	case 0x7C://˫��ƫ��3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/

	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}



//��ȡPIDѭ��ƫ��̶�ֵ ѭ�����̰棨�ǵ��������Ϊǰ�� ������������Σ� @return:PID������
float PID_TrackMP(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1.1;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1.1;
		break;

	case 0xDF: //����ƫ��1
		error = -2;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;

	case 0xFB: //����ƫ��1
		error = 2;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;

	case 0xCF: //˫��ƫ��1
		error = -1.6;
		break;
	case 0x9F: //˫��ƫ��2
		error = -2.6;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 1.6;
		break;
	case 0xF9: //˫��ƫ��2
		error = 2.6;
		break;
	case 0xFC: //˫��ƫ��3
		error = 3.2;
		break;

	default: //0x00&0xff
		error = 0;
	}
/*
	switch (Get_Host_UpTrack(TRACK_Q7))
	{
	case 0x6F: //110 1111 ��ͷ����ƫ��
		error = -1;
		break;
	case 0x7B: //111 1011 ��ͷ����ƫ��
		error = 1;
		break;

	case 0x5F: //����ƫ��1 101 1111
		error = -2;
		break;
	case 0x3F: //����ƫ��2 011 1111
		error = -3;
		break;

	case 0x7D: //����ƫ��1 111 1101
		error = 2;
		break;
	case 0x7E: //����ƫ��2 111 1110
		error = 3;
		break;

	case 0x67://˫��ƫ��1 110 0111
		error = -1.6;
		break;
	case 0x4F://˫��ƫ��2 100 1111
		error = -2.6;
		break;
	case 0x1F://˫��ƫ��3 001 1111
		error = -3;
		break;		
	case 0x73://˫��ƫ��1 111 0011
		error = 1.6;
		break;
	case 0x79://˫��ƫ��2 111 1001
		error = 2.6;
		break;
	case 0x7C://˫��ƫ��3 111 1100
		error = 3;
		break;				

	default: //0x00&0xff
		error = 0;
		break;
	}
*/	
	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}


//��ȡPIDѭ��ƫ��̶�ֵ ʮ��·�ڿ��ܴ��ڰ׿���(ѭ�������ĵ����ϻ�ȫ������������ѭ��) @return:PID������
//����ѭ�� ���3�����ϡ�ȫ����ȫ�� ֹͣѭ��(�����ڰ�ɫ����ɫֹͣ����ͣ��)
float PID_Track5(unsigned int speed)
{
	uint16_t data7,data8;
	float error = 0;

	data8 = Get_Host_UpTrack(TRACK_H8);
	data7 = Get_Host_UpTrack(TRACK_Q7);
	switch (data8)
	{
	case 0xff://��8�Ƶ�ȫ��
		if (data7 == 0x7f)//ǰ7��ȫ�� ���������ж�ǰ7�� ��˵ƿ��ܳ�������
		{
			error = 99;
		}else
		{
			error = 0;
		}
		break;
	case 0x7E: // 0111 1110 �׿����
		error = 99;
		break;	
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -1.2;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 1.2;
		break;
	case 0xDF: //����ƫ��1
		error = -2.4;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;
	case 0xFB: //����ƫ��1
		error = 2.4;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;
	case 0xCF: //˫��ƫ��1
		error = -2.4;
		break;
	case 0x9F: //˫��ƫ��2
		error = -3;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 2.4;
		break;
	case 0xF9: //˫��ƫ��2
		error = 3;
		break;
	case 0xFC: //˫��ƫ��3
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

	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}




//��ȡPIDѭ��ƫ��̶�ֵ ѭ�����̰棨�ǵ��������Ϊǰ�� ������������Σ� @return:PID������
float PID_TrackMP2(unsigned int speed)
{
	float error = 0;
	//P=0,I=0,D=0, PID_value = 0;         //PID�ο�����
	//previous_error = 0, previous_I = 0; //���ֵ
	//ѡ��ƫ��̶�ֵ

	switch (Get_Host_UpTrack(TRACK_H8))
	{
	case 0xEF: //11101111 ��ͷ����ƫ��
		error = -2;
		break;
	case 0xF7: //11110111 ��ͷ����ƫ��
		error = 2;
		break;

	case 0xDF: //����ƫ��1
		error = -2.5;
		break;
	case 0xBF: //����ƫ��2
		error = -3;
		break;
	case 0x7F: //����ƫ��3
		error = -3.2;
		break;

	case 0xFB: //����ƫ��1
		error = 2.5;
		break;
	case 0xFD: //����ƫ��2
		error = 3;
		break;
	case 0xFE: //����ƫ��3
		error = 3.2;
		break;

	case 0xCF: //˫��ƫ��1
		error = -2;
		break;
	case 0x9F: //˫��ƫ��2
		error = -2.6;
		break;
	case 0x3F: //˫��ƫ��3
		error = -3.2;
		break;
	case 0xF3: //˫��ƫ��1
		error = 2;
		break;
	case 0xF9: //˫��ƫ��2
		error = 2.6;
		break;
	case 0xFC: //˫��ƫ��3
		error = 3.2;
		break;

	default: //0x00&0xff
		error = 0;
	}
	if (error == 0) //�ص����������ò���
	{
		PID_reset();
		Send_UpMotor(speed, speed);
		return 0;
	}
	if (error != 99)
	{
		PID_Calc(error); //����ƫ��̶� ����PID���
		Send_UpMotor(speed + PID_value, speed - PID_value);
		return PID_value;
		//Send_UpMotor(speed+PID_value,speed-PID_value);
	}
	return 99;
}
