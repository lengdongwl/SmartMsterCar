/**************************************
任务板相关函数
***************************************/

#include "NEW_Task_board.h"
#include "ultrasonic.h"

//#include "bh1750.h"     //光照传感器端口初始化文件
//#include "infrared.h"   //红外端口初始化文件
//#include "ultrasonic.h" //超声波端口初始化文件
//#include "new_syn.h"    //语音端口初始化文件
/********************************************************
任务板功能检测项目
	1.数码管 
	2.转向灯
	3.红外
	4.蜂鸣器
	5.光照传感器
	6.超声波
	7.语音(扬声器、麦克风)
	
 *8.光敏电阻（标准任务板）
 ***************************************/

uint8_t SMG_LR=0;
uint8_t SEG_Table[17]={0xC0,0xF9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xff}; 
//共阳                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   关闭   
//uint8_t SEG_Table[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00}; 
//共阴                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   关闭   

//	0x89 H   0xc7  L
/*
			 a							
			——						
	f	|    | b
			——
	e	|  g | c
			——   。dp
			d

			dp g  f  e   d  c  b  a
(0)		1  1  0  0   0  0  0  0    0xc0 共阳


(0)		0  0  1  1   1  1  1  1    0x3f 共阴

　　0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e
　　共阴：
　　0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71

*/
uint8_t Digital_tube[2];//数码管显示数据


/**********************************************************************
 * 函 数 名 ：  任务板初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Task_board_Init(void)
{
	#if versions==0
	Tba_Photoresistance_Config();		// 任务板光敏电阻配置
	Tba_Beep_Config();					    // 任务板蜂鸣器配置
	Tba_WheelLED_Config();				  // 任务板转向灯配置
	#endif
	#if versions==1
	Tba_WheelLED_Config();				  // 任务板转向灯配置
	Nixie_tube_595_Init();          //数码管初始化
	#endif
	#if versions==2
	Nixie_tube_595_Init();          //数码管初始化
	CD4051_init();                  //CD4051初始化
	#endif
	#if versions==3
	Nixie_tube_595_Init();          //数码管初始化
	CD4051_init();                  //CD4051初始化
	#endif
	#if versions==4
	Tba_WheelLED_Config();				  // 任务板转向灯配置
	Nixie_tube_595_Init();          //数码管初始化
	#endif
	#if versions==5
	Nixie_tube_595_Init();          //数码管初始化
	CD4051_init();                  //CD4051初始化
	#endif
}

/**********************************************************************
 * 函 数 名 ：  数码管初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Nixie_tube_595_Init(void)								
{
#if versions==1
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	
		/*PH11-SCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
		/*PH10-RCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
		/*PB15-SER*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
		/*PF11-SMG*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;	  	//输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOF,&GPIO_TypeDefStructure);
#endif	
#if versions==2
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
		/*PF11-SCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOF,&GPIO_TypeDefStructure);
		/*PG8-RCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOG,&GPIO_TypeDefStructure);
		/*PC13-SER*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOC,&GPIO_TypeDefStructure);		
#endif
#if versions==3
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		/*PC13-SCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
		/*PG8-RCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOF,&GPIO_TypeDefStructure);
		/*PC13-SER*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);		
#endif	
#if versions==4
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	
		/*PH11-SCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
		/*PH10-RCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
		/*PB15-SER*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
		/*PC13-SMG*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;	  	//输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
#endif
#if versions==5
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	
		/*PC13-SCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
		/*PF11-RCK*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOF,&GPIO_TypeDefStructure);
		/*PB9-SER*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
#endif
}

/**********************************************************************
 * 函 数 名 ：  CD4051初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void CD4051_init(void)					
{	
#if versions==0
	//无
#endif		
#if versions==1
	//无
#endif		
#if versions==2
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
		/*PB4-CD4051_C- -PB15-CD4051_COM-*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_4;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
		/*PH10-CD4051_A- -PH11-CD4051_B*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
#endif
#if versions==3
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
		/*PB4-CD4051_C- -PB15-CD4051_COM-*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_4;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
		/*PH10-CD4051_A- -PH11-CD4051_B*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
#endif
#if versions==4
	//无
#endif	
#if versions==5
		GPIO_InitTypeDef  GPIO_TypeDefStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
		/*PB4-CD4051_C- -PB15-CD4051_COM-*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_4;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
		/*PH10-CD4051_A- -PH11-CD4051_B*/
		GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		  //输出
		GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//上拉
		GPIO_Init(GPIOH,&GPIO_TypeDefStructure);	
