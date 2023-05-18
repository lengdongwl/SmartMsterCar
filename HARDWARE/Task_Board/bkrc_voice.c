/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "bkrc_voice.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "canp_hostcom.h"
#include "OperationFlag.h"
#include "math.h"
/* 变量定义 ---------------------------------------------------------*/
uint8_t uart6_data = 0;			// USART6 接收数据缓存
uint8_t uart6_flag = 0;			// USART6 接收数据时序
uint8_t UART6_RxData[8];		// USART6 接收数据缓存

uint8_t voice_falg = 0;		// 语音模块返回状态
uint8_t YY_Init[5] = {0xFD, 0x00, 0x00, 0x01, 0x01};
uint8_t Zigbee[8];           // Zigbee发送数据缓存

uint8_t start_voice_dis[5]= {0xFA,0xFA,0xFA,0xFA,0xA1};
uint8_t SYN7318_Flag = 0;           // SYN7318语音识别命令ID编号
uint8_t UART6_RxFlag=0;
/*******************************************************
功　能：初始化串口
参　数：无
返回值：无
********************************************************/
static void USART6_Hardware_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能 GPIOC 外设时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* 使能 USART6 外设时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* 配置 PC6/PC7 引脚复用映射 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    /* 将 PC6/PC7 引脚配置为复用功能模式（上拉） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// 端口模式 -> 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 端口输出类型 -> 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 端口输出速度 -> 高速 100MHz(30pF)
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  		// 端口上拉/下拉 -> 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* USART6 初始化配置 */
    USART_InitStructure.USART_BaudRate = 115200;					// 波特率设置
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 硬件流设置 -> 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收发模式设置 -> 接收+发送
    USART_InitStructure.USART_Parity = USART_Parity_No;				// 奇偶校验位设置 -> 无奇偶校验位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			// 停止位设置 -> 1位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 数据位长度 -> 8位数据格式
    USART_Init(USART6, &USART_InitStructure);

    /* 使能 USART6 中断 */
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);		// 接收数据寄存器不为空中断

    /* 设置 USART6 中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;				// 选择 IRQ 通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	// 抢占优先级设置
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;			// 响应优先级设置
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 启用 USART6 IRQ 通道
    NVIC_Init(&NVIC_InitStructure);

    /* 使能 USART6 */
    USART_Cmd(USART6, ENABLE);
}

/*******************************************************
功　能：串口中断函数
参　数：无
返回值：无
********************************************************/
void USART6_IRQHandler(void)
{
    /* 判断 USART6 是否触发指定中断 -> 接收数据寄存器不为空中断 */
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        uart6_data = USART_ReceiveData(USART6);		// 读取 USART6 数据寄存器

        if (uart6_flag == 0x00)
        {
            if (uart6_data == 0x55)				// 自定义数据帧头
            {
                uart6_flag = 0x01;
                UART6_RxData[0] = uart6_data;	// 帧头
                UART6_RxData[1] = 0x00;
                UART6_RxData[2] = 0x00;
                UART6_RxData[3] = 0x00;
                
            }
        }
        else if (uart6_flag == 0x01)
        {
            uart6_flag = 0x02;
            UART6_RxData[1] = uart6_data;		// 数据类型
            UART6_RxFlag=1;

        }
        else if(uart6_flag == 0x02)
        {
            uart6_flag = 0x03;
            UART6_RxData[2] = uart6_data;		// 状态标志
        }
        else if(uart6_flag == 0x03)
        {
            uart6_flag = 0x00;
            UART6_RxData[3] = uart6_data;		// 数据位
            voice_falg = 0x01;					// 自定义数据帧接收完毕
        }
        else
        {
            uart6_flag = 0x00;
            voice_falg = 0x00;
            UART6_RxData[0] = 0x00;
        }
        

    }
    //清除串口中断接收标志位
    USART_ClearITPendingBit(USART6,USART_IT_RXNE);
}

/*******************************************************
功　能：通过串口1发送一个字节，如0x12、0xff等
参　数：hex -> 字节
返回值：无
********************************************************/
void USART6_Send_Byte(uint8_t byte)
{
    USART_SendData(USART6,byte);
    while(USART_GetFlagStatus(USART6,USART_FLAG_TXE) == RESET);
}

/*******************************************************
功　能：通过串口1发送一个数组
参　数：*buf -> 指针指向一个数组
		 length -> 数组的长度
返回值：无
********************************************************/
void USART6_Send_Length(uint8_t *buf,uint8_t length)
{
    uint8_t len = 0;
    for(len = 0; len < length; len++)
    {
        USART6_Send_Byte(buf[len]);
    }
}

