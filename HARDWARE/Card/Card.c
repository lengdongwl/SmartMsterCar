/**********************************************************************
 * 读卡并过路障的相关函数
 * 1.0 21/3/17    1837   2285
 

**********************************************************************/
#include "Card.h"
#include "function.h"
#include "canp_hostcom.h"
#include "roadway_check.h"
#include "delay.h"
#include "drive.h"
#include "Voluntarily.h"
#include "rc522.h"
#include "cba.h"
#include "stdio.h"

uint8_t Card_Speed = 36;									//前进速度值
uint8_t RXRFID[16];											//RFID检测到的数据
uint8_t RFID_RH8[8];										//读出数据高8位
uint8_t RFID_RL8[8];										//读出数据低8位
uint8_t TXRFID[16] = {"ASDASDASAADASAAA"};					//写卡数据
uint8_t Read_Car_Address = 0;								//读卡的块地址
uint8_t KEY_Open[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //密钥
uint8_t s = 0x01;											//RFID写入数据位
uint8_t read_card_quantity;									//RFID检测次数
uint8_t RFID_ONE_detection = 0;								//单RFID卡检测次数

/*密钥更改0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x80,0x69,0xb0,0x01,0xb2,0xb3,0xb4,0xb5;//写卡数据*/

uint16_t terrain;		//地形检测控制
uint16_t MP_A;			//前码盘值
uint16_t MP_B;			//后码盘值
uint16_t detection;		//地形检测状态位
uint16_t MP_difference; //码盘差值

/**********************************************************************
 * 函 数 名 ：  单段检测（路障-读卡-读卡）				 路障
 * 参    数 ：  单段路距离								|
 * 返 回 值 ：  卡位置                                  |
 * 全局变量 ：  无                                 |—— —— ——|
 * 备    注 ：  无                                       ^  ^	
 * 修改记录 ：  V1                                       卡 卡
*****************************************************************/
uint8_t Card_One(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8分之一段距离
	uint8_t number;							// 检测参数位
	terrain = 1;							// 卡检测-地形检测 开启
	MP_A = CanHost_Mp;						// 获取当前码盘值
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // 主车循迹
		Full_STOP();		   // 停下来
		delay_ms(200);
		MP_B = CanHost_Mp;						  // 获取当前码盘值
		MP_difference = MP_B - MP_A;			  // 码盘差值
												  // 						Send_Debug(MP_difference);						 // 上传调试信息（屏幕）
		if (MP_difference < (short_distance * 3)) // 地形
		{
			delay_ms(100);
			SYN_Play("[s10]路障一[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //卡
		{
			delay_ms(100);
			SYN_Play("[s10]卡1[d]");
			RFID_detection_motion(); // 自动读卡
			Car_Go(Card_Speed, 250); // 主车前进
			number = 0x01;			 // 卡1被读取标志位
		}
		else if ((short_distance * 7) < MP_difference) //卡2 或 中心
		{
			if (crossroad == 1) //卡2
			{
				delay_ms(100);
				SYN_Play("[s10]卡2[d]");
				RFID_detection_motion(); // 自动读卡
				terrain = 0;			 // RFID检测-地形检测 关闭
				return 0x02;			 // 卡2被读取标志位
			}
			else //中心
			{
				break; // 退出到正常状态
			}
		}
	}
	crossroad = 0; // 十字路口检测到白卡状态位
	detection = 0; // 地形检测状态位
	terrain = 0;   // RFID检测-地形检测 关闭
	return number;
}

/**********************************************************************
 * 函 数 名 ： 	寻终点的卡
 * 参    数 ：  无										|---|
 * 返 回 值 ：  无                                          ^
 * 全局变量 ：  无                                          卡
 * 备    注 ：  没有卡就是正常的循迹停止
*****************************************************************/
uint8_t Card_Two(void)
{
	terrain = 1;
	crossroad = 0;
	Car_Track(Card_Speed); // 主车循迹
	Full_STOP();		   // 停下来
	delay_ms(200);
	if (crossroad == 1)
	{
		RFID_detection_motion(); //RFID微动检测
		crossroad = 0;
		terrain = 0; // RFID检测-地形检测 关闭
		return 1;
	}
	terrain = 0;
	return 0;
}

/**********************************************************************
 * 函 数 名 ：  双段检测（路障-读卡-路障-读卡-路障-读卡）		路障 路障 路障 
 * 参    数 ：  单段路距离                                     |   |    |
 * 返 回 值 ：  卡位置                                         |   |    |
 * 全局变量 ：  无                                         |—— —— ——|—— —— ——|
 * 备    注 ：  无                                              ^   ^   ^   ^
 * 修改记录 ：  V1										        卡  卡   卡  卡
*****************************************************************/
uint8_t Card_Three(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8分之一段距离
	uint8_t number;							// 检测参数位
	terrain = 1;							// 卡检测-地形检测 开启
	MP_A = CanHost_Mp;						// 获取当前码盘值
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // 主车循迹
		Full_STOP();		   // 停下来
		delay_ms(200);
		MP_B = CanHost_Mp;						  // 获取当前码盘值
		MP_difference = MP_B - MP_A;			  // 码盘差值
												  // 						Send_Debug(MP_difference);						 // 上传调试信息（屏幕）
		if (MP_difference < (short_distance * 3)) // 路障一
		{
			delay_ms(100);
			SYN_Play("[s10]路障一[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //卡二
		{
			delay_ms(100);
			SYN_Play("卡一");
			RFID_detection_motion(); // 自动读卡
			Car_Go(Card_Speed, 250); // 主车前进
			number = 0x01;			 // 卡1被读取标志位
		}
		else if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //路障二
		{
			delay_ms(100);
			SYN_Play("[s10]路障二[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 9)) //卡2 或 中心
		{
			if (crossroad == 1) //卡2
			{
				delay_ms(100);
				SYN_Play("[s10]卡二[d]");
				RFID_detection_motion(); // 自动读卡
				Car_Go(Card_Speed, 250); // 主车前进
				number = 0x02;			 // 卡2被读取标志位
			}
		}
		else if ((short_distance * 9) < MP_difference && MP_difference < (short_distance * 11)) //路障三
		{
			delay_ms(100);
			SYN_Play("[s10]路障三[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 11) < MP_difference && MP_difference < (short_distance * 13)) //卡三
		{
			delay_ms(100);
			SYN_Play("[s10]卡三[d]");
			RFID_detection_motion(); // 自动读卡
			Car_Go(Card_Speed, 250); // 主车前进
			number = 0x03;			 // 卡1被读取标志位
		}
		else if ((short_distance * 15) < MP_difference) //卡3 或 中心
		{
			if (crossroad == 1) //卡3
			{
				delay_ms(100);
				SYN_Play("[s10]卡四[d]");
				RFID_detection_motion(); // 自动读卡
				terrain = 0;			 // RFID检测-地形检测 关闭
				return 0x04;			 // 卡2被读取标志位
			}
			else //中心
			{
				break; // 退出到正常状态
			}
		}
	}
	crossroad = 0; // 十字路口检测到白卡状态位
	detection = 0; // 地形检测状态位
	terrain = 0;   // RFID检测-地形检测 关闭
	return number;
}

/**********************************************************************
 * 函 数 名 ：  双段中路障检测（路障）													路障 
 * 参    数 ：  单段路距离                                                               |    
 * 返 回 值 ：  卡位置                                                                   |    
 * 全局变量 ：  无                                                              |—— —— ——|—— —— ——|
 * 备    注 ：  无		
 * 修改记录 ：  V1	
*****************************************************************/
uint8_t Card_Four(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8分之一段距离
	uint8_t number;							// 检测参数位
	terrain = 1;							// 卡检测-地形检测 开启
	MP_A = CanHost_Mp;						// 获取当前码盘值
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // 主车循迹
		Full_STOP();		   // 停下来
		delay_ms(200);
		MP_B = CanHost_Mp;																  // 获取当前码盘值
		MP_difference = MP_B - MP_A;													  // 码盘差值
																						  // 						Send_Debug(MP_difference);						 // 上传调试信息（屏幕）
		if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //路障二
		{
			delay_ms(100);
			SYN_Play("[s10]路障[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 15) < MP_difference)
		{
			break; // 退出到正常状态
		}
	}
	crossroad = 0; // 十字路口检测到白卡状态位
	detection = 0; // 地形检测状态位
	terrain = 0;   // RFID检测-地形检测 关闭
	return number;
}

/**********************************************************************
 * 函 数 名 ：  单段检测停（路障-读卡）				 路障
 * 参    数 ：  单段路距离                           |
 * 返 回 值 ：  卡位置                               |
 * 全局变量 ：  无                              |—— —— ——|
 * 备    注 ：  无                                    ^   ^	
 * 修改记录 ：  V1                                    卡   卡
*****************************************************************/
uint8_t Card_Five(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8分之一段距离
	uint8_t number;							// 检测参数位
	terrain = 1;							// 卡检测-地形检测 开启
	MP_A = CanHost_Mp;						// 获取当前码盘值
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // 主车循迹
		Full_STOP();		   // 停下来
		delay_ms(200);
		MP_B = CanHost_Mp;						  // 获取当前码盘值
		MP_difference = MP_B - MP_A;			  // 码盘差值
												  // 						Send_Debug(MP_difference);						 // 上传调试信息（屏幕）
		if (MP_difference < (short_distance * 3)) // 地形
		{
			delay_ms(100);
			SYN_Play("[s10]路障[d]");
			Detected = 1; // 地形是否检测到
			terrain = 0;  // RFID检测-地形检测 关闭
			return 0x00;
		}
		else if ((short_distance * 3) < MP_difference && MP_difference < (short_distance * 5)) //卡
		{
			delay_ms(100);
			SYN_Play("[s10]卡[d]");
			terrain = 0; // RFID检测-地形检测 关闭
			return 0x01; // 卡1被读取标志位
		}
		else if ((short_distance * 7) < MP_difference) //卡2 或 中心
		{
			break; // 退出到正常状态
		}
	}
	crossroad = 0; // 十字路口检测到白卡状态位
	detection = 0; // 地形检测状态位
	terrain = 0;   // RFID检测-地形检测 关闭
	return number;
}

/**********************************************************************
 * 函 数 名 ：  双段检测路障不读终点卡（路障-读卡）		    路障 
 * 参    数 ：  单段路距离                                 |    
 * 返 回 值 ：  卡位置                                     |    
 * 全局变量 ：  无                                |—— —— ——|—— —— ——|
 * 备    注 ：  无                                                  ^
 * 修改记录 ：  V1                                                  卡（终点卡停下不读）
*****************************************************************/
uint8_t Card_SIX(uint16_t distance)
{
	uint16_t short_distance = distance / 8; // 8分之一段距离
											//					uint8_t number;													 // 检测参数位
	terrain = 1;							// 卡检测-地形检测 开启
	MP_A = CanHost_Mp;						// 获取当前码盘值
	while (1)
	{
		delay_ms(200);
		Car_Track(Card_Speed); // 主车循迹
		Full_STOP();		   // 停下来
		delay_ms(200);
		MP_B = CanHost_Mp;																  // 获取当前码盘值
		MP_difference = MP_B - MP_A;													  // 码盘差值
																						  // 						Send_Debug(MP_difference);						 // 上传调试信息（屏幕）
		if ((short_distance * 5) < MP_difference && MP_difference < (short_distance * 7)) //路障二
		{
			delay_ms(100);
			SYN_Play("[s10]路障[d]");
			Car_Go(Card_Speed, 1300); // 主车前进
			Detected = 1;			  // 地形是否检测到
		}
		else if ((short_distance * 15) < MP_difference) //卡3 或 中心
		{
			if (crossroad == 1) //卡3
			{
				delay_ms(100);
				SYN_Play("[s10]卡不读[d]");
				terrain = 0; // RFID检测-地形检测 关闭
				return 0x01; // 卡2被读取标志位
			}
			else //中心
			{
				break; // 退出到正常状态
			}
		}
	}
	crossroad = 0; // 十字路口检测到白卡状态位
	detection = 0; // 地形检测状态位
	terrain = 0;   // RFID检测-地形检测 关闭
	return 0;
}

/**********************************************************************
 * 函 数 名 ：  测量单段路距离
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  打印单段路距离 
*****************************************************************/
void Obtain_section_distance(void)
{
	uint16_t Parameter_A = 0, Parameter_B = 0;
	Car_Track(Card_Speed);				   //主车循迹
	Parameter_A = CanHost_Mp;			   //获取当前码盘值
	Car_Track(Card_Speed);				   //主车循迹
	Parameter_B = CanHost_Mp;			   //获取当前码盘值
	Send_Debug(Parameter_B - Parameter_A); //上传调试信息（屏幕）
}

/**********************************************************************
 * 函 数 名 ：  RFID微动检测
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void RFID_detection_motion(void)
{
	read_card_quantity = 0;
	while (Auto_Read_card(Read_Car_Address) == 0)
	{
		Car_Go(Card_Speed - 10, 50); //主车前进
		delay_ms(300);
		RFID_ONE_detection += 1;
		if (RFID_ONE_detection > 8)
		{
			//SYN_Play("[s10]读卡失败[d]");
			Car_Back(30, RFID_ONE_detection * 50); //主车后退
			RFID_ONE_detection = 0;
			read_card_quantity += 1;
			if (read_card_quantity >= 3)
			{
				break;
			}
		}
	}
	delay_ms(200);
	Car_Back(30, RFID_ONE_detection * 50); //主车后退
	RFID_ONE_detection = 0;
}

/**********************************************************************
 * 函 数 名 ：  读卡
 * 参    数 ：  无
 * 返 回 值 ：  读卡状态
 * 全局变量 ：  无
 * 备    注 ：  块地址计算（扇区*4+块？）
*****************************************************************/
uint8_t RFID_Card_One[16] = {0};   //读卡数据缓存区 1
uint8_t RFID_Card_Two[16] = {0};   //读卡数据缓存区 2
uint8_t RFID_Card_Three[16] = {0}; //读卡数据缓存区 3
uint8_t Read_Flag[3] = {0};		   //读卡标志位
uint8_t Card_Fifo[8] = {0};		   //调试信息缓存
uint8_t Auto_Read_card(uint8_t Block_address)
{

	if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
	{
		if (PcdAnticoll(SN) == MI_OK) // 防冲撞成功
		{
			if (PcdSelect(SN) == MI_OK) //选定此卡
			{
				if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY_Open, SN) == MI_OK) //验证密钥
				{
					if (PcdRead(Block_address, RXRFID) == MI_OK) //读卡
					{
						if (Read_Flag[0] == 0) //卡1
						{
							//sprintf((char*)Card_Fifo,"Sq:%d ",Read_Car_Address);//打印扇区数据块地址
							Send_Debug_Info(Card_Fifo, 8);
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_One[i] = RXRFID[i];
							}
							Read_Flag[0] = 1;
							Send_Debug_string("Card 1\n");
							//Send_Debug_Info((uint8_t*)"Card 1\n",8);
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_One[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_One[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
							//Send_Debug_Info((uint8_t*)"\n",1);
						}
						else if (Read_Flag[1] == 0) //卡2
						{
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_Two[i] = RXRFID[i];
							}
							Read_Flag[1] = 1;
							Send_Debug_string("Card 2\n");
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_Two[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_Two[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
						}
						else if (Read_Flag[2] == 0) //卡3
						{
							for (int i = 0; i < 16; i++)
							{
								RFID_Card_Three[i] = RXRFID[i];
							}
							Read_Flag[2] = 1;
							Send_Debug_string("Card 3\n");
							for (int i = 0; i < 8; i++)
							{
								RFID_RH8[i] = RFID_Card_Three[i];
							}
							for (int i = 0; i < 8; i++)
							{
								RFID_RL8[i] = RFID_Card_Three[i + 8];
							}
							Send_Debug_Info(RFID_RH8, 8);
							Send_Debug_Info(RFID_RL8, 8);
							Send_Debug_string("\n");
						}
						/*提示音*/
						MP_SPK = 1; //蜂鸣器开
						delay_ms(200);
						MP_SPK = 0; //蜂鸣器关
						SYN_Play("[s10]读卡成功[d]");
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

/**********************************************************************
 * 函 数 名 ：  写卡
 * 参    数 ：  块地址(0~63)
 * 返 回 值 ：  写卡状态
 * 全局变量 ：  无
 * 备    注 ：  块地址计算（扇区*4+块？）
*****************************************************************/
uint8_t Auto_write_card(uint8_t Block_address)
{

	if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
	{
		if (PcdAnticoll(SN) == MI_OK) // 防冲撞成功
		{
			if (PcdSelect(SN) == MI_OK) //选定此卡
			{
				if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY_Open, SN) == MI_OK) //验证扇区0的密钥A
				{
					if (PcdWrite(Block_address, TXRFID) == MI_OK) //读卡 块地址1
					{
						SYN_Play("[s10]写卡成功[d]");
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
