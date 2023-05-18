#include "stm32f4xx.h"
#include "delay.h"
#include "string.h"
#include "cba.h"
#include "OperationFlag.h"
#include "syn7318.h"
#include "canp_hostcom.h"
/*********************************************************************
�� ����     ��[=tiao2]�⵵[=dang3]"
������ѡ��[m3]���[m3]  ����
						[m51]���[m3] ��С��
						[m52]���[m3] ��Сǿ
						[m53]���[m3] ������
						[m54]���[m3] ����Ѽ
						[m55]���[m3] С����
���ִ���	[n0]����123456��123����[d] �Զ��ж�
						[n1]����123456��123����[d] ǿ�ƺϳɺ������ִ���һ��һ������
						[n2]����123456��123����[d] ǿ�ƺϳ���ֵ���ִ�����С��
������      ���[p1000]�ټ� ������ú���ʱ1s�ڶ��ټ�
�����ϣ�    [r0]��λ�ĵ�С������[d]	�Զ��ж���������
						[r1]��λ�ĵ�С������[d]	ǿ�ƿ�ͷ��������
						[r2]��λ�ĵ�С������[d]	�������ĺ���ǿ�ư�������
���٣�		  [s5]���[d] 5�����ٲ���
						[s8]���[d] 8�����ٲ���
						[s2]���[d] 2�����ٲ���
�����		  [t5]���[d] 5���������
						[t8]���[d] 8���������
						[t2]���[d] 2���������
�������ڣ�  [v5]���[d] 5����������
						[v8]���[d] 8����������
						[v2]���[d] 2����������
��ʾ����    [x0]sounda msga[d]	����������Ӣ����ĸ		 			
						[x1]sounda msga[d]  ��ʾ��
����1��			[y0]1008611[d]	�������������
						[y1]1008611[d]	һ�������һһ
�������  [f0]һ��������[d] һ��һ��
						[f0]һ��������[d] ƽ��ֱ��
�������ţ�[b0]��ӭ���٣�[d] ������̾��
						[b1]��ӭ���٣�[d] ��ӭ���� ��ͨ��
**********************************************************************/
uint8_t	TIM14_break = 0; //�����־λ
uint8_t	TIM14_count = 0; //�жϼ���
uint8_t Back[4] = {0};   //��������ش�������
uint8_t ASR[6] = {0};    //����ʶ�����ش�������.
uint8_t S[4] = {0};      //����ģ�鵱ǰ����״̬�ش�������

uint8_t Wake_Up[] = {0xfd,0x00,0x02,0x51,0x1F};
uint8_t Stop_Wake_Up[] = {0xFD,0x00,0x01,0x52};

uint8_t Start_ASR_Buf[] = {0xFD,0x00,0x02,0x10,0x04};
uint8_t Stop_ASR_Buf[] = {0xFD,0x00,0x01,0x11};

uint8_t Play_MP3[] ={ 0xFD,0x00,0x1E,0x01,0x01,0xC6,0xF4,0xB6,0xAF,0xD3, 0xEF ,0xD2, 0xF4,
                            	0xBF, 0xD8, 0xD6 ,0xC6 ,0xBC ,0xDD ,0xCA ,0xBB ,0xA3 ,0xAC, 0xC7, 0xEB,
                             	0xB7, 0xA2, 0xB3, 0xF6 ,0xD6, 0xB8, 0xC1, 0xEE };
uint8_t Ysn7813_flag=0;

#define SYN7318_RST_H GPIO_SetBits(GPIOB,GPIO_PinSource9)
#define SYN7318_RST_L GPIO_ResetBits(GPIOB,GPIO_PinSource9)

void Yu_Yin_Asr(void);		

void USART6_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	USART_InitTypeDef USART_TypeDefStructure;						
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	
	//PC6->Tx    PC7->RX
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;   //�������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
	
/*	//PC7-RX
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//����
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP; 	//����
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);*/
	
	USART_TypeDefStructure.USART_BaudRate = baudrate;					   //������
	USART_TypeDefStructure.USART_HardwareFlowControl = 				       //��Ӳ��������
												 USART_HardwareFlowControl_None;  
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; 		//�����뷢��ģʽ
	USART_TypeDefStructure.USART_Parity = USART_Parity_No; 		       		//��У��λ
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;        		//ֹͣλ1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;   		//����λ8λ
	USART_Init(USART6,&USART_TypeDefStructure);
	

	USART_Cmd(USART6,ENABLE);
	USART_ClearFlag(USART6, USART_FLAG_TC);						//���������ɱ�־λ
	USART_ClearFlag(USART6, USART_FLAG_RXNE);					//���������ɱ�־λ
}



