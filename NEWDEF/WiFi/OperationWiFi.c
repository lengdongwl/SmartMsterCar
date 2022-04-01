/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-15 16:33:43
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-21 15:08:07
 */
#include "OperationWiFi.h"
#include "OperationFlag.h"
#include "stdint.h"
#include "delay.h"
#include "Timer.h"
#include "canP_HostCom.h"
#include "MasterCarCotrol.h"
uint8_t OWifi_CalcCode[7] = {0, 0, 0, 0, 0, 0, '\0'};  //����ѹ���㷨����
uint8_t OWifi_CRCode[7] = {0, 0, 0, 0, 0, 0};	   //���ն�ά��
uint8_t OWifi_TFTCP[7] = {0, 0, 0, 0, 0, 0, '\0'};	   //����TFT����
uint8_t OWifi_TFTShape[5] = {0, 0, 0, 0, 0};		   //[0]:����,[1]:Բ��,[2]:����,[3]:����,[4]:�����
uint8_t OWifi_alarm[7] = {0, 0, 0, 0, 0, 0, '\0'};	   //���շ��̨����
uint8_t OWifi_TFTJTFLAG = 0;                           //����TFTʶ��Ľ�ͨ��־ 2.��ͷ 3.��ת 4.ֱ�� 5.��ת 6.��ֹͨ�� 7.��ֱֹ��
uint8_t OWiFi_END = 0;								   //WIFI��ɽ��ս�����־
uint8_t *OWiFi_readBuffer = 0;						   //wifi��ȡָ��
uint8_t OWifi_CRCode_falg = 0;						   //��̬��־�� 0.δ���� 1.�������
uint8_t OWifi_TFT_falg = 0;							   //TFT���� ͼ��
uint8_t OWifi_JT = 0;								   //���ս�ͨ������
uint32_t Wifi_request = 0;							   //ƽ��ȷ��ʶ������

void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		if (Wifi_Rx_flag) //���յ�wifi��Ϣ
		{
			Operation_WiFi(); //���ʶ����
			Wifi_Rx_flag = 0;
			for (int i = 0; i < 10; i++) //�������
			{
				Wifi_Rx_Buf[i] = 0;
			}
		}
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
}

/**
 * 0xB0	TFT_A	0x00	0x00	0x00
 * 0xB1	TFT_B	0x00	0x00	0x00
 * 0xB2	��ͨ��A	0x00	0x00	0x00
 * 0xB3	��ͨ��B	0x00	0x00	0x00
 * 0xB4	��̬��־��1	0x00	0x00	0x00
 * 0xB5	��̬��־��2	0x00	0x00	0x00
 * 0xB6				
 * 0xB7	�����㷨ѹ��A	01A	10B	11C
 * 0xB8	�����㷨ѹ��B	01A	10B	11C
 * 0xB9	������1	����[0]	����[1]	����[2]
 * 0xBA	������2	����[3]	����[4]	����[5]
 * 0xBB	·�Ƴ�ʼ��λ	��ʼ��λ	0x00	0x00
 * 0xBC	����������	0x00	0x00	0x00
 * 0xCD	�ϴ�������Ϣ1	ASCLL��[0]	ASCLL��[1]	ASCLL��[2]
 * 0XCE	�ϴ�������Ϣ2	ʮ����[0]	ʮ����[1]	ʮ����[2]
 * 0XCF	�ϴ�������Ϣ3	ʮ������[0]	ʮ������[1]	ʮ������[2]
 * @description: �����������ݵ�Wifi
 * @param {uint8_t} cmd ��ָ��
 * @param {uint8_t} cmd1 ��ָ��x
 * @param {uint8_t} cmd2
 * @param {uint8_t} cmd3
 * @return {*}
 */
void OWiFi_Send(uint8_t instruct, uint8_t coord1, uint8_t coord2, uint8_t coord3)
{
	uint8_t Send_flat_computer[8] = {0X55, 0X11, 0x00, 0x00, 0x00, 0x00, 0x00, 0XBB};
	Send_flat_computer[2] = instruct;
	Send_flat_computer[3] = coord1;
	Send_flat_computer[4] = coord2;
	Send_flat_computer[5] = coord3;
	Send_flat_computer[6] = (Send_flat_computer[3] + Send_flat_computer[4] +
							 Send_flat_computer[5] + Send_flat_computer[2]) %
							0xFF;
	Send_WifiData_To_Fifo(Send_flat_computer, 8);
	delay_ms(200);
}

