#include "new_syn.h"
#include "stm32f4xx.h"
#include "delay.h"
#include <string.h>
#include "canp_hostcom.h"
#include "function.h"

uint8_t USART6_receiving = 0;											  //接收次数
uint8_t USART6_Back[9];													  //接收的数据
uint8_t USART6_Flag;													  //UART6接收完成标志位
uint8_t TIM14_Flag = 0;													  //TIM14单次定时标志位
uint8_t TIM14_Overtime = 0;												  //TIM14多次定时标志位
uint8_t Start_Buf2[] = {0xFD, 0x00, 0x02, 0x10, 0x04};					  //单次测试 使用用户词典2
uint8_t Stop_Buf[] = {0xFD, 0x00, 0x01, 0x11};							  //停止语音识别
uint8_t SYN_Random[8] = {0x55, 0x06, 0x20, 0x01, 0x00, 0x00, 0x21, 0xbb}; //随机语音
uint8_t Setting[11] = {0XFD, 0X00, 0X07, 0X1E, 0X03, 0X0F, 0XA0, 0X0F, 0XA0, 0X02};
uint8_t Uploading[8] = {0xAF, 0x06, 0x00, 0x02, 0x00, 0x00, 0x01, 0xBF}; //上传评分中断
/*********************************************************************
音 调：     调[=tiao2]光档[=dang3]"
发音人选择：[m3]你好[m3]  晓玲
						[m51]你好[m3] 尹小坚
						[m52]你好[m3] 尹小强
						[m53]你好[m3] 田蓓蓓
						[m54]你好[m3] 唐老鸭
						[m55]你好[m3] 小燕子
数字处理：	[n0]拨打123456，123公斤[d] 自动判断
						[n1]拨打123456，123公斤[d] 强制合成号码数字串（一个一个读）
						[n2]拨打123456，123公斤[d] 强制合成数值数字串（大小）
静音：      你好[p1000]再见 读完你好后延时1s在读再见
读姓氏：    [r0]单位的单小虎来了[d]	自动判断姓名读音
						[r1]单位的单小虎来了[d]	强制开头按姓名读
						[r2]单位的单小虎来了[d]	仅紧跟的汉字强制按姓名读
语速：		  [s5]你好[d] 5级语速播放
						[s8]你好[d] 8级语速播放
						[s2]你好[d] 2级语速播放
语调：		  [t5]你好[d] 5级语调播放
						[t8]你好[d] 8级语调播放
						[t2]你好[d] 2级语调播放
音量调节：  [v5]你好[d] 5级音量播放
						[v8]你好[d] 8级音量播放
						[v2]你好[d] 2级音量播放
提示音：    [x0]sounda msga[d]	单个单个读英文字母		 			
						[x1]sounda msga[d]  提示音
号码1：			[y0]1008611[d]	幺零零八六幺幺
						[y1]1008611[d]	一零零八六一一
发音风格：  [f0]一二三四五[d] 一字一顿
						[f0]一二三四五[d] 平铺直叙
读标点符号：[b0]欢迎光临！[d] 不读感叹号
						[b1]欢迎光临！[d] 欢迎光临 沟通好
**********************************************************************/