/**********************************************************************
 * �� �� �� ��  ��ʱ��14�жϷ�����
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET) //����ж�
	{
		TIM14_count += 1;
		if (TIM14_count > 20) //20s��ʱ
		{
			TIM14_break = 1; //��ʱ��־
			TIM14_count = 0; //�������
		}
	}
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update); //����жϱ�־λ
}



//����һ���ַ�
int U6SendChar(int ch) 
{
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_TXE)));
	USART_SendData(USART6,ch&0x1FF);
	return (ch);
}
/*********************************************************************
���� �� ������SYN7318_Put_Char----����һ���ֽں���
������˵������txd---�����͵��ֽڣ�8λ��
����    ������SYN7318_Put_Char('d');  ���͡�d��
*********************************************************************/
void SYN7318_Put_Char(uint8_t txd)
{
	U6SendChar(txd);
}

/*********************************************************************
���� �� ������SYN7318_Put_String----�����ַ�������
������˵������Pst������ַ�����������
              Length���ַ�������
����    ������uchar d[4] = {0x00,0x01,0x02,0x03};
              SYN7318_Put_String(d,4); ---��������d�е�Ԫ��
*********************************************************************/
void SYN7318_Put_String(uint8_t* Pst,uint8_t Length)
{ 
   	uint8_t i;
	for(i = 0; i < Length; i++)
	{
		SYN7318_Put_Char( Pst[i]);
	}
}

/*********************************************************************
���� �� ������SYN7318_Get_char----����һ���ֽں���
������˵�������޲�
���� �� ֵ�������յ����ֽ�
����    ������uchar d;
              d = SYN7318_Get_char();
              SYN7318_Get_char(d); -----������յ����ֽ�
*********************************************************************/
uint8_t SYN7318_Get_char(void) 
{
  	uint8_t return_data=0;	
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_RXNE)));
	return_data = (uint8_t)USART_ReceiveData(USART6);
	return return_data;	
}

/*********************************************************************
���� �� ������SYN7318_Get_String----�����ַ�������
������˵������Pst����Ž��յ����ַ�����������
              Length���ַ�������
����    ������uchar d[4] = {0};
              SYN7318_Get_String(d,4);
              SYN7318_Get_String(d,4);----������յ����ַ��� 
*********************************************************************/
void SYN7318_Get_String(uint8_t *Pst,uint8_t Length)
{
	uint8_t i;
	for(i=0; i<Length; i++)
	{
		Pst[i] = SYN7318_Get_char();
	}
}

uint8_t SYN7318_Rst(void)  //����ģ�鸴λ
{
	SYN7318_RST_H;	
	delay_ms(10); 
	SYN7318_RST_L;	
	delay_ms(100);	
	SYN7318_RST_H;
	while(0x55 == SYN7318_Get_char());
	SYN7318_Get_String(Back ,3 );
	if(Back[2] ==0x4A )  return 1;
	else return 0;
}

void SYN7318_Init(void)
{
	USART6_Init(115200);
	
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		// ���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;		// �������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_9);							// Ĭ��Ϊ�ߵ�ƽ
}
/*********************************************************************
���� �� ������SYN7318_Play----�����ϳɲ��ź���
������˵������Pst�����Ҫ�ϳɲ��ŵ��ı���������
����    ������uchar Data[] = {"��������"};
              SYN7318_Play(Data); -----�ϳɲ��ű�������
*********************************************************************/
void SYN7318_Play(char *Pst)
{
	uint8_t Length;
	uint8_t Frame[5];   //���淢�����������
	
	Length = strlen((char *)Pst);
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x01;      //�����ϳɲ�������
	Frame[4] = 0x00;      //���ű����ʽΪ��GB2312��
	TIM_Cmd(TIM14, ENABLE);					   //ʹ�ܶ�ʱ��14������20s��ʱ��⣩
	SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String((uint8_t *)Pst, Length);
	
	SYN7318_Get_String(Back,4);	
	//���ճɹ�
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
		if (TIM14_break == 1) //��ʱ
		{
			break;
		}
	}
		
	//���м��
	SYN7318_Get_String(Back,4);
	while(!(Back[3] == 0x4f))
	{
		SYN7318_Get_String(Back,4);
		if (TIM14_break == 1) //��ʱ
		{
			break;
		}
	}
	TIM_Cmd(TIM14, DISABLE); //ʧ�ܶ�ʱ��14���ر�20s��ʱ��⣩
	TIM14_count = 0;			 //�����ʱ��14��ʱ��־λ
}