/**************************************************
功  能：语音识别函数
参  数：	0控制语音播报随即播报指令语音识别测试，2-6播报指定命令进行语音识别测试
参  数：	n 识别次数
返回值：	语音词条ID    词条内容

		0x01      美好生活

		0x02      秀丽山河

		0x03      追逐梦想

		0x04      扬帆启航

		0x05      齐头并进
        
		0x00      未识别到词条/识别超时
**************************************************/
uint8_t BKRC_Voice_Extern(uint8_t yy_mode,uint8_t n)		// 语音识别
{
    uint32_t timers = 0;               // 计数值2
	
    for (int i = 0; i < n; i++)
    {
        SYN7318_Flag=0;
        UART6_RxFlag=0;
        USART6_Send_Length(start_voice_dis,5);//发送开启语音识别指令
        delay_ms(500);
        if(UART6_RxFlag)
        {
            if(UART6_RxData[0]==0x55 && UART6_RxData[1]==0x02)
            {
                return UART6_RxData[2];
            }
            UART6_RxFlag=0;    
        }
        
        delay_ms(500);
        delay_ms(500);
        delay_ms(500);
        //SYN7318_Flag = Voice_Drive();//接收返回状态
        if(yy_mode==0)
        {
            OFlag_YY_cmd(0);			//语音播报随机语音命令
            OFlag_YY_cmd(0);			//语音播报随机语音命令
        }else
        {
            OFlag_YY_cmd(yy_mode);			//语音播报指定语音
            OFlag_YY_cmd(yy_mode);			//语音播报指定语音
        }
        while (1)
        {

            if(UART6_RxFlag)
            {
                if(UART6_RxData[0]==0x55 && UART6_RxData[1]==0x02)
                {
                    return UART6_RxData[2];
                }
                UART6_RxFlag=0;    
            }

            delay_ms(1);
            timers++;
            if (timers>6000)//判断超时退出
            {
                timers=0;
                break;
            }

            /*
            if(timers%1000==0)//每一秒发送一次识别
            {
                USART6_Send_Length(start_voice_dis,5);//发送开启语音识别指令
                delay_ms(500);
            }*/
        }
    }
    return 0;
}


/**************************************************
功  能：语音识别回传命令解析函数
参  数：	无
返回值：	语音词条ID /小创语音识别模块状态
**************************************************/
uint8_t Voice_Drive(void)
{
    uint8_t status = 0;
    if ((voice_falg == 0x01) && (UART6_RxData[0] == 0x55))			// 自定义数据帧接收完毕
    {
        if (UART6_RxData[1] == 0x02)
        {
            status &= 0xF0;
            switch (UART6_RxData[2])
            {
            case 0x01: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* 美好生活 *");
                status |= 0x01;
                break;
            }
            case 0x02: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* 秀丽山河 *");
                status |= 0x02;
                break;
            }
            case 0x03: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* 追逐梦想 *");
                status |= 0x03;
                break;
            }
            case 0x04: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* 扬帆启航 *");
                status |= 0x04;
                break;
            }
            case 0x05: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* 齐头并进 *");
                status |= 0x05;
                break;
            }
            default  :
                status=UART6_RxData[2];//返回识别结果ID
                return status;
                
            }
        }
        voice_falg = 0x00;
    }
    return status;
}

/**************************************************
功  能：控制语音播报标志物播报指定文本信息
参  数：	*p  --> 需要发送的数据
返回值：	无

void YY_Play(char *p)
{
    uint16_t p_len = strlen(p);             // 文本长度

    YY_Init[1] = 0xff & ((p_len + 2) >> 8); // 数据区长度高八位
    YY_Init[2] = 0xff & (p_len + 2);        // 数据区长度低八位
    Send_ZigbeeData_To_Fifo(YY_Init, 5);
    Send_ZigbeeData_To_Fifo((uint8_t *)p, p_len);
    delay_ms(100);
}
**************************************************/
/**********************************************************************
 * 函 数 名 ：  控制语音播报标志物播报语音控制命令
 * 参    数 ：  Primary   -> 主指令
                Secondary -> 副职令
                详见附录1
 * 返 回 值 ：  无
 * 简    例 ：  YY_Comm_Zigbee(0x20, 0x01);     // 语音播报随机语音命令

附录1：
-----------------------------------------------------------------------
| Primary | Secondary | 说明
|---------|-----------|------------------------------------------------
|  0x10   |  0x02     | 美好生活
|         |  0x03     | 秀丽山河
|         |  0x04     | 追逐梦想
|         |  0x05     | 扬帆启航
|         |  0x06     | 齐头并进
|---------|-----------|------------------------------------------------
|  0x20   |  0x01     | 随机指令
|---------|-----------|------------------------------------------------
***********************************************************************/

/*******************************************************
功　能：语音识别初始化函数
参　数：无
返回值：无
********************************************************/
void BKRC_Voice_Init(void)
{
    USART6_Hardware_Init();
}

//										endfile

/**
 * @description: 播报0-9
 * @param {int} number
 * @return {*}
 */
