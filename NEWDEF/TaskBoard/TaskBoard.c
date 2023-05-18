/*
 * @Description: 任务板器件操作
 * @Autor: 309 Mushroom
 * @Date: 2021-10-27 15:15:12
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-10-11 19:54:14
 */
#include "TaskBoard.h"
#include "ultrasonic.h" //超声波
#include "infrared.h"   //红外线
#include "bh1750.h"     //光照度
#include "syn7318.h"    //syn7318语音模块
#include "canp_hostcom.h"
#include "OperationFlag.h"

#define gpioA 0
#define gpioB 1
#define gpioC 2
#define gpioD 3
#define gpioE 4
#define gpioF 5
#define gpioG 6
#define gpioH 7
uint8_t SMG_show = 0; //数码管刷新显示
uint8_t SMG_data1 = 99, SMG_data2 = 99;
int TaskBoard_TimeCount=0;//倒计时统计 
int TaskBoard_TimeIT=0;//打断倒计时
int RCC_check(int gpioX)//检测IO口是否使能
{
    return( (RCC->AHB1ENR>>gpioX)%2);
}


int tim2_count;//统计tim2中断次数

/**
 * @description: 开启倒计时
 * @param {int} time 倒计时数值 单位：ms
 * @return {*}
 */
void TaskBoard_Time(int time)
{
    tim2_count=0;
    TaskBoard_TimeIT=0;//重置打断状态
    TaskBoard_TimeCount=time;//装载倒计时值
    TIM_Cmd(TIM2, ENABLE);
}

void TaskBoard_TimeITOpen(void){TaskBoard_TimeIT=1;}//中断倒计时
void TaskBoard_TimeClose(void){TIM_Cmd(TIM2, DISABLE);}//关闭倒计时
/**
 * @description: 获取倒计时状态
 * @param {*}
 * @return {*}0.倒计时完成 1.倒计时状态 2.打断状态  
 */

uint8_t TaskBoard_TimeStatus(void)
{
    if (TaskBoard_TimeIT)
    {
        return 2;
    }
    return TaskBoard_TimeCount!=0?1:0;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        
        if (SMG_show) //首位数码管亮度取决于定时器2的arr 中断过快影响核心板响应速度
        {
            TaskBoard_SMGDisplay(0, SMG_data1);
            delay_ms(1);
            TaskBoard_SMGDisplay(1, SMG_data2);
        }
        else
        {
            TaskBoard_SMGDisplay(0, 99);
            TaskBoard_SMGDisplay(1, 99);
        }
        if(TaskBoard_TimeCount!=0)
        {
            if(tim2_count>=500)
            {
                tim2_count=0;
                TaskBoard_TimeCount--;
                //Send_Debug_num2(TaskBoard_TimeCount);
            } 
        }  
        tim2_count++;  
    }

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

//LED灯初始化
void _LED_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 5;             //0.A 1.B 2.C 3.D 4.E 5.F 使能GPIO时钟
    GPIO_Set(GPIOF, PIN11, 1, 0, 1, 1); //初始化GPIOF11
    TaskBoard_LED = 1;
#endif
//Z1　使用cd4051  3.LED_L 4.LED_R
#if IOversion == 2
    RCC->AHB1ENR |= 1 << 7;
    GPIO_Set(GPIOH, PIN10, 1, 0, 1, 1); //初始化GPIOH10  LEDL
    GPIO_Set(GPIOH, PIN11, 1, 0, 1, 1); //初始化GPIOH10　LEDR
    TaskBoard_LED_L = 1;//关闭
    TaskBoard_LED_R = 1;
#endif
}

//蜂鸣器初始化
void _BEEP_Init(void)
{
#if IOversion == 0
    //与LED一致
#endif
//Z1　使用cd4051   5.BEEP 

#if IOversion == 2
    RCC->AHB1ENR |= 1 << gpioC;
    GPIO_Set(GPIOC, PIN13, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //初始化GPIOC13 BEEP
    TaskBoard_BEEP=1;
#endif
}

//超声波初始化
void _WAVE_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 1; //GPIOB
    RCC->AHB1ENR |= 1 << 0; //GPIOA
    GPIO_Set(GPIOB, PIN4, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);//INC
    GPIO_Set(GPIOA, PIN15, GPIO_Mode_IN, 0, 3, GPIO_PuPd_UP);//INT0
#endif

#if IOversion == 1
    RCC->AHB1ENR |= 1 << 0; //GPIOA
    GPIO_Set(GPIOA, PIN15, GPIO_Mode_IN, 0, 3, GPIO_PuPd_UP);//INT0
    //Z1　使用cd4051 2.INC
#endif

#if IOversion == 2
    RCC->AHB1ENR |= 1 << gpioB; //GPIOB
    RCC->AHB1ENR |= 1 << gpioA; //GPIOA
    GPIO_Set(GPIOB, PIN4, GPIO_Mode_IN, 0, 3, GPIO_PuPd_UP);//INT0
    GPIO_Set(GPIOA, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);//INC
#endif

}

