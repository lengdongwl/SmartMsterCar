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
/* �������� ---------------------------------------------------------*/
uint8_t uart6_data = 0;			// USART6 �������ݻ���
uint8_t uart6_flag = 0;			// USART6 ��������ʱ��
uint8_t UART6_RxData[8];		// USART6 �������ݻ���

uint8_t voice_falg = 0;		// ����ģ�鷵��״̬
uint8_t YY_Init[5] = {0xFD, 0x00, 0x00, 0x01, 0x01};
uint8_t Zigbee[8];           // Zigbee�������ݻ���

uint8_t start_voice_dis[5]= {0xFA,0xFA,0xFA,0xFA,0xA1};
uint8_t SYN7318_Flag = 0;           // SYN7318����ʶ������ID���
uint8_t UART6_RxFlag=0;
/*******************************************************
�����ܣ���ʼ������
�Ρ�������
����ֵ����
********************************************************/
static void USART6_Hardware_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ʹ�� GPIOC ����ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* ʹ�� USART6 ����ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* ���� PC6/PC7 ���Ÿ���ӳ�� */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    /* �� PC6/PC7 ��������Ϊ���ù���ģʽ�������� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// �˿�ģʽ -> ���ù���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// �˿�������� -> �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// �˿�����ٶ� -> ���� 100MHz(30pF)
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  		// �˿�����/���� -> ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* USART6 ��ʼ������ */
    USART_InitStructure.USART_BaudRate = 115200;					// ����������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ�������� -> ��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// �շ�ģʽ���� -> ����+����
    USART_InitStructure.USART_Parity = USART_Parity_No;				// ��żУ��λ���� -> ����żУ��λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			// ֹͣλ���� -> 1λֹͣλ
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// ����λ���� -> 8λ���ݸ�ʽ
    USART_Init(USART6, &USART_InitStructure);

    /* ʹ�� USART6 �ж� */
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);		// �������ݼĴ�����Ϊ���ж�

    /* ���� USART6 �ж����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;				// ѡ�� IRQ ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	// ��ռ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;			// ��Ӧ���ȼ�����
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ���� USART6 IRQ ͨ��
    NVIC_Init(&NVIC_InitStructure);

    /* ʹ�� USART6 */
    USART_Cmd(USART6, ENABLE);
}

/*******************************************************
�����ܣ������жϺ���
�Ρ�������
����ֵ����
********************************************************/
void USART6_IRQHandler(void)
{
    /* �ж� USART6 �Ƿ񴥷�ָ���ж� -> �������ݼĴ�����Ϊ���ж� */
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        uart6_data = USART_ReceiveData(USART6);		// ��ȡ USART6 ���ݼĴ���

        if (uart6_flag == 0x00)
        {
            if (uart6_data == 0x55)				// �Զ�������֡ͷ
            {
                uart6_flag = 0x01;
                UART6_RxData[0] = uart6_data;	// ֡ͷ
                UART6_RxData[1] = 0x00;
                UART6_RxData[2] = 0x00;
                UART6_RxData[3] = 0x00;
                
            }
        }
        else if (uart6_flag == 0x01)
        {
            uart6_flag = 0x02;
            UART6_RxData[1] = uart6_data;		// ��������
            UART6_RxFlag=1;

        }
        else if(uart6_flag == 0x02)
        {
            uart6_flag = 0x03;
            UART6_RxData[2] = uart6_data;		// ״̬��־
        }
        else if(uart6_flag == 0x03)
        {
            uart6_flag = 0x00;
            UART6_RxData[3] = uart6_data;		// ����λ
            voice_falg = 0x01;					// �Զ�������֡�������
        }
        else
        {
            uart6_flag = 0x00;
            voice_falg = 0x00;
            UART6_RxData[0] = 0x00;
        }
        

    }
    //��������жϽ��ձ�־λ
    USART_ClearITPendingBit(USART6,USART_IT_RXNE);
}

/*******************************************************
�����ܣ�ͨ������1����һ���ֽڣ���0x12��0xff��
�Ρ�����hex -> �ֽ�
����ֵ����
********************************************************/
void USART6_Send_Byte(uint8_t byte)
{
    USART_SendData(USART6,byte);
    while(USART_GetFlagStatus(USART6,USART_FLAG_TXE) == RESET);
}

