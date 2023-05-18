/*
 * @Description: �������������
 * @Autor: 309 Mushroom
 * @Date: 2021-10-27 15:15:12
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-10-11 19:54:14
 */
#include "TaskBoard.h"
#include "ultrasonic.h" //������
#include "infrared.h"   //������
#include "bh1750.h"     //���ն�
#include "syn7318.h"    //syn7318����ģ��
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
uint8_t SMG_show = 0; //�����ˢ����ʾ
uint8_t SMG_data1 = 99, SMG_data2 = 99;
int TaskBoard_TimeCount=0;//����ʱͳ�� 
int TaskBoard_TimeIT=0;//��ϵ���ʱ
int RCC_check(int gpioX)//���IO���Ƿ�ʹ��
{
    return( (RCC->AHB1ENR>>gpioX)%2);
}


int tim2_count;//ͳ��tim2�жϴ���

/**
 * @description: ��������ʱ
 * @param {int} time ����ʱ��ֵ ��λ��ms
 * @return {*}
 */
void TaskBoard_Time(int time)
{
    tim2_count=0;
    TaskBoard_TimeIT=0;//���ô��״̬
    TaskBoard_TimeCount=time;//װ�ص���ʱֵ
    TIM_Cmd(TIM2, ENABLE);
}

void TaskBoard_TimeITOpen(void){TaskBoard_TimeIT=1;}//�жϵ���ʱ
void TaskBoard_TimeClose(void){TIM_Cmd(TIM2, DISABLE);}//�رյ���ʱ
/**
 * @description: ��ȡ����ʱ״̬
 * @param {*}
 * @return {*}0.����ʱ��� 1.����ʱ״̬ 2.���״̬  
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
        
        if (SMG_show) //��λ���������ȡ���ڶ�ʱ��2��arr �жϹ���Ӱ����İ���Ӧ�ٶ�
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

//LED�Ƴ�ʼ��
void _LED_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 5;             //0.A 1.B 2.C 3.D 4.E 5.F ʹ��GPIOʱ��
    GPIO_Set(GPIOF, PIN11, 1, 0, 1, 1); //��ʼ��GPIOF11
    TaskBoard_LED = 1;
#endif
//Z1��ʹ��cd4051  3.LED_L 4.LED_R
#if IOversion == 2
    RCC->AHB1ENR |= 1 << 7;
    GPIO_Set(GPIOH, PIN10, 1, 0, 1, 1); //��ʼ��GPIOH10  LEDL
    GPIO_Set(GPIOH, PIN11, 1, 0, 1, 1); //��ʼ��GPIOH10��LEDR
    TaskBoard_LED_L = 1;//�ر�
    TaskBoard_LED_R = 1;
#endif
}

//��������ʼ��
void _BEEP_Init(void)
{
#if IOversion == 0
    //��LEDһ��
#endif
//Z1��ʹ��cd4051   5.BEEP 

#if IOversion == 2
    RCC->AHB1ENR |= 1 << gpioC;
    GPIO_Set(GPIOC, PIN13, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP); //��ʼ��GPIOC13 BEEP
    TaskBoard_BEEP=1;
#endif
}

//��������ʼ��
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
    //Z1��ʹ��cd4051 2.INC
#endif

#if IOversion == 2
    RCC->AHB1ENR |= 1 << gpioB; //GPIOB
    RCC->AHB1ENR |= 1 << gpioA; //GPIOA
    GPIO_Set(GPIOB, PIN4, GPIO_Mode_IN, 0, 3, GPIO_PuPd_UP);//INT0
    GPIO_Set(GPIOA, PIN15, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);//INC
#endif

}

//�������߷����ʼ��
void _IR_Init(void)
{
#if IOversion == 0
    RCC->AHB1ENR |= 1 << 6; //GPIOG
    GPIO_Set(GPIOG, PIN8, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
#endif
//Z1��ʹ��cd4051 6.RI_TXD
#if IOversion == 2
    RCC->AHB1ENR |= 1 << 5; //GPIOF
    GPIO_Set(GPIOF, PIN11, GPIO_Mode_OUT, 0, 3, GPIO_PuPd_UP);
    
#endif
}

//���նȼ���ʼ��
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

//����ܳ�ʼ��
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

//CD4051��ʼ��
void _CD4051_Init(void)
{
#if IOversion == 0
    //G0��
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

//*****************************���������ʼ��*****************************
/**����������IO�ڡ�
 * 
 * һ.LED��������� 
 * {TaskBoard.c}��ʼ������
 * {TaskBoard.h}λ����������
 * 
 * ��.���������
 * {TaskBoard.c}��ʼ��������������� �������������
 * {TaskBoard.h}λ���������� ���������
 * {ultrasonic.c}1.���� Ultrasonic_EXTI()�ⲿ�жϳ�ʼ�� 2.���� Ultrasonic_Ranging()��� 3.EXTI15_10_IRQHandler()�ⲿ�жϺ���
 * 
 * ��.�����߷���
 * {TaskBoard.c}��ʼ��������������� ������
 * {TaskBoard.h}λ���������� �����
 * {infrared.c}1.Infrared_Send()
 * 
 * ��.���նȶ�ȡ
 * {TaskBoard.c}��ʼ���������������
 * {bh1750.h}λ���������� 
 * {bh1750.c}1.����SDA_OUT() 2.����SDA_IN()
 * 
 * ��.�����
 * {TaskBoard.c}��ʼ���������������
 * {TaskBoard.h}λ���������� 
 * 
 * ��.����ʶ��
 * һ��̶�����λ�� ��ͬģ�����ò�ͬ��
 * 
************************************************************************/
void TaskBoard_Init(void)
{
    _LED_Init();    //LED
    _BEEP_Init();   //������
    _WAVE_Init();   //������
    _IR_Init();     //������
    _LIGHT_Init();  //���ն�
    _SMG_Init();    //�����
    _CD4051_Init(); //CD4051��ʼ��
                    //����ͨѶ��ʼ�� �������
}
//*************************************************************************