/*********************************************************************
���� �� ������Start_ASR----��ʼ����ʶ����
������˵������Dict���ʵ���
����    ������Start_ASR(0x00); ----ʶ��0x00�ʵ��еĴ���
*********************************************************************/
void Start_ASR(uint8_t Dict)
{
	uint8_t Frame[5];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //��ʼ����ʶ������
	Frame[4] = Dict;      //�ʵ���  �������޸���Ҫʶ��Ĵʵ���
	ASR[3]=0;
	
	SYN7318_Put_String(Frame, 5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)
	{
		SYN7318_Get_String(Back,3);  //����ʶ������ش����
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
		}
	}
}

/*********************************************************************
���� �� ������Stop_ASR----ֹͣ����ʶ����
������˵�������޲�
*********************************************************************/
void Stop_ASR()
{
	uint8_t Frame[4];		// ���淢�����������

	Frame[0] = 0xFD;		// ֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;
	Frame[3] = 0x11;		// ֹͣ����ʶ������

	SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4);
}

/***************************************************************************
���� �� ������Status_Query----ģ��״̬��ѯ����
������˵�������޲�
����    ������Status_Query(); 
***************************************************************************/
void Status_Query()
{
	uint8_t Frame[4];		// ���淢�����������
	
	Frame[0] = 0xFD;		// ֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x21;		// ״̬��ѯ����
	
	SYN7318_Put_String(Frame, 4);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)  
	{
		SYN7318_Get_String(S,4);  // ģ�鵱ǰ����״̬�Ļش����
	}
}

void SYN7318_Test( void)  // ������������
{
	Ysn7813_flag = 1;
	// SYN7318_Init();

	SYN7318_Play("����ʶ�����,�뷢�������Ѵʣ�������ʻ");
	LED1 = 1;
	Status_Query();  //��ѯģ�鵱ǰ�Ĺ���״̬
	if(S[3] == 0x4F)  //ģ����м���������
	{
		LED2 = 1;
		delay_ms(1);

		SYN7318_Put_String(Wake_Up,5);//���ͻ���ָ��
		SYN7318_Get_String(Back,4);   //���շ�����Ϣ
		if(Back[3] == 0x41)         //���ճɹ�
		{
			LED3 = 1;
			SYN7318_Get_String(Back,3); //����ǰ��λ�ش�����
			if(Back[0] == 0xfc)       //֡ͷ�ж�
			{
				LED4 = 1;
				SYN7318_Get_String(ASR,Back[2]);//���ջش�����
				if(ASR[0] == 0x21)            //���ѳɹ�
				{
					SYN7318_Put_String(Play_MP3,33);//���š������⡱ 
					SYN7318_Get_String(Back,4);
					SYN7318_Get_String(Back,4);
					while(!(Back[3] == 0x4f))    //�ȴ�����
					{
						LED2 = ~LED2;
						delay_ms(500);
					}
					//��ʼ����ʶ��
					while(Ysn7813_flag)
					{
						SYN7318_Put_String(Start_ASR_Buf,5);//������ʶ������
						SYN7318_Get_String(Back,4);     //���շ�����Ϣ
						if(Back[3] == 0x41)           //���ճɹ�
						{
							LED1 = ~LED1;             //LED1��ת
							SYN7318_Get_String(Back,3);  //����ʶ������ش����
							if(Back[0] == 0xfc)        //֡ͷ�ж�
							{
								LED2 = ~LED2;
								SYN7318_Get_String(ASR,Back[2]);//���ջش�����	                    
								Yu_Yin_Asr();
							}
						}
					}
					SYN7318_Put_String(Stop_Wake_Up,4);//����ֹͣ����ָ��
				}
				else                          //�����ڲ�����
				{

				}
			}
		}				
	}
}