/*******************************************************
�����ܣ�ͨ������1����һ������
�Ρ�����*buf -> ָ��ָ��һ������
		 length -> ����ĳ���
����ֵ����
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
��  �ܣ�����ʶ����
��  ����	0�������������漴����ָ������ʶ����ԣ�2-6����ָ�������������ʶ�����
��  ����	n ʶ�����
����ֵ��	��������ID    ��������

		0x01      ��������

		0x02      ����ɽ��

		0x03      ׷������

		0x04      �﷫����

		0x05      ��ͷ����
        
		0x00      δʶ�𵽴���/ʶ��ʱ
**************************************************/
uint8_t BKRC_Voice_Extern(uint8_t yy_mode,uint8_t n)		// ����ʶ��
{
    uint32_t timers = 0;               // ����ֵ2
	
    for (int i = 0; i < n; i++)
    {
        SYN7318_Flag=0;
        UART6_RxFlag=0;
        USART6_Send_Length(start_voice_dis,5);//���Ϳ�������ʶ��ָ��
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
        //SYN7318_Flag = Voice_Drive();//���շ���״̬
        if(yy_mode==0)
        {
            OFlag_YY_cmd(0);			//�������������������
            OFlag_YY_cmd(0);			//�������������������
        }else
        {
            OFlag_YY_cmd(yy_mode);			//��������ָ������
            OFlag_YY_cmd(yy_mode);			//��������ָ������
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
            if (timers>6000)//�жϳ�ʱ�˳�
            {
                timers=0;
                break;
            }

            /*
            if(timers%1000==0)//ÿһ�뷢��һ��ʶ��
            {
                USART6_Send_Length(start_voice_dis,5);//���Ϳ�������ʶ��ָ��
                delay_ms(500);
            }*/
        }
    }
    return 0;
}


/**************************************************
��  �ܣ�����ʶ��ش������������
��  ����	��
����ֵ��	��������ID /С������ʶ��ģ��״̬
**************************************************/
uint8_t Voice_Drive(void)
{
    uint8_t status = 0;
    if ((voice_falg == 0x01) && (UART6_RxData[0] == 0x55))			// �Զ�������֡�������
    {
        if (UART6_RxData[1] == 0x02)
        {
            status &= 0xF0;
            switch (UART6_RxData[2])
            {
            case 0x01: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* �������� *");
                status |= 0x01;
                break;
            }
            case 0x02: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* ����ɽ�� *");
                status |= 0x02;
                break;
            }
            case 0x03: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* ׷������ *");
                status |= 0x03;
                break;
            }
            case 0x04: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* �﷫���� *");
                status |= 0x04;
                break;
            }
            case 0x05: {
                USART6_Send_Byte(UART6_RxData[2]);
                //                printf("* ��ͷ���� *");
                status |= 0x05;
                break;
            }
            default  :
                status=UART6_RxData[2];//����ʶ����ID
                return status;
                
            }
        }
        voice_falg = 0x00;
    }
    return status;
}

/**************************************************
��  �ܣ���������������־�ﲥ��ָ���ı���Ϣ
��  ����	*p  --> ��Ҫ���͵�����
����ֵ��	��

void YY_Play(char *p)
{
    uint16_t p_len = strlen(p);             // �ı�����

    YY_Init[1] = 0xff & ((p_len + 2) >> 8); // ���������ȸ߰�λ
    YY_Init[2] = 0xff & (p_len + 2);        // ���������ȵͰ�λ
    Send_ZigbeeData_To_Fifo(YY_Init, 5);
    Send_ZigbeeData_To_Fifo((uint8_t *)p, p_len);
    delay_ms(100);
}
**************************************************/
/**********************************************************************
 * �� �� �� ��  ��������������־�ﲥ��������������
 * ��    �� ��  Primary   -> ��ָ��
                Secondary -> ��ְ��
                �����¼1
 * �� �� ֵ ��  ��
 * ��    �� ��  YY_Comm_Zigbee(0x20, 0x01);     // �������������������

��¼1��
-----------------------------------------------------------------------
| Primary | Secondary | ˵��
|---------|-----------|------------------------------------------------
|  0x10   |  0x02     | ��������
|         |  0x03     | ����ɽ��
|         |  0x04     | ׷������
|         |  0x05     | �﷫����
|         |  0x06     | ��ͷ����
|---------|-----------|------------------------------------------------
|  0x20   |  0x01     | ���ָ��
|---------|-----------|------------------------------------------------
***********************************************************************/

/*******************************************************
�����ܣ�����ʶ���ʼ������
�Ρ�������
����ֵ����
********************************************************/
void BKRC_Voice_Init(void)
{
    USART6_Hardware_Init();
}

//										endfile

/**
 * @description: ����0-9
 * @param {int} number
 * @return {*}
 */
