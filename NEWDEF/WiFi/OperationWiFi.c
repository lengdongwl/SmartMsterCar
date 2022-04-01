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
uint8_t OWifi_CalcCode[7] = {0, 0, 0, 0, 0, 0, '\0'};  //接收压缩算法数据
uint8_t OWifi_CRCode[7] = {0, 0, 0, 0, 0, 0};	   //接收二维码
uint8_t OWifi_TFTCP[7] = {0, 0, 0, 0, 0, 0, '\0'};	   //接收TFT车牌
uint8_t OWifi_TFTShape[5] = {0, 0, 0, 0, 0};		   //[0]:矩形,[1]:圆形,[2]:三角,[3]:菱形,[4]:五角星
uint8_t OWifi_alarm[7] = {0, 0, 0, 0, 0, 0, '\0'};	   //接收烽火台数据
uint8_t OWifi_TFTJTFLAG = 0;                           //接收TFT识别的交通标志 2.掉头 3.右转 4.直行 5.左转 6.禁止通行 7.禁止直行
uint8_t OWiFi_END = 0;								   //WIFI完成接收结束标志
uint8_t *OWiFi_readBuffer = 0;						   //wifi读取指针
uint8_t OWifi_CRCode_falg = 0;						   //静态标志物 0.未接收 1.接收完成
uint8_t OWifi_TFT_falg = 0;							   //TFT车牌 图形
uint8_t OWifi_JT = 0;								   //接收交通灯数据
uint32_t Wifi_request = 0;							   //平板确认识别请求