void XiaoChuang_PlayNUMbit(int number)
{
/*
A0::零:5503A000
A1::一:5503A100
A2::二:5503A200
A3::三:5503A300
A4::四:5503A400
A5::五:5503A500
A6::六:5503A600
A7::七:5503A700
A8::八:5503A800
A9::九:5503A900
*/
	USART6_Send_Byte(0xA0+(number));
	/*if(number<0)
	{
		USART6_Send_Byte(0xA0+(number*-1));
	}else
	{
		
	}*/
}
/**********************************************************************
 * 函 数 名 ：  小创语音播报数字大小
 * 参    数 ：  语音数值 0 到 999
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ： 例:105 一百零五
*****************************************************************/
void XiaoChuang_PlaySIZE(int number)
{	int t = 800; //语音合成间隔/us 没有间隔会造成阻塞
	int buf = number;
	int len = 1;
	int bufARR[20];//将数据存放至数组
	if(number>9 || number<-9)
	{
		while (buf/=10)//计算数字位数
		{
			if(len==1)//读出各位
			{
				bufARR[len]=number%10;
			}else //读出中间位
			{
				bufARR[len]=pow(10,(len-1));
				bufARR[len]=number/bufARR[len]%10;
			}
			
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
			len++;
		}
		if(len>1)
		{
			//读出末位
			bufARR[len]=pow(10,(len-1));
			bufARR[len]=number/bufARR[len];
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
		}
	}else
	{
		bufARR[len]=number;
	}
	//printf("len=%d\n",len);
	
	//1 2345 6 7 8 9
	switch(len)
	{
	case 1: 
		XiaoChuang_PlayNUMbit(bufARR[1]);
		break;
	case 2:
		if(bufARR[2]>1)//防止播报一十
		{
			XiaoChuang_PlayNUMbit(bufARR[2]);//n
			delay_us(t);
		}
		USART6_Send_Byte(0xAA);//十
		delay_us(t);
		if(bufARR[1]!=0)//个位为0不播报
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
			delay_us(t);
		}
		break;
	case 3://100   320  409    111
		XiaoChuang_PlayNUMbit(bufARR[3]);//n（百位上的数值）
		delay_us(t);
		USART6_Send_Byte(0xAB);//百
		delay_us(t);
		if(bufARR[2]!=0)//110 111 (十位不为零)
		{
			XiaoChuang_PlayNUMbit(bufARR[2]);
			delay_us(t);
			USART6_Send_Byte(0xAA);//十
			delay_us(t);
			if(bufARR[1]!=0)//防止播报十零
			{
				XiaoChuang_PlayNUMbit(bufARR[1]);
				delay_us(t);
			}
		}else//101 (十位为零)
		{
			if(bufARR[2]==0 && bufARR[1]!=0)//x0x 若x00则不播报十位与各位
			{
				XiaoChuang_PlayNUMbit(bufARR[2]);
				delay_us(t);
				XiaoChuang_PlayNUMbit(bufARR[1]);
				delay_us(t);
			}
		}
		break;
	/*case 4:
		XiaoChuang_PlayNUMbit(bufARR[4]);
		USART6_Send_Byte(0xAC);
		if(bufARR[3]!=0)//千
		{
			XiaoChuang_PlayNUMbit(bufARR[3]);
			USART6_Send_Byte(0xAB);
		}//n0nn
		
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//十
		if(bufARR[1]!=0)//防止播报十零
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;
	case 5:
		XiaoChuang_PlayNUMbit(bufARR[5]);//万
		USART6_Send_Byte(0xAD);
		XiaoChuang_PlayNUMbit(bufARR[4]);
		USART6_Send_Byte(0xAC);
		XiaoChuang_PlayNUMbit(bufARR[3]);
		USART6_Send_Byte(0xAB);
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//十
		if(bufARR[1]!=0)//防止播报十零
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;
	case 6:
		if(bufARR[6]>1)
		{
			XiaoChuang_PlayNUMbit(bufARR[6]);
		}
		USART6_Send_Byte(0xAA);//十
		XiaoChuang_PlayNUMbit(bufARR[5]);//万
		USART6_Send_Byte(0xAD);
		XiaoChuang_PlayNUMbit(bufARR[4]);
		USART6_Send_Byte(0xAC);
		XiaoChuang_PlayNUMbit(bufARR[3]);
		USART6_Send_Byte(0xAB);
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//十
		if(bufARR[1]!=0)//防止播报十零
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;*/
	}

	
}

void XiaoChuang_PlayNUMBER(int number)
{	int t = 800; //语音合成间隔/us 没有间隔会造成阻塞
	int buf = number;
	int len = 1;
	int bufARR[20];//将数据存放至数组
	if(number>9 || number<-9)
	{
		while (buf/=10)//计算数字位数
		{
			if(len==1)//读出各位
			{
				bufARR[len]=number%10;
			}else //读出中间位
			{
				bufARR[len]=pow(10,(len-1));
				bufARR[len]=number/bufARR[len]%10;
			}
			
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
			len++;
		}
		if(len>1)
		{
			//读出末位
			bufARR[len]=pow(10,(len-1));
			bufARR[len]=number/bufARR[len];
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
		}
	}else
	{
		bufARR[len]=number;
	}
    for (int i = len; i >0; i--)
    {
        XiaoChuang_PlayNUMbit(bufARR[i]);
        delay_us(t);
    }
    
}