void XiaoChuang_PlayNUMbit(int number)
{
/*
A0::��:5503A000
A1::һ:5503A100
A2::��:5503A200
A3::��:5503A300
A4::��:5503A400
A5::��:5503A500
A6::��:5503A600
A7::��:5503A700
A8::��:5503A800
A9::��:5503A900
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
 * �� �� �� ��  С�������������ִ�С
 * ��    �� ��  ������ֵ 0 �� 999
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע �� ��:105 һ������
*****************************************************************/
void XiaoChuang_PlaySIZE(int number)
{	int t = 800; //�����ϳɼ��/us û�м�����������
	int buf = number;
	int len = 1;
	int bufARR[20];//�����ݴ��������
	if(number>9 || number<-9)
	{
		while (buf/=10)//��������λ��
		{
			if(len==1)//������λ
			{
				bufARR[len]=number%10;
			}else //�����м�λ
			{
				bufARR[len]=pow(10,(len-1));
				bufARR[len]=number/bufARR[len]%10;
			}
			
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
			len++;
		}
		if(len>1)
		{
			//����ĩλ
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
		if(bufARR[2]>1)//��ֹ����һʮ
		{
			XiaoChuang_PlayNUMbit(bufARR[2]);//n
			delay_us(t);
		}
		USART6_Send_Byte(0xAA);//ʮ
		delay_us(t);
		if(bufARR[1]!=0)//��λΪ0������
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
			delay_us(t);
		}
		break;
	case 3://100   320  409    111
		XiaoChuang_PlayNUMbit(bufARR[3]);//n����λ�ϵ���ֵ��
		delay_us(t);
		USART6_Send_Byte(0xAB);//��
		delay_us(t);
		if(bufARR[2]!=0)//110 111 (ʮλ��Ϊ��)
		{
			XiaoChuang_PlayNUMbit(bufARR[2]);
			delay_us(t);
			USART6_Send_Byte(0xAA);//ʮ
			delay_us(t);
			if(bufARR[1]!=0)//��ֹ����ʮ��
			{
				XiaoChuang_PlayNUMbit(bufARR[1]);
				delay_us(t);
			}
		}else//101 (ʮλΪ��)
		{
			if(bufARR[2]==0 && bufARR[1]!=0)//x0x ��x00�򲻲���ʮλ���λ
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
		if(bufARR[3]!=0)//ǧ
		{
			XiaoChuang_PlayNUMbit(bufARR[3]);
			USART6_Send_Byte(0xAB);
		}//n0nn
		
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//ʮ
		if(bufARR[1]!=0)//��ֹ����ʮ��
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;
	case 5:
		XiaoChuang_PlayNUMbit(bufARR[5]);//��
		USART6_Send_Byte(0xAD);
		XiaoChuang_PlayNUMbit(bufARR[4]);
		USART6_Send_Byte(0xAC);
		XiaoChuang_PlayNUMbit(bufARR[3]);
		USART6_Send_Byte(0xAB);
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//ʮ
		if(bufARR[1]!=0)//��ֹ����ʮ��
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;
	case 6:
		if(bufARR[6]>1)
		{
			XiaoChuang_PlayNUMbit(bufARR[6]);
		}
		USART6_Send_Byte(0xAA);//ʮ
		XiaoChuang_PlayNUMbit(bufARR[5]);//��
		USART6_Send_Byte(0xAD);
		XiaoChuang_PlayNUMbit(bufARR[4]);
		USART6_Send_Byte(0xAC);
		XiaoChuang_PlayNUMbit(bufARR[3]);
		USART6_Send_Byte(0xAB);
		XiaoChuang_PlayNUMbit(bufARR[2]);
		USART6_Send_Byte(0xAA);//ʮ
		if(bufARR[1]!=0)//��ֹ����ʮ��
		{
			XiaoChuang_PlayNUMbit(bufARR[1]);
		}
		break;*/
	}

	
}

void XiaoChuang_PlayNUMBER(int number)
{	int t = 800; //�����ϳɼ��/us û�м�����������
	int buf = number;
	int len = 1;
	int bufARR[20];//�����ݴ��������
	if(number>9 || number<-9)
	{
		while (buf/=10)//��������λ��
		{
			if(len==1)//������λ
			{
				bufARR[len]=number%10;
			}else //�����м�λ
			{
				bufARR[len]=pow(10,(len-1));
				bufARR[len]=number/bufARR[len]%10;
			}
			
			//printf("bufARR[%d]=%d\n",len,bufARR[len]);
			len++;
		}
		if(len>1)
		{
			//����ĩλ
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

