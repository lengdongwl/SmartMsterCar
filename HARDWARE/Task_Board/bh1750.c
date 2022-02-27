#include "stm32f4xx.h"
#include "delay.h"
#include "bh1750.h"
#include "drive.h"

#define	 SlaveAddress   0x46 			//����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
uint8_t  BUF[4];                  //�������ݻ�����
uint16_t illumination_data; 										//���ն�ȡֵ

void BH1750_PortInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOG,ENABLE);
#if versions==0
	//PB6-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
#endif
#if versions==1
	//PB6-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//PB7-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif	
#if versions==2
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	//PB7-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==3
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	//PB7-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==4
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	//PB7-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif	
#if versions==5
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	//PB7-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif	

//#IOVersion ����������
}




void SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if versions==0
	// PB7-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==1
	// PG15-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
#endif	
#if versions==2
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
#endif
#if versions==3
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
#endif
#if versions==4
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
#endif
#if versions==5
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
#endif

#if IOversion==0
	//PB6-SDA
	GPIO_Set(GPIOB,PIN6,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_100MHz,0);
#endif
#if IOversion==1
	//PB7-SDA	
	GPIO_Set(GPIOB,PIN7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_100MHz,0);
#endif
#if IOversion==2
	//PB7-SDA	
	GPIO_Set(GPIOB,PIN7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_Speed_100MHz,0);
#endif
}

void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if versions==0
	// PB7-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==1
	// PG15-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
#endif	
#if versions==2
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==3
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==4
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
#if versions==5
	// PB6-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif

#if IOversion==0
	// PB6-SDA	
	GPIO_Set(GPIOB,PIN6,GPIO_Mode_IN,GPIO_OType_PP,GPIO_Speed_100MHz,GPIO_PuPd_UP);
#endif
#if IOversion==1
	// PB6-SDA	
	GPIO_Set(GPIOB,PIN6,GPIO_Mode_IN,GPIO_OType_PP,GPIO_Speed_100MHz,GPIO_PuPd_UP);
#endif
#if IOversion==2
	// PB7-SDA	
	GPIO_Set(GPIOB,PIN7,GPIO_Mode_IN,GPIO_OType_PP,GPIO_Speed_100MHz,GPIO_PuPd_UP);
#endif
}

/***************************************************************
** ���ܣ�     ����IIC��ʼ�ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_Start()
{
    SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/***************************************************************
** ���ܣ�     ����IICֹͣ�ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_Stop()
{
	SDA_OUT();//sda�����
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1;
 	delay_us(4);
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);
}

/***************************************************************
** ���ܣ�     ����IICӦ���ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_SendACK(uint8_t ack)
{
    SDA_OUT();          //sda�����
	if(ack)IIC_SDA=1;   //дӦ���ź�
	else IIC_SDA=0;          
    IIC_SCL=1;          //����ʱ����
    delay_us(2);        //��ʱ
    IIC_SCL=0;          //����ʱ����
    delay_us(2);        //��ʱ
}

/***************************************************************
** ���ܣ�     ����IIC�����ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
uint8_t BH1750_RecvACK()
{
    uint8_t data;
	  SDA_IN();         //SDA����Ϊ����
    IIC_SCL=1;        //����ʱ����
    delay_us(2);      //��ʱ
    data = READ_SDA;  //��Ӧ���ź�
    IIC_SCL=0;        //����ʱ����
    delay_us(2);      //��ʱ
    return data;
}

/***************************************************************
** ���ܣ�     ��IIC���߷���һ���ֽ�����
** ������	  dat��һ�ֽ�����
** ����ֵ��    ��
****************************************************************/
void BH1750_SendByte(uint8_t dat)
{
    uint8_t i,bit;
	SDA_OUT();               //sda�����
    for (i=0; i<8; i++)      //8λ������
    {
	    bit=dat&0x80;
		if(bit) IIC_SDA=1;
		else IIC_SDA=0;
        dat <<= 1;           //�Ƴ����ݵ����λ
        IIC_SCL=1;           //����ʱ����
        delay_us(2);         //��ʱ
        IIC_SCL=0;           //����ʱ����
        delay_us(2);         //��ʱ
    }
    BH1750_RecvACK();
}