/**
 * @description: ��ƽ���ϴ�������Ϣ
 * @param {uint8_t} mode 1.�ϴ�ASCALL�� 2.�ϴ�ʮ���� 3.�ϴ�ʮ������
 * @return {*}
 */
void OWiFi_Send_test(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3)
{
	uint8_t cmd = 0;
	if (mode == 1)
	{
		cmd = 0xCD;
	}
	else if (mode == 2)
	{
		cmd = 0xCE;
	}
	else if (mode == 3)
	{
		cmd = 0xCF;
	}
	OWiFi_Send(cmd, data1, data2, data3);
}

/**
 * @description: ͼ��ʶ�����󲢷�ֹ��ʱ���� ģ�� 
 * @param {uint8_t} cmd ��ָ��
 * @param {uint8_t} *flag ��־��ַ
 * @param {uint32_t} time �ȴ����� ��λ����
 * @return {*}1.���ճɹ� 0.��ʱ
 */
uint8_t OWiFi_cmd(uint8_t cmd, uint32_t time)
{
	uint8_t r = 1, t = 0; //_num=2;
	OWiFi_END = 1;		  //���ý���״̬
	Wifi_request = 0;	  //����wifiȷ������
	//Wifi_Rx_flag_num=0;//���ý��մ����ٴη�������
	TIM_Cmd(TIM5, ENABLE);	  //����ʶ�����
	OWiFi_Send(cmd, 0, 0, 0); //����һ��ʶ������
	while (OWiFi_END == 1)	  //�ȴ��ش����
	{
		t++;
		if (t >= time) //��ʱ����
		{
			r = 0;
			break;
		}
		if (OWiFi_END == 0) //�����������
		{
			break;
		}
		delay_ms(500);
		delay_ms(500);

		if (Wifi_request == 0 && t > 2) //��Ϊƽ��δȷ���������ٴη���
		{
			OWiFi_Send(cmd, 0, 0, 0); //����һ��ʶ������
									  //Send_Debug_string("wifi_error:1");
		}
	}
	TIM_Cmd(TIM5, DISABLE);
	return r;
}

/**
 * @description: ����ʶ��̬��־��1
 * @param {uint32_t} time  �ȴ����� ��λ����
 * @return {*}0.��ʱ 1.���
 */
uint8_t OWiFi_QRCode1(uint32_t time)
{

	return OWiFi_cmd(0xB4, time);
}

/**
 * @description: ����ʶ��̬��־��2
 * @param {uint32_t} time  �ȴ����� ��λ����
 * @return {*}0.��ʱ 1.���
 */
uint8_t OWiFi_QRCode2(uint32_t time)
{

	return OWiFi_cmd(0xB5, time);
}

/**
 * @description: TFTʶ�� ���� ͼ���
 * @param {uint8_t} AorB 'A'=TFTA 'B'=TFTB
 * ��ͼ��ʶ���� OWifi_TFTShape[0]������ [1]��Բ��  [2]������  [3]: ���� [4]������ǣ�
 * @param {uint32_t} time  �ȴ����� ��λ����
 * @return {*}0.��ʱ 1.���
 */
uint8_t OWiFi_TFT(uint8_t AorB, uint32_t time)
{
	return OWiFi_cmd(AorB == 'A' ? 0xB0 : 0xB1, time); //����TFTʶ����
}
/**
 * @description: ����ͨ��ʶ�� �����־����ȷ��ʶ����
 * @param {uint8_t} AorB 'A'=��ͨ��A 'B'=��ͨ��B
 * @return {*}
 */