#endif
}

/**********************************************************************
 * 函 数 名 ：  CD4051选择通道
 * 参    数 ：  gear：通道
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Choose_CD4051_Gear(uint8_t gear)
{
#if versions==0
//无CD4051
#endif		
#if versions==1
//无CD4051
#endif	
#if versions==2
	if(gear==0)										//SMG_A 
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==1)							//SMG_B
	{
		CD4051_A = 1; 
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==2)							//INC
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==3)							//RI_TXD
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==4)							//LED_L  默认初始化左闪
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==5)							//LED_R
	{
		CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==6)							//BEEP
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 1;
	}
	else if(gear==7)							//out
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 1;
	}
#endif			
#if versions==3
	if(gear==0)										//SMG_A 
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==1)							//SMG_B
	{
		CD4051_A = 1; 
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==2)							//INC
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==3)							//RI_TXD
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==4)							//LED_L  默认初始化左闪
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==5)							//LED_R
	{
		CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==6)							//BEEP
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 1;
	}
	else if(gear==7)							//out
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 1;
	}
#endif			
#if versions==4
//无CD4051
#endif	
#if versions==5
	if(gear==0)										//SMG_A 
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==1)							//SMG_B
	{
		CD4051_A = 1; 
		CD4051_B = 0;
		CD4051_C = 0;
	}
	else if(gear==2)							//INC
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==3)							//LED_L
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 0;
	}
	else if(gear==4)							//LED_R 
	{
		CD4051_A = 0;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==5)							//BEEP
	{
		CD4051_A = 1;
		CD4051_B = 0;
		CD4051_C = 1;
	}
	else if(gear==6)							//RI_TXD
	{
		CD4051_A = 0;
		CD4051_B = 1;
		CD4051_C = 1;
	}
	else if(gear==7)							//out
	{
		CD4051_A = 1;
		CD4051_B = 1;
		CD4051_C = 1;
	}
#endif	
}

/**********************************************************************
 * 函 数 名 ：  任务板蜂鸣器初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Tba_Beep_Config(void)
{
#if versions==0	
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		// 输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		// 上拉
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
#endif			
#if versions==1	
	//与LED-L-R 共用端口
#endif		
#if versions==2	
	//CD4051控制
#endif	
#if versions==3	
	//CD4051控制
#endif		
#if versions==4	
	//与LED共用端口
#endif	
#if versions==5	
	//CD4051控制
#endif
}

/**********************************************************************
 * 函 数 名 ：  任务板蜂鸣器控制
 * 参    数 ：  state：状态 （RET开 RESET关）
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Task_board_buzzer(uint8_t state)
{
#if versions==0	
	
	if(state == SET)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	} 
	else if(state == RESET)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
	}	
#endif				
#if versions==1
	if(state == SET)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	} 
	else if(state == RESET)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}	
#endif	
#if versions==2	
	if((state == SET) || (state == 1))
	{
		Choose_CD4051_Gear(6);
		CD4051_COM = 0;
	} 
	else if((state == RESET) || (state == 0))
	{
		Choose_CD4051_Gear(6);
		CD4051_COM = 1;
	}	
#endif	
#if versions==3	
	if((state == SET) || (state == 1))
	{
		Choose_CD4051_Gear(6);
		CD4051_COM = 0;
	} 
	else if((state == RESET) || (state == 0))
	{
		Choose_CD4051_Gear(6);
		CD4051_COM = 1;
	}	
#endif	
#if versions==4	
	if(state == SET)
	{
		GPIO_ResetBits(GPIOF,GPIO_Pin_11);
	} 
	else if(state == RESET)
	{
		GPIO_SetBits(GPIOF,GPIO_Pin_11);
	}	
#endif
#if versions==5	
	if((state == SET) || (state == 1))
	{
		Choose_CD4051_Gear(5);
		CD4051_COM = 0;
	} 
	else if((state == RESET) || (state == 0))
	{
		Choose_CD4051_Gear(5);
		CD4051_COM = 1;
	}	
#endif
	Task_board_turn_light(L_LED,RESET);  	    //左闪 关    
}

/**********************************************************************
 * 函 数 名 ：  任务板转向灯初始化
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Tba_WheelLED_Config(void)
{
#if versions==0	
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);	
	// LED_L -左闪- PH10  LED_R -右闪-PH11
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		// 输入
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		// 上拉
	GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
	GPIO_SetBits(GPIOH,GPIO_Pin_10|GPIO_Pin_11);
#endif
#if versions==1		//SET左闪 RESET右闪
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	// LED_L-LED_R --PB9
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		// 输入
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		// 上拉
	GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
#endif
#if versions==2	
//使用CD4051控制	
#endif	
#if versions==3	
//使用CD4051控制	
#endif	
#if versions==4	
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	// LED_L-LED_R --PF11 蜂鸣器
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		// 输入
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		// 上拉
	GPIO_Init(GPIOF,&GPIO_TypeDefStructure);
#endif
#if versions==5	
//使用CD4051控制	
#endif
}

/**********************************************************************
 * 函 数 名 ：  任务板转向灯和设置
 * 参    数 ：  LorR  L_LED--左侧   R_LED--右侧 swch  SET -- 开     RESET--关
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Task_board_turn_light(uint8_t LorR,uint8_t swch)
{
#if versions==0			
	switch(LorR)
	{
		case L_LED:
		{
			if(swch)
			{
				GPIO_ResetBits(GPIOH,GPIO_Pin_10);
			}
			else	
			{
				GPIO_SetBits(GPIOH,GPIO_Pin_10);
			}
			break;
		}
		case R_LED:
		{
			if(swch)
			{
				GPIO_ResetBits(GPIOH,GPIO_Pin_11);
			}				
			else
			{
				GPIO_SetBits(GPIOH,GPIO_Pin_11);
			}
			break;
		}
	}
#endif
#if versions==1			
	switch(LorR)
	{
		case L_LED:
		{
			if(swch)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_9);			
			}
			else	
			{			
				GPIO_ResetBits(GPIOB,GPIO_Pin_9);
			}
			break;
		}
		case R_LED:
		{
			if(swch)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_9);
			}				
			else
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_9);
			}
			break;
		}
	}
#endif
#if versions==2		//LED LR 蜂鸣器 共用一个端口
	switch(LorR)
	{
		case L_LED:
			if((swch == SET) || (swch == 1))	  //左闪		初始化也是左闪
			{
				Choose_CD4051_Gear(4);				
				CD4051_COM = 1;
			}				
			else
			{
				Choose_CD4051_Gear(4);						//全灭				
				CD4051_COM = 0;
			}		
			break;
		case R_LED:
			if((swch == SET) || (swch == 1))		//双闪
			{
				Choose_CD4051_Gear(5);
				CD4051_COM = 0;
			}
			else	
			{
				Choose_CD4051_Gear(5);				    //左闪		初始化也是左闪			
				CD4051_COM = 1;
			}
			break;
	}
#endif				
#if versions==3		
	switch(LorR)
	{
		case L_LED:
			if((swch == SET) || (swch == 1))	  //左闪		初始化也是左闪
			{
				Choose_CD4051_Gear(4);				
				CD4051_COM = 1;
			}				
			else
			{
				Choose_CD4051_Gear(4);						//全灭				
				CD4051_COM = 0;
			}		
			break;
		case R_LED:
			if((swch == SET) || (swch == 1))		//双闪
			{
				Choose_CD4051_Gear(5);				
				CD4051_COM = 0;
			}
			else	
			{
				Choose_CD4051_Gear(5);				    //左闪		初始化也是左闪
				
				CD4051_COM = 1;
			}
			break;
	}
#endif					
#if versions==4			//LED LR 蜂鸣器 共用一个端口
	switch(LorR)
	{
		case L_LED:
		{
			if(swch)
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_11);			
			}
			else	
			{			
				GPIO_ResetBits(GPIOF,GPIO_Pin_11);
			}
			break;
		}
		case R_LED:
		{
			if(swch)
			{
				GPIO_ResetBits(GPIOF,GPIO_Pin_11);
			}				
			else
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_11);
			}
			break;
		}
	}
#endif
#if versions==5		//LED LR 蜂鸣器 共用一个端口
	switch(LorR)
	{
		case L_LED:
			if((swch == SET) || (swch == 1))	  //左闪		初始化也是左闪
			{
				Choose_CD4051_Gear(3);				
				CD4051_COM = 1;
			}				
			else
			{
				Choose_CD4051_Gear(3);						//全灭				
				CD4051_COM = 0;
			}		
			break;
		case R_LED:
			if((swch == SET) || (swch == 1))		//双闪
			{
				Choose_CD4051_Gear(4);				
				CD4051_COM = 0;
			}
			else	
			{
				Choose_CD4051_Gear(4);				    //左闪		初始化也是左闪				
				CD4051_COM = 1;
			}
			break;
	}
#endif
}

/**********************************************************************
 * 函 数 名 ：  数码管显示
 * 参    数 ：  Data:显示的数字 bit：显示的位
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void SMG_display(uint8_t Data,uint8_t bit) 
{
#if versions!=0
/**********************************段选**********************************/