void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		if (Wifi_Rx_flag) //接收到wifi消息
		{
			Operation_WiFi(); //检测识别结果
			Wifi_Rx_flag = 0;
			for (int i = 0; i < 10; i++) //清除缓存
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
 * 0xB2	交通灯A	0x00	0x00	0x00
 * 0xB3	交通灯B	0x00	0x00	0x00
 * 0xB4	静态标志物1	0x00	0x00	0x00
 * 0xB5	静态标志物2	0x00	0x00	0x00
 * 0xB6				
 * 0xB7	数据算法压缩A	01A	10B	11C
 * 0xB8	数据算法压缩B	01A	10B	11C
 * 0xB9	破损车牌1	车牌[0]	车牌[1]	车牌[2]
 * 0xBA	破损车牌2	车牌[3]	车牌[4]	车牌[5]
 * 0xBB	路灯初始档位	初始档位	0x00	0x00
 * 0xBC	主车空闲中	0x00	0x00	0x00
 * 0xCD	上传调试信息1	ASCLL码[0]	ASCLL码[1]	ASCLL码[2]
 * 0XCE	上传调试信息2	十进制[0]	十进制[1]	十进制[2]
 * 0XCF	上传调试信息3	十六进制[0]	十六进制[1]	十六进制[2]
 * @description: 主车发送数据到Wifi
 * @param {uint8_t} cmd 主指令
 * @param {uint8_t} cmd1 副指令x
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
 * @description: 向平板上传调试信息
 * @param {uint8_t} mode 1.上传ASCALL码 2.上传十进制 3.上传十六进制
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
 * @description: 图像识别请求并防止超时操作 模板 
 * @param {uint8_t} cmd 主指令
 * @param {uint8_t} *flag 标志地址
 * @param {uint32_t} time 等待周期 单位：秒
 * @return {*}1.接收成功 0.超时
 */
uint8_t OWiFi_cmd(uint8_t cmd, uint32_t time)
{
	uint8_t r = 1, t = 0; //_num=2;
	OWiFi_END = 1;		  //重置接收状态
	Wifi_request = 0;	  //重置wifi确认请求
	//Wifi_Rx_flag_num=0;//利用接收次数再次发送请求
	TIM_Cmd(TIM5, ENABLE);	  //开启识别接收
	OWiFi_Send(cmd, 0, 0, 0); //发送一次识别请求
	while (OWiFi_END == 1)	  //等待回传结果
	{
		t++;
		if (t >= time) //超时跳出
		{
			r = 0;
			break;
		}
		if (OWiFi_END == 0) //接收完毕跳出
		{
			break;
		}
		delay_ms(500);
		delay_ms(500);

		if (Wifi_request == 0 && t > 2) //若为平板未确认请求则再次发送
		{
			OWiFi_Send(cmd, 0, 0, 0); //发送一次识别请求
									  //Send_Debug_string("wifi_error:1");
		}
	}
	TIM_Cmd(TIM5, DISABLE);
	return r;
}

/**
 * @description: 请求识别静态标志物1
 * @param {uint32_t} time  等待周期 单位：秒
 * @return {*}0.超时 1.完成
 */
uint8_t OWiFi_QRCode1(uint32_t time)
{

	return OWiFi_cmd(0xB4, time);
}

/**
 * @description: 请求识别静态标志物2
 * @param {uint32_t} time  等待周期 单位：秒
 * @return {*}0.超时 1.完成
 */
uint8_t OWiFi_QRCode2(uint32_t time)
{

	return OWiFi_cmd(0xB5, time);
}

/**
 * @description: TFT识别 车牌 图像等
 * @param {uint8_t} AorB 'A'=TFTA 'B'=TFTB
 * （图形识别结果 OWifi_TFTShape[0]：矩形 [1]：圆形  [2]：三角  [3]: 菱形 [4]：五角星）
 * @param {uint32_t} time  等待周期 单位：秒
 * @return {*}0.超时 1.完成
 */
uint8_t OWiFi_TFT(uint8_t AorB, uint32_t time)
{
	return OWiFi_cmd(AorB == 'A' ? 0xB0 : 0xB1, time); //请求TFT识别车牌
}
/**
 * @description: 请求交通灯识别 并向标志发送确认识别结果
 * @param {uint8_t} AorB 'A'=交通灯A 'B'=交通灯B
 * @return {*}
 */
uint8_t OWiFi_JTlight(uint8_t AorB)
{
	if (AorB == 'A')
	{
		OFlag_JT_cmd(1, 0); //交通灯A进入识别模式
		delay_ms(200);
		OFlag_JT_cmd(1, 0); //交通灯A进入识别模式
		delay_ms(200);
		OFlag_JT_cmd(1, 0);		 //交通灯A进入识别模式
		if (OWiFi_cmd(0xB2, 15)) //请求识别成功
		{
			OFlag_JT_cmd(1, OWifi_JT); //A确认结果
			delay_ms(200);
			OFlag_JT_cmd(1, OWifi_JT); //A确认结果
			delay_ms(200);
			OFlag_JT_cmd(1, OWifi_JT); //A确认结果
			return 1;
		}
		OFlag_JT_cmd(1, 1); //识别失败忙猜结果
		delay_ms(100);
		OFlag_JT_cmd(1, 1); //识别失败忙猜结果
	}
	else
	{
		OFlag_JT_cmd(0, 0); //交通灯B进入识别模式
		delay_ms(200);
		OFlag_JT_cmd(0, 0); //交通灯B进入识别模式
		delay_ms(200);
		OFlag_JT_cmd(0, 0);		 //交通灯B进入识别模式
		if (OWiFi_cmd(0xB3, 15)) //请求识别成功
		{
			OFlag_JT_cmd(0, OWifi_JT); //B确认结果
			delay_ms(200);
			OFlag_JT_cmd(0, OWifi_JT); //B确认结果
			delay_ms(200);
			OFlag_JT_cmd(0, OWifi_JT); //B确认结果
			return 1;
		}
		OFlag_JT_cmd(0, 1); //识别失败忙猜结果
		delay_ms(100);
		OFlag_JT_cmd(0, 1); //识别失败忙猜结果
	}
	return 0;
}

/**
 * @description: Wifi上传数据，请求压缩算法计算
 * @param {uint8_t} *data  上传算法数据
 * @param {uint32_t} time 等待周期 单位：秒
 * @return {*}1.接收成功 0.超时
 */
uint8_t OWiFi_CodeCalc(uint8_t *data, uint32_t time)
{
	uint8_t r = 1, t = 0;
	OWiFi_END = 1;								 //重置接收状态
	Wifi_request = 0;							 //重置wifi确认请求
	TIM_Cmd(TIM5, ENABLE);						 //开启识别接收
	OWiFi_Send(0xb7, data[0], data[1], data[2]); //上传信息到平板处理算法
	delay_ms(100);
	OWiFi_Send(0xb8, data[3], data[4], data[5]); //上传信息到平板处理算法
	while (OWiFi_END == 1)						 //等待回传结果
	{
		t++;
		if (t >= time) //超时跳出
		{
			r = 0;
			break;
		}
		if (OWiFi_END == 0) //接收完毕跳出
		{
			break;
		}
		delay_ms(500);
		delay_ms(500);

		if (Wifi_request == 0 && t > 2) //若为平板未确认请求则再次发送
		{
			OWiFi_Send(0xb7, data[0], data[1], data[2]); //上传信息到平板处理算法
			delay_ms(100);
			OWiFi_Send(0xb8, data[3], data[4], data[5]); //上传信息到平板处理算法
		}
	}
	TIM_Cmd(TIM5, DISABLE);
	return r;
}

/**
 * @description: WiFi请求发送Zigbee消息
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
 * @description: WiFi、Zigbee接收回调
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
				Wifi_request = 1;		 //确认
				if (Wifi_Rx_Buf[3] == 1) //识别结束
				{
					OWiFi_END = 0;
				}
				break;
			case 0xA2: //二维码A
				OWifi_CRCode[0] = Wifi_Rx_Buf[3];
				OWifi_CRCode[1] = Wifi_Rx_Buf[4];
				OWifi_CRCode[2] = Wifi_Rx_Buf[5];
				break;
			case 0xA3: //二维码B
				OWifi_CRCode[3] = Wifi_Rx_Buf[3];
				OWifi_CRCode[4] = Wifi_Rx_Buf[4];
				OWifi_CRCode[5] = Wifi_Rx_Buf[5];
				break;
			case 0x40: //TFT车牌1
				OWifi_TFTCP[0] = Wifi_Rx_Buf[3];
				OWifi_TFTCP[1] = Wifi_Rx_Buf[4];
				OWifi_TFTCP[2] = Wifi_Rx_Buf[5];
				break;
			case 0x41: //TFT车牌2
				OWifi_TFTCP[3] = Wifi_Rx_Buf[3];
				OWifi_TFTCP[4] = Wifi_Rx_Buf[4];
				OWifi_TFTCP[5] = Wifi_Rx_Buf[5];
				break;
			case 0x45: //图形识别1
				OWifi_TFTShape[0] = Wifi_Rx_Buf[3];
				OWifi_TFTShape[1] = Wifi_Rx_Buf[4];
				OWifi_TFTShape[2] = Wifi_Rx_Buf[5];

				break;
			case 0x46: //图形识别2
				OWifi_TFTShape[3] = Wifi_Rx_Buf[3];
				OWifi_TFTShape[4] = Wifi_Rx_Buf[4];
				break;
			case 0xAE: //数据算法压缩A
				OWifi_CalcCode[0] = Wifi_Rx_Buf[3];
				OWifi_CalcCode[1] = Wifi_Rx_Buf[4];
				OWifi_CalcCode[2] = Wifi_Rx_Buf[5];
				break;
			case 0xAF: //数据算法压缩B
				OWifi_CalcCode[3] = Wifi_Rx_Buf[3];
				OWifi_CalcCode[4] = Wifi_Rx_Buf[4];
				OWifi_CalcCode[5] = Wifi_Rx_Buf[5];
				break;
			case 0xA5: //识别TFT交通标志
				OWifi_TFTJTFLAG	= Wifi_Rx_Buf[3];
				if(OWifi_TFTJTFLAG<2 && OWifi_TFTJTFLAG>7)//若其他结果则赋值为2
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
			if (Wifi_Rx_Buf[2] == 0x02) //交通灯识别
			{
				OWifi_JT = Wifi_Rx_Buf[3];
			}
		}
		else
		{
			MasterCar_TaskReceiveThread(); //查看是否为其他线程(安卓向主车发送消息，以主车作为zigbee转发请求控制标志物)
		}
	}
}

/**
 * @description: WIFI操控主车
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
			MasterCar_SmartRunMP(buf[3], buf[4] + buf[5] * 0x100); // 主车前进
			break;
		case 0x03:
			MasterCar_BackMP(buf[3], buf[4] + buf[5] * 0x100); // 主车后退
			break;
		case 0x04:
			if (buf[3] == 0)
			{
				// 主车左转
			}
			else
			{
				MasterCar_LeftMP(buf[3], buf[4] + buf[5] * 0x100);
			}
			break;
		case 0x05:
			if (buf[3] == 0)
			{
				// 主车右转
			}
			else
			{
				MasterCar_RightMP(buf[3], buf[4] + buf[5] * 0x100);
			}
			break;
		case 0x06:
			MasterCar_SmartRun(buf[3]); // 主车循迹
			break;
		case 0xA0: //启动主车
			Set_Flag_Task(0x08);
			break;
		case 0XAA: //接收烽火台数据1
			OWifi_alarm[0] = Wifi_Rx_Buf[3];
			OWifi_alarm[1] = Wifi_Rx_Buf[4];
			OWifi_alarm[2] = Wifi_Rx_Buf[5];
			break;
		case 0XAB: //接收烽火台数据2
			OWifi_alarm[3] = Wifi_Rx_Buf[3];
			OWifi_alarm[4] = Wifi_Rx_Buf[4];
			OWifi_alarm[5] = Wifi_Rx_Buf[5];
			break;
		}
	}
}