uint8_t OWiFi_JTlight(uint8_t AorB)
{
	if (AorB == 'A')
	{
		OFlag_JT_cmd(1, 0); //��ͨ��A����ʶ��ģʽ
		delay_ms(200);
		OFlag_JT_cmd(1, 0); //��ͨ��A����ʶ��ģʽ
		delay_ms(200);
		OFlag_JT_cmd(1, 0);		 //��ͨ��A����ʶ��ģʽ
		if (OWiFi_cmd(0xB2, 15)) //����ʶ��ɹ�
		{
			OFlag_JT_cmd(1, OWifi_JT); //Aȷ�Ͻ��
			delay_ms(200);
			OFlag_JT_cmd(1, OWifi_JT); //Aȷ�Ͻ��
			delay_ms(200);
			OFlag_JT_cmd(1, OWifi_JT); //Aȷ�Ͻ��
			return 1;
		}
		OFlag_JT_cmd(1, 1); //ʶ��ʧ��æ�½��
		delay_ms(100);
		OFlag_JT_cmd(1, 1); //ʶ��ʧ��æ�½��
	}
	else
	{
		OFlag_JT_cmd(0, 0); //��ͨ��B����ʶ��ģʽ
		delay_ms(200);
		OFlag_JT_cmd(0, 0); //��ͨ��B����ʶ��ģʽ
		delay_ms(200);
		OFlag_JT_cmd(0, 0);		 //��ͨ��B����ʶ��ģʽ
		if (OWiFi_cmd(0xB3, 15)) //����ʶ��ɹ�
		{
			OFlag_JT_cmd(0, OWifi_JT); //Bȷ�Ͻ��
			delay_ms(200);
			OFlag_JT_cmd(0, OWifi_JT); //Bȷ�Ͻ��
			delay_ms(200);
			OFlag_JT_cmd(0, OWifi_JT); //Bȷ�Ͻ��
			return 1;
		}
		OFlag_JT_cmd(0, 1); //ʶ��ʧ��æ�½��
		delay_ms(100);
		OFlag_JT_cmd(0, 1); //ʶ��ʧ��æ�½��
	}
	return 0;
}

/**
 * @description: Wifi�ϴ����ݣ�����ѹ���㷨����
 * @param {uint8_t} *data  �ϴ��㷨����
 * @param {uint32_t} time �ȴ����� ��λ����
 * @return {*}1.���ճɹ� 0.��ʱ
 */
uint8_t OWiFi_CodeCalc(uint8_t *data, uint32_t time)
{
	uint8_t r = 1, t = 0;
	OWiFi_END = 1;								 //���ý���״̬
	Wifi_request = 0;							 //����wifiȷ������
	TIM_Cmd(TIM5, ENABLE);						 //����ʶ�����
	OWiFi_Send(0xb7, data[0], data[1], data[2]); //�ϴ���Ϣ��ƽ�崦���㷨
	delay_ms(100);
	OWiFi_Send(0xb8, data[3], data[4], data[5]); //�ϴ���Ϣ��ƽ�崦���㷨
	while (OWiFi_END == 1)						 //�ȴ��ش����
	{
		t++;
		if (t >= time) //��ʱ����
		{
			r = 0;
			break;
		}
		if (OWiFi_END == 0) //�����������
		{
			break;
		}
		delay_ms(500);
		delay_ms(500);

		if (Wifi_request == 0 && t > 2) //��Ϊƽ��δȷ���������ٴη���
		{
			OWiFi_Send(0xb7, data[0], data[1], data[2]); //�ϴ���Ϣ��ƽ�崦���㷨
			delay_ms(100);
			OWiFi_Send(0xb8, data[3], data[4], data[5]); //�ϴ���Ϣ��ƽ�崦���㷨
		}
	}
	TIM_Cmd(TIM5, DISABLE);
	return r;
}

/**
 * @description: WiFi������Zigbee��Ϣ
 * @param {*}
 * @return {*}
 */
uint8_t OWiFi_toZigbee(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[1] != 0X01))
	{
		if ((Wifi_Rx_Buf[1] != 0XAA))
		{
			if (Wifi_Rx_Buf[7] == 0xBB)
			{
				Agreement_Send(2, Wifi_Rx_Buf);
				return 1;
			}
		}
	}
	return 0;
}

/**
 * @description: WiFi��Zigbee���ջص�
 * @param {*}
 * @return {*}
 */