/**********************************************************************
 * 函 数 名 ： 	USART6初始化
 * 参    数 ：  baudrate：波特率
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void USART6_INIT(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_TypeDefStructure;	  //GPIO结构体定义
	USART_InitTypeDef USART_TypeDefStructure; //USART结构体定义
	NVIC_InitTypeDef NVIC_InitStructure;	  //NVIC结构体定义

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //开启GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //开启USART6时钟

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //USART6复用到PC6
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //USART6复用到PC7

	//PC6->Tx    PC7->RX
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //定义PC6 PC7
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;			  //复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;		  //推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;			  //上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;	  //100MHz
	GPIO_Init(GPIOC, &GPIO_TypeDefStructure);				  //GPIO初始化
	//USART6
	USART_TypeDefStructure.USART_BaudRate = baudrate; //波特率
	USART_TypeDefStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;								   //无硬件控制流
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //接收与发送模式
	USART_TypeDefStructure.USART_Parity = USART_Parity_No;			   //无校验位
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;		   //停止位1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;	   //数据位8位
	USART_Init(USART6, &USART_TypeDefStructure);					   //USART初始化

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); //开启相关中断

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;		  //串口6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;		  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //NVIC初始化
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART6, ENABLE);				  //使能USART6
	USART_ClearFlag(USART6, USART_FLAG_TC);	  //清除发送完成标志位
	USART_ClearFlag(USART6, USART_FLAG_RXNE); //清除接收完成标志位
}

/**********************************************************************
 * 函 数 名 ： 	SYN初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  （SYN7318复位引脚初始化）
*****************************************************************/
void SYN_Init(void)
{
	USART6_INIT(115200);				 //USART6初始化
	TIM14_Int_Init(10000 - 1, 8400 - 1); //TIM14初始化
	USART6_SendChar(0xFD);				 //发送帧头（否则第一次数据一定是错的）
#if versions == 0						 //标准任务板
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // 默认为高电平
#endif
#if versions == 1 //2020 G1
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//PC13 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_13); // 默认为高电平
#endif
#if versions == 2 //2020 G2
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // 默认为高电平
#endif
#if versions == 3 //2020 G3
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//PG8 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8); // 默认为高电平
#endif
#if versions == 4 //2020 G0
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9); // 默认为高电平
#endif
#if versions == 5 //2021 Z1
	GPIO_InitTypeDef GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//PG8 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;  // 复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_TypeDefStructure);

	GPIO_SetBits(GPIOG, GPIO_Pin_8); // 默认为高电平
#endif
	//USART6_SendString(Setting,10);
}

/**********************************************************************
 * 函 数 名 ： 	USART6发送一个字节函数
 * 参    数 ：  SYN_Data：需要发送的数据（字节）
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void USART6_SendChar(uint8_t SYN_Data)
{
	USART_SendData(USART6, SYN_Data); //向串口6发送数据
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) != SET)
		; //等待发送结束
}

/**********************************************************************
 * 函 数 名 ： 	USART6发送字符串函数
 * 参    数 ：   SYN_Data：需要发送的数据（字符串） size：大小
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void USART6_SendString(uint8_t *SYN_Data, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		USART6_SendChar(SYN_Data[i]); //循环发送单字节
	}
}

/**********************************************************************
 * 函 数 名 ： 	SYN语音合成播放函数
 * 参    数 ：  Pst：存放要合成播放的文本的数组名
 * 返 回 值 ：  无
 * 全局变量 ：  无						 
 * 备    注 ：  SYN_TTS("你好");
*****************************************************************/
void SYN7318_Play(char *Pst)
{
	uint8_t Length;
	uint8_t Frame[5]; //保存发送命令的数组

	Length = strlen((char *)Pst);
	Frame[0] = 0xFD;	   //帧头
	Frame[1] = 0x00;	   //数据长度
	Frame[2] = Length + 2; //数据长度
	Frame[3] = 0x01;	   //语音合成播放命令
	Frame[4] = 0x00;	   //播放编码格式为“GB2312”

	TIM_Cmd(TIM14, ENABLE);					   //使能定时器14（开启20s超时检测）
	USART6_SendString(Frame, 5);			   //发帧头+数据长度+命令
	USART6_SendString((uint8_t *)Pst, Length); //发送数据
	while (!(USART6_Back[3] == 0x41))		   //模块接收成功判断
	{
		//Send_Debug_HEX(USART6_Back[3]);
		if (TIM14_Overtime == 1) //超时
		{
			break;
		}
	}
	while (!(USART6_Back[3] == 0x4f)) //模块空闲判断
	{
		Send_Debug_HEX(USART6_Back[3]);
		if (TIM14_Overtime == 1) //超时
		{
			break;
		}
	}
	TIM_Cmd(TIM14, DISABLE); //失能定时器14（关闭20s超时检测）
	TIM14_Flag = 0;			 //清除定时器14计时标志位
	delay_ms(200);
}

 //SYN7318