//红外线线发射初始化
void _IR_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 6; //GPIOG
    GPIO_Set(GPIOG, PIN8, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
#endif
//Z1　使用cd4051 6.RI_TXD
#if IOversion == 2
    RCC->AHB1ENR |= 1 << 5; //GPIOF
    GPIO_Set(GPIOF, PIN11, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    
#endif
}

//光照度检测初始化
void _LIGHT_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 6; //GPIOG ADDR
    RCC->AHB1ENR |= 1 << 1; //GPIOB SCL
    GPIO_Set(GPIOG, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    GPIO_Set(GPIOB, PIN7, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    BH1750_Configure();
#endif
#if IOversion == 1
    RCC->AHB1ENR |= 1 << 6; //GPIOG ADDR
    RCC->AHB1ENR |= 1 << 1; //GPIOB SCL
    GPIO_Set(GPIOG, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    GPIO_Set(GPIOB, PIN7, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    BH1750_Configure();
#endif
#if IOversion == 2
    RCC->AHB1ENR |= 1 << 6; //GPIOG ADDR
    RCC->AHB1ENR |= 1 << 1; //GPIOB SCL
    GPIO_Set(GPIOG, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    GPIO_Set(GPIOB, PIN6, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    BH1750_Configure();
#endif
}

//数码管初始化
void _SMG_Init()
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 1;                                    //GPIOB
    RCC->AHB1ENR |= 1 << 2;                                    //GPIOC
    RCC->AHB1ENR |= 1 << 7;                                    //GPIOH
    GPIO_Set(GPIOB, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //SER
    GPIO_Set(GPIOH, PIN10, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //RCK
    GPIO_Set(GPIOH, PIN11, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //SCK
    GPIO_Set(GPIOC, PIN13, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //SMG
#endif

#if IOversion == 1
    RCC->AHB1ENR |= 1 << 1;                                    //GPIOB
    RCC->AHB1ENR |= 1 << 2;                                    //GPIOC
    RCC->AHB1ENR |= 1 << 5;                                    //GPIOF
    GPIO_Set(GPIOB, PIN9, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);  //SER
    GPIO_Set(GPIOF, PIN11, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //RCK
    GPIO_Set(GPIOC, PIN13, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //SCK
#endif
}

//CD4051初始化
void _CD4051_Init(void)
{
#if IOversion == 0
    //G0无
#endif

#if IOversion == 1
    //Z1
    RCC->AHB1ENR |= 1 << 1;                                    //GPIOB
    RCC->AHB1ENR |= 1 << 7;                                    //GPIOH
    GPIO_Set(GPIOH, PIN10, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //A
    GPIO_Set(GPIOH, PIN11, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //B
    GPIO_Set(GPIOB, PIN4, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);  //C
    GPIO_Set(GPIOB, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //COM
#endif
}

//*****************************任务版各类初始化*****************************
/**【任务版更改IO口】
 * 
 * 一.LED灯与蜂鸣器 
 * {TaskBoard.c}初始化函数
 * {TaskBoard.h}位带操作定义
 * 
 * 二.超声波测距
 * {TaskBoard.c}初始化函数与操作函数 发射线与接受线
 * {TaskBoard.h}位带操作定义 输出与输入
 * {ultrasonic.c}1.函数 Ultrasonic_EXTI()外部中断初始化 2.函数 Ultrasonic_Ranging()测距 3.EXTI15_10_IRQHandler()外部中断函数
 * 
 * 三.红外线发射
 * {TaskBoard.c}初始化函数与操作函数 发射线
 * {TaskBoard.h}位带操作定义 输出线
 * {infrared.c}1.Infrared_Send()
 * 
 * 四.光照度读取
 * {TaskBoard.c}初始化函数与操作函数
 * {bh1750.h}位带操作定义 
 * {bh1750.c}1.函数SDA_OUT() 2.函数SDA_IN()
 * 
 * 五.数码管
 * {TaskBoard.c}初始化函数与操作函数
 * {TaskBoard.h}位带操作定义 
 * 
 * 六.语音识别
 * 一般固定串口位置 不同模块需用不同库
 * 
************************************************************************/
void TaskBoard_Init(void)
{
    _LED_Init();    //LED
    _BEEP_Init();   //蜂鸣器
    _WAVE_Init();   //超声波
    _IR_Init();     //红外线
    _LIGHT_Init();  //光照度
    _SMG_Init();    //数码管
    _CD4051_Init(); //CD4051初始化
                    //串行通讯初始化 无需更改
}
//*************************************************************************

/**
 * @description: 任务版测试
 * @param {*}
 * @return {*}
 */
void TaskBoard_test(void)
{
#if IOversion == 0
    //蜂鸣器
    TaskBoard_BEEP = 1;
    delay_ms(200);
    TaskBoard_BEEP = 0;
    delay_ms(200);
    TaskBoard_BEEP = 1;

    //LED灯
    TaskBoard_LED_show(0,1);
    delay_ms(500);
    TaskBoard_LED_show(1,1);

    //超声波测距
    Send_Debug_string("WAVE=");
    Send_Debug_num(TaskBoard_WAVE());
    Send_Debug_string("\n");

    //红外发射距离信息
    OFlag_LED_jl(200);

    //光照度获取
    Send_Debug_string("BH=");
    Send_Debug_num(TaskBoard_BH());
    Send_Debug_string("\n");

    //语音识别
    SYN_Extern_wait(20);

    //数码管显示 99
    TaskBoard_SMGDisplay2(9, 9);

    SYN7318_Play("测试完成"); //语音播放
#endif

#if IOversion == 2
      //蜂鸣器
    TaskBoard_BEEP = 1;
    delay_ms(200);
    TaskBoard_BEEP = 0;
    delay_ms(200);
    TaskBoard_BEEP = 1;
    //LED灯
    
    TaskBoard_LED_show(0,0); //左灯开
    TaskBoard_LED_show(1,0); //右灯开

    //超声波测距
    Send_Debug_string("WAVE=");
    Send_Debug_num(TaskBoard_WAVE());
    Send_Debug_string("\n");

    //调光 3挡
    OFlag_light(3);

    //光照度获取
    Send_Debug_string("BH=");
    Send_Debug_num(TaskBoard_BH());
    Send_Debug_string("\n");

    //语音识别
    SYN_Extern_wait(20);

    //数码管显示无

    SYN7318_Play("测试完成"); //语音播放  
#endif
}

/**
 * @description: 读取超声波测距值 单位：mm
 * @param {*}
 * @return {*}测距值
 */
uint32_t TaskBoard_WAVE(void)
{
#if IOversion == 0
    return Ultrasonic_GetBufferN();
#endif
#if IOversion == 1
    return Ultrasonic_GetBufferN();
#endif
#if IOversion == 2
    return Ultrasonic_GetBufferN();
#endif

    return 0;
}

/**
 * @description: 红外线发送
 * @param {uint8_t} *data 数据
 * @param {int} n 长度
 * @return {*}
 */
void TaskBoard_RISend(uint8_t *data, int n)
{
#if IOversion == 0
    Infrared_Send(data, n);
#endif
#if IOversion == 1
    Infrared_Send(data, n);
#endif
#if IOversion == 2
    Infrared_Send(data, n);
#endif
}

/**
 * @description: 读取光照度
 * @param {*}
 * @return {*}光照值
 */
uint16_t TaskBoard_BH(void)
{
    int i, j, b;
    int buf[10];
    for (i = 0; i < 10; i++)
    {
        buf[i] = Get_Bh_Value();
        delay_ms(10);
    }
    for (i = 0; i < 10; i++) //排序
    {
        for (j = 0; j < 10 - i - 1; j++)
        {
            if (buf[j] > buf[j + 1])
            {
                b = buf[j];
                buf[j] = buf[j + 1];
                buf[j + 1] = b;
            }
        }
    }
    //取中间数+偏差
    return (buf[5] + buf[6]) / 2;
}

uint8_t SEG_arr[19] = {0xC0, 0xF9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xff, ~0x80, ~0x73};
//共阳                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   关闭   .  p
//uint8_t SEG_Table[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
//共阴                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   关闭
/**
 * @description: 数码管静态显示 
 * @param {uint8_t} wx 位置
 * @param {uint8_t} number  数据0-15(0-F) 16.全亮 17.（DP 点） 18.（特殊位P） 
 * @return {*}
 */
void TaskBoard_SMGDisplay(uint8_t wx, uint8_t number)
{
#if IOversion !=2  //标志板无数码管
    int i;
    //传输数据 74HC595(SER存入单位数据 SCK将数据QA->QH顺序移位 RCK存储数据)
    uint8_t buf1 = SEG_arr[number], buf2;
    RCK = 0;

    for (i = 0; i < 8; i++)
    {
        SCK = 0;
        //1.右移取头部的数 再左移回来后在进行异或运算 得出结果
        buf2 = buf1 >> (7 - i);  //1111 1111 -> 0000 0001
        SER = buf2;              //取1位数据 从头到尾取 发送到位移寄存器
        SCK = 1;                 //数据寄存器的数据移位
        buf1 ^= buf2 << (7 - i); //0000 0001 -> 1000 0000  ( buf1 = 1000 0000^1111 1111=0111 1111 )
    }

    RCK = 1; //移位寄存器的数据进入数据存储寄存器 更新显示数据
#endif
//位选
#if IOversion == 0
    SMG = (wx == 0 ? 0 : 1);
#endif
#if IOversion == 1
    TaskBoard_CD4051_Chooce(wx == 0 ? 0 : 1);
#endif
}

/**
 * @description: 数码管显示两位数据
 * @param {uint8_t} number1 第一位数据
 * @param {uint8_t} number2 第二位数据
 * @return {*}
 */
void TaskBoard_SMGDisplay2(uint8_t number1, uint8_t number2)
{
    SMG_show = 1;
    SMG_data1 = number1;
    SMG_data2 = number2;

#if IOversion == 0
    TIM_Cmd(TIM2, ENABLE);
#endif
}


/**
 * @description: LED灯操作
 * 注:
 * 有的板子直接可直接使用位带操作
 * 有的板子无法关闭LED灯
 * @param {uint8_t} LeftOrRight 0：左灯 1：右灯
 * @param {uint8_t} com 0打开 1关闭
 * 使用CD4051的板子专用参数
 * @return {*}
 */
void TaskBoard_LED_show(uint8_t LeftOrRight,uint8_t status)
{
#if IOversion == 0
//G0板无法开关LED
    TaskBoard_LED=LeftOrRight;
#endif
#if IOversion == 1
    TaskBoard_CD4051_Chooce(LeftOrRight,status);
#endif
#if IOversion == 2
     if(LeftOrRight==0)TaskBoard_LED_L=status;
     else TaskBoard_LED_R=status;
#endif
}

/**
 * @description: 蜂鸣器操作
 * @param {uint8_t} status 0打开 1关闭
 * @return {*}
 */
void TaskBoard_BEEP_show(uint8_t status)
{
#if IOversion == 0
//G0板无法开关蜂鸣器 选择1时自动响起
    TaskBoard_BEEP=status;
#endif
#if IOversion == 1
    TaskBoard_CD4051_Chooce(5,status);
#endif
#if IOversion == 2
    TaskBoard_BEEP=status;
#endif
}

/**
 * @description: CD4051选择通道
 * @param {uint8_t} N 0-7：接通并选择通道
 * @param {uint8_t} com 0打开 1关闭
 * @return {*}
 */
void TaskBoard_CD4051_Chooce(uint8_t N,uint8_t com)
{

#if IOversion == 0
//G0无
#endif
#if IOversion == 1
    if (N >= 0 && N <= 7)
    {
        //Z1 : 0.SMG_A   1.0SMG_B    2.INC   3.LED_L 4.LED_R  5.BEEP  6.RI_TXD   7.out
        CD4051_COM = 0;    //选择接通通道
        CD4051_A = N >> 2; //二进制分解
        CD4051_B = N >> 1 & 1;
        CD4051_C = N & 1;
        CD4051_COM = open;
    }
#endif
}