void Operation_WiFi(void)
{
	if ((Wifi_Rx_Buf[0] == 0x55) && (Wifi_Rx_Buf[7] == 0xBB))
	{
		if (Wifi_Rx_Buf[1] == 0XAA)
		{
			switch (Wifi_Rx_Buf[2])
			{
			case 0xA1:
				Wifi_request = 1;		 //ȷ��
				if (Wifi_Rx_Buf[3] == 1) //ʶ�����
				{
					OWiFi_END = 0;
				}
				break;
			case 0xA2: //��ά��A
				OWifi_CRCode[0] = Wifi_Rx_Buf[3];
				OWifi_CRCode[1] = Wifi_Rx_Buf[4];
				OWifi_CRCode[2] = Wifi_Rx_Buf[5];
				break;
			case 0xA3: //��ά��B
				OWifi_CRCode[3] = Wifi_Rx_Buf[3];
				OWifi_CRCode[4] = Wifi_Rx_Buf[4];
				OWifi_CRCode[5] = Wifi_Rx_Buf[5];
				break;
			case 0x40: //TFT����1
				OWifi_TFTCP[0] = Wifi_Rx_Buf[3];
				OWifi_TFTCP[1] = Wifi_Rx_Buf[4];
				OWifi_TFTCP[2] = Wifi_Rx_Buf[5];
				break;
			case 0x41: //TFT����2
				OWifi_TFTCP[3] = Wifi_Rx_Buf[3];
				OWifi_TFTCP[4] = Wifi_Rx_Buf[4];
				OWifi_TFTCP[5] = Wifi_Rx_Buf[5];
				break;
			case 0x45: //ͼ��ʶ��1
				OWifi_TFTShape[0] = Wifi_Rx_Buf[3];
				OWifi_TFTShape[1] = Wifi_Rx_Buf[4];
				OWifi_TFTShape[2] = Wifi_Rx_Buf[5];

				break;
			case 0x46: //ͼ��ʶ��2
				OWifi_TFTShape[3] = Wifi_Rx_Buf[3];
				OWifi_TFTShape[4] = Wifi_Rx_Buf[4];
				break;
			case 0xAE: //�����㷨ѹ��A
				OWifi_CalcCode[0] = Wifi_Rx_Buf[3];
				OWifi_CalcCode[1] = Wifi_Rx_Buf[4];
				OWifi_CalcCode[2] = Wifi_Rx_Buf[5];
				break;
			case 0xAF: //�����㷨ѹ��B
				OWifi_CalcCode[3] = Wifi_Rx_Buf[3];
				OWifi_CalcCode[4] = Wifi_Rx_Buf[4];
				OWifi_CalcCode[5] = Wifi_Rx_Buf[5];
				break;
			case 0xA5: //ʶ��TFT��ͨ��־
				OWifi_TFTJTFLAG	= Wifi_Rx_Buf[3];
				if(OWifi_TFTJTFLAG<2 && OWifi_TFTJTFLAG>7)//�����������ֵΪ2
				{
					OWifi_TFTJTFLAG = 2;
				}
				break;
			default:
				break;
			}
		}
		else if (Wifi_Rx_Buf[1] == 0X0E)
		{
			if (Wifi_Rx_Buf[2] == 0x02) //��ͨ��ʶ��
			{
				OWifi_JT = Wifi_Rx_Buf[3];
			}
		}
		else
		{
			MasterCar_TaskReceiveThread(); //�鿴�Ƿ�Ϊ�����߳�(��׿������������Ϣ����������Ϊzigbeeת��������Ʊ�־��)
		}
	}
}

/**
 * @description: WIFI�ٿ�����
 * @param {*}
 * @return {*}
 */
void Operation_WiFi_Master(uint8_t *buf)
{
	if ((buf[0] == 0x55) && (buf[1] == 0XAA) && (buf[7] == 0xBB))
	{
		switch (buf[2])
		{
		case 0x01:
			MasterCar_Stop();
			break;
		case 0x02:
			MasterCar_SmartRunMP(buf[3], buf[4] + buf[5] * 0x100); // ����ǰ��
			break;
		case 0x03:
			MasterCar_BackMP(buf[3], buf[4] + buf[5] * 0x100); // ��������
			break;
		case 0x04:
			if (buf[3] == 0)
			{
				// ������ת
			}
			else
			{
				MasterCar_LeftMP(buf[3], buf[4] + buf[5] * 0x100);
			}
			break;
		case 0x05:
			if (buf[3] == 0)
			{
				// ������ת
			}
			else
			{
				MasterCar_RightMP(buf[3], buf[4] + buf[5] * 0x100);
			}
			break;
		case 0x06:
			MasterCar_SmartRun(buf[3]); // ����ѭ��
			break;
		case 0xA0: //��������
			Set_Flag_Task(0x08);
			break;
		case 0XAA: //���շ��̨����1
			OWifi_alarm[0] = Wifi_Rx_Buf[3];
			OWifi_alarm[1] = Wifi_Rx_Buf[4];
			OWifi_alarm[2] = Wifi_Rx_Buf[5];
			break;
		case 0XAB: //���շ��̨����2
			OWifi_alarm[3] = Wifi_Rx_Buf[3];
			OWifi_alarm[4] = Wifi_Rx_Buf[4];
			OWifi_alarm[5] = Wifi_Rx_Buf[5];
			break;
		}
	}
}