/***************************************************************
** ���ܣ�     ��IIC���߽���һ���ֽ�����
** ������	  �޲���
** ����ֵ��   dat������һ�ֽ�����
****************************************************************/
uint8_t BH1750_RecvByte()
{
    uint8_t i;
    uint8_t dat = 0;
	SDA_IN();                 //SDA����Ϊ����
    IIC_SDA=1;              //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)     //8λ������
    {
        dat <<= 1;
        IIC_SCL=1;          //����ʱ����
        delay_us(2);        //��ʱ
		if(READ_SDA) dat+=1;               
        IIC_SCL=0;          //����ʱ����
        delay_us(2);        //��ʱ
    }
    return dat;
}

/***************************************************************
** ���ܣ�     ��bh1750д������
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void Single_Write_BH1750(uint8_t REG_Address)
{
    BH1750_Start();                  //��ʼ�ź�
    BH1750_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    BH1750_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    BH1750_Stop();                   //����ֹͣ�ź�
}

/***************************************************************
** ���ܣ�     ��������BH1750�ڲ����� 
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void Multiple_Read_BH1750(void)
{   uint8_t i;	
    BH1750_Start();                          //��ʼ�ź�
    BH1750_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	
	for (i=0; i<3; i++)                      //������ȡ2����ַ���ݣ��洢��BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 3)
        {
           BH1750_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //��ӦACK
       }
   }
    BH1750_Stop();                          //ֹͣ�ź�
}

/***************************************************************
** ���ܣ�     ��ʼ��BH1750
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void BH1750_Configure(void)
{
   BH1750_PortInit();
   Single_Write_BH1750(0x01);  
   ADDR = 0;  						//��ADDRλ��ʼ������
}

/***************************************************************
** ���ܣ�     ��ȡ���ն�
** ������	  �޲���
** ����ֵ��   data�����ع��ն�ֵ
****************************************************************/
uint16_t Get_Bh_Value(void)
{  
	float temp;
	int dis_data;
	BUF[0]=0;BUF[1]=0;BUF[2]=0;BUF[3]=0;
	illumination_data=0;
    Single_Write_BH1750(0x01);			// power on
    Single_Write_BH1750(0x10);			// H- resolution mode
    Multiple_Read_BH1750();				// �����������ݣ��洢��BUF��
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1];		// �ϳ����ݣ�����������
    temp=(float)(dis_data/1.2);
	illumination_data=(int)temp;
    return illumination_data;//���ն�ȡֵ
}
uint16_t Get_Bh_Value_Buffer(void)
{
	return illumination_data;
}
/**********************************************************************
 * �� �� �� ��  ��ȡ���նȣ���Σ�
 * ��    �� ��  ��
 * �� �� ֵ ��  ��
 * ȫ�ֱ��� ��  ��
 * ��    ע ��  ��
*****************************************************************/
void illumination_display(void)
	{		
				int i,j,temp;
				float arr[8]={0};
		
				for(i=0;i<8;i++)
				{
						Get_Bh_Value();     	//��ȡ���ն�
						delay_ms(1);
						//delay_ms(100);
						arr[i]=illumination_data;
				}
				
				for(i=0;i<8;i++) 
				{
					for(j=0;j<7-i;j++) 
					{
						if(arr[j]>arr[j+1]) 
						{
							temp = arr[j];
							arr[j] = arr[j+1];
							arr[j+1] = temp; 
						}
					}
				}
				//TFT_Test_Zigbee('A',0x40,0x00,illumination_data/256,illumination_data%256);	  // ������Ϣ��ʾ
	}