/*
		uint8_t Data_Bit; 
		int i=0;		
		for(i=0; i<8; i++)								     //当i=1时数码管显示小数点     
		{
				Data_Bit  = SEG_Table[Data];
				if (((Data_Bit)<<i)&0x80)					//判断输入的数据			是 1 或者 0
				{
						SER=1;							
				}
				else
				{
						SER=0;
				}
				SCK = 0;									//		移位     当确定输入的数据之后进行移位操作，把数据存进寄存器里面
				SCK = 1;
		}					
		RCK = 0;											//寄存器里面的数据传给数码管
		RCK = 1;
					*/








				
/**********************************位选**********************************/
					
#if versions==0
//没有数码管				
#endif									
#if versions==1			
					if(bit==0)											//切换数码管
					{
							SMG=0;
					}
					else
					{
							SMG=1;
					}
#endif							
#if versions==2			
					if(bit==0)											//切换数码管
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(0);
					}
					else
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(1);
					}
#endif
#if versions==3			
					if(bit==0)											//切换数码管
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(0);
					}
					else
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(1);
					}
#endif
#if versions==4			
					if(bit==0)											//切换数码管
					{
							SMG=0;
					}
					else
					{
							SMG=1;
					}
#endif		
#if versions==5			
					if(bit==0)											//切换数码管
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(0);
					}
					else
					{
							CD4051_COM = 1;
							Choose_CD4051_Gear(1);
					}