/**********************************************************************
 * 函 数 名 ： 	USART6中断服务程序
 * 参    数 ：  baudrate：波特率
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void USART6_IRQHandler(void)
{
	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET) //溢出中断
	{
		Send_Debug_HEX(USART_ReceiveData(USART6));//测试

		
		USART6_Back[USART6_receiving] = USART_ReceiveData(USART6); //USART6收到的数据获取
		if (USART6_Back[0] == 0xFC)								   //判断标准帧头
		{
			USART6_receiving += 1;
			if (USART6_receiving > USART6_Back[2] + 2)
			{
				//				 Send_Debug_Info((uint8_t*)"\n",2);//测试
				USART6_Flag = 1;
				USART6_receiving = 0;
			}
		}
		else if (USART6_Back[USART6_receiving] == 0x55) //判断复位数据
		{
			USART6_receiving += 1;
			if (USART6_receiving > 4)
			{
				//					Send_Debug_Info((uint8_t*)"\n",2);//测试
				USART6_Flag = 1;
				USART6_receiving = 0;
			}
		}
		else //帧错误
		{
			USART6_receiving = 0;
		}
	}
	USART_ClearITPendingBit(USART6, USART_IT_RXNE); //清除中断标志位
}


/**********************************************************************
 * 函 数 名 ： 	模块状态查询函数
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void SYN_Status_Query(void)
{
	uint8_t Frame[4]; // 保存发送命令的数组

	Frame[0] = 0xFD; // 帧头
	Frame[1] = 0x00; //数据长度
	Frame[2] = 0x01; //数据长度
	Frame[3] = 0x21; // 状态查询命令

	TIM_Cmd(TIM14, ENABLE);			  //使能定时器14（开启20s超时检测）
	USART6_SendString(Frame, 4);	  //发送模块状态查询指令
	while (!(USART6_Back[3] == 0x41)) //模块接收成功判断
	{
		//模块处于语言识别，语音唤醒，语音合成或收到不能识别的命令帧
		if (USART6_Back[3] == 0x42 || USART6_Back[3] == 0x45 || USART6_Back[3] == 0x45)
		{
			SYN73118_RST();				 //语音模块复位
			USART6_SendString(Frame, 4); //发送模块状态查询指令
		}
		if (TIM14_Overtime == 1) //超时
		{
			break;
		}
	}
	TIM_Cmd(TIM14, DISABLE); //失能定时器14（关闭20s超时检测）
	TIM14_Flag = 0;			 //清除定时器14计时标志位
}

/**********************************************************************
 * 函 数 名 ： 	语音识别
 * 参    数 ：  无
 * 返 回 值 ：  识别到的指令
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t SYN_Extern(void)
{
#if Voice == 0

	uint8_t Order = 0;
	TIM_Cmd(TIM14, ENABLE); //使能定时器14（开启20s超时检测）
	SYN7318_Play("开始");
	SYN_Status_Query();				//查询模块状态
	USART6_SendString(Stop_Buf, 4); //停止语音识别
	
	for (int i = 0; i < 4; i++)
	{
		USART6_SendString(Start_Buf2, 5); //发送开始语音识别
		TIM_Cmd(TIM14, ENABLE);			  //使能定时器14（开启20s超时检测）
		while (!(USART6_Back[3] == 0x41)) //模块接收成功判断
		{
			if (TIM14_Overtime == 1) //超时
			{
				return 0xFF; //超时返回值
			}
		}
		TIM14_Flag = 0;					//清除定时器14计时标志位
		TIM_Cmd(TIM14, DISABLE);		//失能定时器14（关闭20s超时检测）
		for (uint8_t i = 0; i < 9; i++) //清空数据缓存区
		{
			USART6_Back[i] = 0;
		}
		Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
		Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
		delay_ms(100);
		delay_ms(500); //延时越高效果越好
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500); //但是不要太久

		if (USART6_Back[3] == 0x01)
		{
			switch (USART6_Back[8]) //判断识别到的内容
			{
			case 0x01:
			{
				SYN7318_Play("美好生活");
				i = 5;
				Order = 0x02;
				break;
			}
			case 0x02:
			{
				SYN7318_Play("秀丽山河");
				i = 5;
				Order = 0x03;
				break;
			}
			case 0x03:
			{
				SYN7318_Play("追逐梦想");
				Order = 0x04;
				i = 5;
				break;
			}
			case 0x04:
			{
				SYN7318_Play("扬帆起航");
				Order = 0x05;
				i = 5;
				break;
			}
			case 0x05:
			{
				SYN7318_Play("齐头并进");
				Order = 0x06;
				i = 5;
				break;
			}
			default:
				break;
			}
		}
		else
		{
			SYN7318_Play("识别失败");
			Order = 0x01;
		}
		USART6_Back[8] = 0;				//清除数据缓存
		USART6_SendString(Stop_Buf, 4); //停止语音识别
	}
	Uploading[2] = Order;
	Send_ZigbeeData_To_Fifo(Uploading, 8);
	Send_ZigbeeData_To_Fifo(Uploading, 8);

	TIM_Cmd(TIM14, DISABLE); //失能定时器14（关闭20s超时检测）
	TIM14_Flag = 0;			 //清除定时器14计时标志位
	delay_ms(200);
	return Order;
#endif
#if Voice == 1
	return ASR(); //小创语音
#endif
}

/**********************************************************************
 * 函 数 名 ：  通用定时器14中断初始化
 * 参    数 ：  arr：自动重装值 psc：时钟预分频数
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us. Ft=定时器工作频率,单位:Mhz
*****************************************************************/
void TIM14_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; //定时器结构体定义
	NVIC_InitTypeDef NVIC_InitStructure;			   //NVIC结构体定义

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); //使能TIM14时钟

	TIM_TimeBaseInitStructure.TIM_Period = arr;						//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStructure); //初始化TIM14

	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); //允许定时器14更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn; //定时器14中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	  //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;			  //子优先级10
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				  //使能中断请求
	NVIC_Init(&NVIC_InitStructure);								  //中断初始化
}