/**
 * @description: ��������
 * @param {*}
 * @return {*}
 */
void TaskBoard_test(void)
{
#if IOversion == 0
    //������
    TaskBoard_BEEP = 1;
    delay_ms(200);
    TaskBoard_BEEP = 0;
    delay_ms(200);
    TaskBoard_BEEP = 1;

    //LED��
    TaskBoard_LED_show(0,1);
    delay_ms(500);
    TaskBoard_LED_show(1,1);

    //���������
    Send_Debug_string("WAVE=");
    Send_Debug_num(TaskBoard_WAVE());
    Send_Debug_string("\n");

    //���ⷢ�������Ϣ
    OFlag_LED_jl(200);

    //���նȻ�ȡ
    Send_Debug_string("BH=");
    Send_Debug_num(TaskBoard_BH());
    Send_Debug_string("\n");

    //����ʶ��
    SYN_Extern_wait(20);

    //�������ʾ 99
    TaskBoard_SMGDisplay2(9, 9);

    SYN7318_Play("�������"); //��������
#endif

#if IOversion == 2
      //������
    TaskBoard_BEEP = 1;
    delay_ms(200);
    TaskBoard_BEEP = 0;
    delay_ms(200);
    TaskBoard_BEEP = 1;
    //LED��
    
    TaskBoard_LED_show(0,0); //��ƿ�
    TaskBoard_LED_show(1,0); //�ҵƿ�

    //���������
    Send_Debug_string("WAVE=");
    Send_Debug_num(TaskBoard_WAVE());
    Send_Debug_string("\n");

    //���� 3��
    OFlag_light(3);

    //���նȻ�ȡ
    Send_Debug_string("BH=");
    Send_Debug_num(TaskBoard_BH());
    Send_Debug_string("\n");

    //����ʶ��
    SYN_Extern_wait(20);

    //�������ʾ��

    SYN7318_Play("�������"); //��������  
#endif
}