#endif
#endif						
}

/**********************************************************************
 * 函 数 名 ：  数码管动态显示
 * 参    数 ：  dta:显示的数字 state：SET开启 RES关闭
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Digital_Display(uint8_t One,uint8_t Two)
{
#if versions!=0
	
			Digital_tube[0]=One;
			Digital_tube[1]=Two;

			if(One>=16 && Two>=16)
			{
					SMG_display(16,0);
					SMG_display(16,1);
					#if versions==2	//CD4051	
						CD4051_COM = 0;				
					#endif		
					#if versions==3	//CD4051	
						CD4051_COM = 0;				
					#endif				
					#if versions==5 //CD4051	
						CD4051_COM = 0;				
					#endif	
					TIM_Cmd(TIM4,DISABLE);	//失能TIM4
			}
			else
			{
					TIM_Cmd(TIM4,ENABLE);   //使能TIM4
			}
#endif						
	
}

/**********************************************************************
 * 函 数 名 ：  定时器初始化
 * 参    数 ：  
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStart;
	NVIC_InitTypeDef NVIC_InitStart;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseStart.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStart.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStart.TIM_Period=arr;
	TIM_TimeBaseStart.TIM_Prescaler=psc;
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStart);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	NVIC_InitStart.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStart.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStart.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStart.NVIC_IRQChannelSubPriority=0x04;
	NVIC_Init(&NVIC_InitStart);
	
	Digital_Display(16,16);//数码管清屏
	
}

/**********************************************************************
 * 函 数 名 ：  TIM4中断服务程序
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Scan_Falg=0;// 0仅数码管 1仅转向灯  3全都有
/*注意！有CD4051的板之只能*/
/*
#if version>=0|| version>=5
void TIM4_IRQHandler()
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
			if(SMG_LR==0)
			{	
				SMG_LR=1;	
				if(Scan_Falg==0||Scan_Falg==3)		//数码管动态扫描
				{
					SMG_display(Digital_tube[1],1);			
				}			
				if(Scan_Falg==1||Scan_Falg==3)		//双闪控制
				{
					Task_board_turn_light(L_LED,1);
				}
			}
			else
			{
				SMG_LR=0;			
				if(Scan_Falg==0||Scan_Falg==3)		//数码管动态扫描
				{
					SMG_display(Digital_tube[0],0);			
				}	
				if(Scan_Falg==1||Scan_Falg==3)		//双闪控制
				{
					Task_board_turn_light(R_LED,1);
				}					
			}
		
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}
#endif
*/
/**********************************************************************
 * 函 数 名 ：  红外发射子程序
 * 参    数 ：  *s：指向要发送的数据  n：数据长度
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Digital_tube_Infrared_Send(uint8_t *s,int n)
{
	#if 0 //红外端口初始化在这个文件里
		#include "infrared.h"
	#endif
	
	uint8_t i,j,temp;
#if versions==0
	//端口控制
#endif
#if versions==1
	//端口控制
#endif
#if versions==2
	Choose_CD4051_Gear(3);
#endif
#if versions==3
	Choose_CD4051_Gear(3);
#endif	
#if versions==4
	//端口控制
#endif
#if versions==5
	Choose_CD4051_Gear(6);
#endif	




/*
	Infrared_out = 0;
	delay_ms(9);
	Infrared_out = 1;
	delay_ms(4);
	delay_us(560);

	for(i=0; i<n; i++)
	{
		for(j=0;j<8;j++)
		{
			temp = (s[i]>>j)&0x01;
			if(temp==0)				  //发射0
			{
				Infrared_out = 0;
				delay_us(500);		//延时0.5ms
				Infrared_out = 1;
				delay_us(500);		//延时0.5ms
			}
			if(temp==1)				  //发射1
			{
				Infrared_out = 0;
				delay_us(500);		//延时0.5ms
				Infrared_out = 1;
				delay_ms(1);
				delay_us(800);		//延时1.69ms
			}
		}
	}
		Infrared_out = 0;     //结束
		delay_us(560);				//延时0.56ms
		Infrared_out = 1;     //关闭红外发射
*/





}

/**********************************************************************
 * 函 数 名 ：  任务板光敏电阻配置
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
void Tba_Photoresistance_Config(void)
{
#if versions==0		
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_IN;		// 输入模式
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   	// 上拉模式
	GPIO_Init(GPIOA,&GPIO_TypeDefStructure);
#endif
#if versions==1		
//无光敏电阻	
#endif
#if versions==2		
//无光敏电阻	
#endif
#if versions==3		
//无光敏电阻	
#endif
#if versions==4	
//无光敏电阻	
#endif
#if versions==5	
//无光敏电阻	
#endif
}

/**********************************************************************
 * 函 数 名 ：  获取光敏电阻状态
 * 参    数 ：  无
 * 返 回 值 ：  无
 * 全局变量 ：  无
 * 备    注 ：  无
*****************************************************************/
uint8_t Get_tba_phsis_value(void)
{
	return GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
}