void Yu_Yin_Asr(void)  // ����ʶ������
{
	switch(ASR[0])
	{
		case 0x01:		
		{
			switch(ASR[3])
			{
				case 0x00:  // ��һ����һ�㲻����ʶ��
				{
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x01:  // ǰ��
				{
					SYN7318_Play("С��ǰ�������");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x02:
				{
					SYN7318_Play("С����������ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ�� 
					break;
				}
				case 0x03:
				{
					SYN7318_Play("С����ת��ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��;
					break;
				}
				case 0x04:
				{
					SYN7318_Play("С����ת��ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x05:
				{
					SYN7318_Play("��ͣ�����ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x06:
				{
					SYN7318_Play("Ѱ������ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x07:
				{
//					Infrared_Send(HW_K,6);	//�򿪲��Ժ��ⱨ��
					SYN7318_Play("�������Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x08:
				{
//					Send_ZigbeeData_To_Fifo(DZ_K ,8);  // ������բ
					SYN7318_Play("��բ�Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x09:
				{
//					Send_ZigbeeData_To_Fifo( DZ_G ,8);  // ������բ
					SYN7318_Play("��բ�ѹرգ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x0a:
				{
//					Send_ZigbeeData_To_Fifo( SMG_SHOW ,8);  // �������ʾ
					SYN7318_Play("LED��ʾ�ѿ����ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��										
					break;
				}
				case 0x0b:
				{
//					Send_ZigbeeData_To_Fifo( SMG_JSK ,8);  // ����ܼ�ʱ
					SYN7318_Play("��ʱϵͳ�Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x0c:
				{
//					Send_ZigbeeData_To_Fifo( SMG_JSG ,8);  // ����ܹر�
					SYN7318_Play("��ʱϵͳ�ѹرգ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x0d:
				{
//					Send_ZigbeeData_To_Fifo( SMG_JL ,8);  // �������ʾ����
					SYN7318_Play("LED��ʾ��������ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x0e:
				{
					SYN7318_Play("�����ʾ���Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x0f:
				{
					SYN7318_Play("�Ҳ���ʾ���Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��														
					break;
				}
				case 0x10:
				{
					SYN7318_Play("��ʾ���ѹرգ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x11:
				{
					SYN7318_Play("�������Ѵ򿪣��ȴ���һ��ָ��");
					delay_ms(500);
					//BEEP =1;
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x12:
				{
					//						BEEP =1;  //�ط�����
					SYN7318_Play("�������ѹرգ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x13:
				{
//					Infrared_Send(CP_SHOW1,6);
//					delay_ms(500);
//					Infrared_Send(CP_SHOW2,6);

					SYN7318_Play("��������ʾ���ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x14:
				{
//					Infrared_Send(H_SD,4);
					SYN7318_Play("����ŷ�ϵͳ�Ѵ򿪣��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x15:
				{
//					Infrared_Send(H_S,4);
					SYN7318_Play("ͼƬ��ҳ��ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x16:
				{
//					Infrared_Send(H_1,4);
					SYN7318_Play("��[=tiao2]�⵵[=dang3]λ�Ѽ�1���ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					break;
				}
				case 0x17:
				{									
					SYN7318_Play("��ӭʹ�ñ����ٿ���[=rong2]����������С��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��					
					break;
				}
				case 0x18:
				{
					SYN7318_Play("��ʼԭ�ص�ͷ�����Ժ�");

					/*															while(G_Flag);  //�ȴ�ǰ�����
					Left_Test( 80); 
					while(L_Flag);
					Left_Test( 80); 
					while(L_Flag);*/
					SYN7318_Play("ԭ�ص�ͷ����ɣ��ȴ���һ��ָ��");
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��	
					break;
				}
				case 0x19:
				{
					SYN7318_Play("�õģ�����������������"); 
					SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
					Ysn7813_flag = 0;
					//Ysn7813_flag_cs =0;
					break;
				}
			}
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x02: //ʶ��ɹ���������ID�ţ�
		{
			SYN7318_Play("�Բ�����û����");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x03://�û�������ʱ
		{
			SYN7318_Play("�������ߣ���ʱΪ������");
			Ysn7813_flag = 0;
			//												Ysn7813_flag_cs = 0;
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x04:
		{
			SYN7318_Play("�밲�����鷳����˵һ��");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x05:
		{
			SYN7318_Play("�Բ�������˵һ��");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x06:
		{
			SYN7318_Play("ʶ���ڲ�����");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		case 0x07:
		{
			SYN7318_Play("�Բ�������˵һ��");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
		default:
		{
			SYN7318_Play("����");
			SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
			break;
		}
	}
}


/***************************************************************
* �� �� �� ��	SYN7318����ʶ��
* ��    �� ��	��
* �� �� ֵ ��	
***************************************************************/
uint8_t SYN_Extern(void)		// ����ʶ��
{
	uint8_t SYN7318_Flag = 0;
	SYN7318_Flag = 0;
	Status_Query();		//��ѯģ�鵱ǰ�Ĺ���״̬
	if(S[3] == 0x4F)	//ģ����м���������
	{
		SYN7318_Put_String(Start_ASR_Buf,5);	// ������ʶ������
		SYN7318_Get_String(Back,4);				// ���շ�����Ϣ
		if(Back[3] == 0x41)						// ���ճɹ�
		{
			LED1 = ~LED1;						// LED1��ת
			delay_ms(200);
			OFlag_YY(0x20,0x01,0,0);		// �������������������
			
			SYN7318_Get_String(Back,3);			// ����ʶ������ش����
			if(Back[0] == 0xFC)					// ֡ͷ�ж�
			{
				LED2 = ~LED2;
				SYN7318_Get_String(ASR,Back[2]);	//���ջش�����
				
				switch(ASR[0])
				{
					case 0x01:
					{
						switch(ASR[5])
						{
							case 0x01:
							{
								SYN7318_Play("��������");	
								SYN7318_Flag=0x01;
								break;				
							}
							case 0x02:
							{
								SYN7318_Play("����ɽ��");	
								SYN7318_Flag=0x02;					
								break;				
							}
							case 0x03:
							{
								SYN7318_Play("׷������");	
								SYN7318_Flag=0x03;
								break;				
							}			
							case 0x04:
							{
								SYN7318_Play("�﷫��");	
								SYN7318_Flag=0x04;
								break;				
							}
							case 0x05:
							{
								SYN7318_Play("��ͷ����");	
								SYN7318_Flag=0x05;
								break;				
							}
							case 0x06:
							{
								SYN7318_Play("��ǿ·վ");	
								SYN7318_Flag=0x06;
								break;				
							}
							case 0x07:
							{
								SYN7318_Play("����·վ");	
								SYN7318_Flag=0x07;
								break;				
							}
							case 0x08:
							{
								SYN7318_Play("����·վ");	
								SYN7318_Flag=0x08;
								break;				
							}
							case 0x09:
							{
								SYN7318_Play("��г·վ");	
								SYN7318_Flag=0x09;
								break;				
							}
							case 0x10:
							{
								SYN7318_Play("����·վ");	
								SYN7318_Flag=0x10;
								break;				
							}
							case 0x11:
							{
								SYN7318_Play("��ҵ·վ");	
								SYN7318_Flag=0x11;
								break;				
							}
							case 0x12:
							{
								SYN7318_Play("����·վ ");	
								SYN7318_Flag=0x12;
								break;				
							}



							default:
							{
								
								SYN7318_Play("����ID����");
								SYN7318_Flag = 0x00;
								break;
							}
						}
						break;
					} 
					
					case 0x02: //ʶ��ɹ���������ID�ţ�
					{
						SYN7318_Play("������ID");
						SYN7318_Flag = 0x00;
						break;
					}
					case 0x04:
					{
						SYN7318_Play("�밲��");
						SYN7318_Flag = 0x00;
						break;
					}
					default:
					{
						SYN7318_Play("��û����");
						SYN7318_Flag = 0x00;
						break;
					}
				}
			}
		}
	}
	return SYN7318_Flag;
}

/***************************************************************
* �� �� �� ��	�ȴ�SYN7318����ʶ����ɲ��ϴ�����
* ��    �� ��	count ʶ������������
* �� �� ֵ ��	
***************************************************************/
/*
void SYN_Extern_wait(uint8_t count)
{
	uint32_t i=0,r=0;
	while(r==0)
	{
		r=SYN_Extern();
		if(r!=0)
		{
			break;
		}

		i++;

		if(count<=i)//��ʱ����
		{
			r=0;
			break;
		}
	}
	if(r!=0)//ʶ��ɹ�
	{
		OFlag_YYupload(r);//�ϴ�����
		//SYN7318_Play("�ϴ����");
	}
}*/

//countʶ�����
uint32_t SYN_Extern_wait(uint8_t count)
{
	uint32_t i=0,r=0;
	//SYN7318_Play("��ʼʶ��");
	while(count--)
	{
		r=SYN_Extern();
		if(r!=0)
		{
			break;
		}
		i++;
	}
	
	if(r!=0)//ʶ��ɹ�
	{
		OFlag_YYupload(r);//�ϴ�����
	}else
	{
		OFlag_YYupload(0x02);//�ϴ�����
	}
}


/**
 * @description: ���˷����� �����ϳɲ��ź���
 * @param {char} *str
 * @return {*}
 */
void SYN7318_Play_NotAscll(char *str)
{
	char newstr[100],*p;
	int i=0;
	int len=0;//���˺�ĳ���
    while(*str)
    {
    	if(0>=*str || *str>127)
    	{
    		newstr[i]=*str;
    		//printf("%d\n",*str);
    		i++;
    	}else
    	{
    		//printf("del - >%c\n",*str);
    	}
        str++;
    }
    newstr[i]='\0';
    //len=i;//���������ȸպ�=i
    p=newstr;
    SYN7318_Play(p);
}