/**
 * @description: ��ȡ���������ֵ ��λ��mm
 * @param {*}
 * @return {*}���ֵ
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
 * @description: �����߷���
 * @param {uint8_t} *data ����
 * @param {int} n ����
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
 * @description: ��ȡ���ն�
 * @param {*}
 * @return {*}����ֵ
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
    for (i = 0; i < 10; i++) //����
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
    //ȡ�м���+ƫ��
    return (buf[5] + buf[6]) / 2;
}

uint8_t SEG_arr[19] = {0xC0, 0xF9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xff, ~0x80, ~0x73};
//����                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   �ر�   .  p
//uint8_t SEG_Table[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
//����                   0   1    2     3    4    5    6    7    8    9    A    B    C    D    E    F   �ر�
/**
 * @description: ����ܾ�̬��ʾ 
 * @param {uint8_t} wx λ��
 * @param {uint8_t} number  ����0-15(0-F) 16.ȫ�� 17.��DP �㣩 18.������λP�� 
 * @return {*}
 */
void TaskBoard_SMGDisplay(uint8_t wx, uint8_t number)
{
#if IOversion !=2  //��־���������
    int i;
    //�������� 74HC595(SER���뵥λ���� SCK������QA->QH˳����λ RCK�洢����)
    uint8_t buf1 = SEG_arr[number], buf2;
    RCK = 0;

    for (i = 0; i < 8; i++)
    {
        SCK = 0;
        //1.����ȡͷ������ �����ƻ������ڽ���������� �ó����
        buf2 = buf1 >> (7 - i);  //1111 1111 -> 0000 0001
        SER = buf2;              //ȡ1λ���� ��ͷ��βȡ ���͵�λ�ƼĴ���
        SCK = 1;                 //���ݼĴ�����������λ
        buf1 ^= buf2 << (7 - i); //0000 0001 -> 1000 0000  ( buf1 = 1000 0000^1111 1111=0111 1111 )
    }

    RCK = 1; //��λ�Ĵ��������ݽ������ݴ洢�Ĵ��� ������ʾ����
#endif
//λѡ
#if IOversion == 0
    SMG = (wx == 0 ? 0 : 1);
#endif
#if IOversion == 1
    TaskBoard_CD4051_Chooce(wx == 0 ? 0 : 1);
#endif
}

/**
 * @description: �������ʾ��λ����
 * @param {uint8_t} number1 ��һλ����
 * @param {uint8_t} number2 �ڶ�λ����
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
 * @description: LED�Ʋ���
 * ע:
 * �еİ���ֱ�ӿ�ֱ��ʹ��λ������
 * �еİ����޷��ر�LED��
 * @param {uint8_t} LeftOrRight 0����� 1���ҵ�
 * @param {uint8_t} com 0�� 1�ر�
 * ʹ��CD4051�İ���ר�ò���
 * @return {*}
 */
void TaskBoard_LED_show(uint8_t LeftOrRight,uint8_t status)
{
#if IOversion == 0
//G0���޷�����LED
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
 * @description: ����������
 * @param {uint8_t} status 0�� 1�ر�
 * @return {*}
 */
void TaskBoard_BEEP_show(uint8_t status)
{
#if IOversion == 0
//G0���޷����ط����� ѡ��1ʱ�Զ�����
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
 * @description: CD4051ѡ��ͨ��
 * @param {uint8_t} N 0-7����ͨ��ѡ��ͨ��
 * @param {uint8_t} com 0�� 1�ر�
 * @return {*}
 */
void TaskBoard_CD4051_Chooce(uint8_t N,uint8_t com)
{

#if IOversion == 0
//G0��
#endif
#if IOversion == 1
    if (N >= 0 && N <= 7)
    {
        //Z1 : 0.SMG_A   1.0SMG_B    2.INC   3.LED_L 4.LED_R  5.BEEP  6.RI_TXD   7.out
        CD4051_COM = 0;    //ѡ���ͨͨ��
        CD4051_A = N >> 2; //�����Ʒֽ�
        CD4051_B = N >> 1 & 1;
        CD4051_C = N & 1;
        CD4051_COM = open;
    }
#endif
}