/**********************************************************************
 * 函 数 名 ：  定时器14中断服务函数
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET) //溢出中断
	{
		TIM14_Flag += 1;
		if (TIM14_Flag > 20) //20s定时
		{
			TIM14_Overtime = 1; //清除标志位
			TIM14_Flag = 0;		//清除标志位
		}
	}
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update); //清除中断标志位
}

/**********************************************************************
 * 函 数 名 ：  语音模块复位
 * 参    数 ：  无
 * 返 回 值 ：  复位成功（1） 复位失败（0）
 * 全局变量 ：  无
 * 备    注 ：  （语音模块从高拉低复位）
*****************************************************************/
uint8_t SYN73118_RST(void)
{
	SYN_RST = SET; //置高
	delay_ms(10);
	SYN_RST = RESET; //置低
	delay_ms(100);
	SYN_RST = SET;					  //置高
	TIM_Cmd(TIM14, ENABLE);			  //使能定时器14（开启20s超时检测）
	while (!(USART6_Back[3] == 0x55)) //复位成功判断
	{
		if (TIM14_Overtime == 1) //超时
		{
			return 0;
		}
	}
	while (!(USART6_Back[3] == 0x4A)) //检测模块空闲
	{
		if (TIM14_Overtime == 1) //超时
		{
			return 0;
		}
	}
	USART6_Back[3] = 0;
	TIM_Cmd(TIM14, DISABLE); //失能定时器14（关闭20s超时检测）
	TIM14_Flag = 0;			 //清除定时器14计时标志位
	return 1;
}

/**********************************************************************
 * 函 数 名 ：  小创语音
 * 参    数 ：  无
 * 返 回 值 ：  语音编号
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t ASR(void)
{
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xFA);
	USART6_SendChar(0xA1);
	Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
	Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
	uint16_t Counter_Fifo = 0;
	uint8_t Temp_flag = 0;
	USART6_Flag = 0;
	while (1)
	{
		if (USART6_Flag == 1)
		{
			if (USART6_Back[1] == 0x02 && USART6_Back[3] == 0x00)
			{
				switch (USART6_Back[2])
				{
				case 1:
				{
					Uploading[2] = 2;
					break;
				}
				case 2:
				{
					Uploading[2] = 3;
					break;
				}
				case 3:
				{
					Uploading[2] = 4;
					break;
				}
				case 4:
				{
					Uploading[2] = 5;
					break;
				}
				case 5:
				{
					Uploading[2] = 6;
					break;
				}
				}
				Send_ZigbeeData_To_Fifo(Uploading, 8);
				Send_ZigbeeData_To_Fifo(Uploading, 8);
				return Uploading[2];
			}
		}
		delay_ms(100);
		Counter_Fifo += 1;
		if (Counter_Fifo > 30)
		{
			Counter_Fifo = 0;
			if (Temp_flag >= 4)
			{
				break;
			}
			Temp_flag += 1;
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xFA);
			USART6_SendChar(0xA1);
			Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
			Send_ZigbeeData_To_Fifo(SYN_Random, 8); // 随机语音
		}
	}
	return 0;
}
