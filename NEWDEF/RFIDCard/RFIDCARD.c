/*
 * @Description: ����RC522���� RFIDCard�Ĳ���
 * @Autor: 309
 * @Date: 2021-10-11 10:56:06
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 16:47:53

RFID��д��˵����Mifare S50 RFID��
������8Kb
16������  64����ַ��  ÿ����洢16���ֽ� ÿ�������������������
--------------------------------------------------|
	     |��0|	����0 ��0 ���ɸ���	  |���ݿ�| 0  |
  ����0  |��1|					      |���ݿ�| 1  |
         |��2|				    	  |���ݿ�| 2  |
	     |��3|	����A ���ƿ� ����B    |���ƿ�| 3  |
--------------------------------------------------|
	     |��0|					      |���ݿ�| 4  |
  ����1  |��1|					      |���ݿ�| 5  |
         |��2|				    	  |���ݿ�| 6  |
	     |��3|	����A ���ƿ� ����B    |���ƿ�| 7  |
--------------------------------------------------|
��������������������������������������������������|
--------------------------------------------------|
         |��0|					      |���ݿ�| 60 |
  ����15 |��1|					      |���ݿ�| 61 |
         |��2|				    	  |���ݿ�| 62 |
	     |��3|	����A ���ƿ� ����B    |���ƿ�| 63 |
--------------------------------------------------|
RFID���ƿ����ݣ�
0xff,0xff,0xff,0xff,0xff,0xff
����A��Ĭ�����룩
0xff,0x07,0x80,0x69,0xff
��ȡ���� 
0xff,0xff,0xff,0xff,0xff
����B	 						
��������������֤��Կ�󣬸�����Ҫ�޸ĵ�������3��
 */
#include "RFIDCARD.h"
#include "rc522.h"
#include "PID.h"
#include "MasterCarCotrol.h"
#include "delay.h"
#include "canp_hostcom.h"
#include "stdio.h"
#include "roadway_check.h"
#include "OperationFlag.h"
#include "stdarg.h"//�����Ʋ���
#define CARDTESTMODE 1 //1.���������Ϣ 0.�����������Ϣ
#define CARDWIDTH 200  //��ɫ�����
#define CARDHEIGHT 350 //��ɫ���߶�
//#define white_limitMAX 300000 //��ɫ�����ֵ
//#define white_limitMIN 200000 //��ɫ��С��ֵ
#define MP_DX 1300                                     //ͨ��������ε�����ֵ
uint8_t K_A[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Ĭ��A��Կ
uint8_t K_B[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Ĭ��B��Կ
static uint8_t buffer_data[99];                               //RC_check_read���ݻ�����
static uint8_t bufferCard[20][99];                            //�����Ƭ��Ż�����
void RC_Clean_buffer(void);
void RC_Clean_bufferN(void);
uint8_t TIM_PIDTrackFlag = 0;
/**
 * @description: ��ַ����
 * @param {uint8_t} S ����
 * @param {uint8_t} Block �� ��0=��һ�����ݿ�
 * @return {*}
 */
uint8_t RC_Get_address(uint8_t S, uint8_t Block)
{
    //return S * 4 + Block-1;
    return S * 4 + Block;
}

/**
 * @description: ���RFID��
 * @param {*} 
 * @return {*}0.ʧ�� 1.�ɹ�
 */
uint8_t RC_check(void)
{
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
    {
        return 1;
    }
    return 0;
}

/**
 * @description: ���Ѽ��Ŀ�Ƭ���ݶ�ȡ 
 * @param {uint8_t} Block_address
 * @param {uint8_t} *KEY
 * @return {*}1.��ȡ�ɹ� 0.��ȡʧ��
 */
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY)
{
#if 0
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
    {
        if (PcdAnticoll(SN) == MI_OK) // ȡ��Ƭ���к�
        {
            if (PcdSelect(SN) == MI_OK) //ѡ���˿�
            {

                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //��֤��Կ
                {
                    if (PcdRead(Block_address, buffer_data) == MI_OK) //����
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
#else

    if (PcdAnticoll(SN) == MI_OK) // ȡ��Ƭ���к�
    {
        if (PcdSelect(SN) == MI_OK) //ѡ���˿�
        {

            if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //��֤��Կ
            {
                if (PcdRead(Block_address, buffer_data) == MI_OK) //����
                {
                    return 1;
                }
            }
        }
    }

    return 0;
#endif
}
/**
 * @description: ��ȡRC_check_read()�����ݻ���
 * @param {*}
 * @return {*}
 */
uint8_t *RC_Get_buffer()
{
    return buffer_data;
}

/**
 * @description: ��ȡ���鿨��������ݻ���
 * @param {*}
 * @return {*}
 */
uint8_t *RC_Get_bufferN(uint8_t position)
{
    return bufferCard[position];
}

/**
 * @description: �������ݻ���
 * @param {*}
 * @return {*}
 */
void *RC_Set_bufferN(uint8_t position,char *data)
{
    sprintf(bufferCard[position], "%s", data); //����
}

/**
 * @description: ������鿨�Ļ�������
 * @return {*}
 */
void RC_Clean_bufferN(void)
{
    for (int i = 0; i < 20; i++)
    {
        sprintf((char *)bufferCard[i], "%s", (char *)0);
    }
}
void RC_Clean_buffer(void)
{
    for (int i = 0; i < 99; i++)
    {
        buffer_data[i] = 0;
    }
}
/**
 * @description: ��ȡRFID�� Ѱ������ſɶ�ȡ����
 * ����=(int)(Block_address/4)
 * ������������=����*4+3
 * @param {uint8_t} Block_address ���ַ
 * @param {uint8_t} *KEY ��Կ������Կ��Ϊ""
 * @return {*} �ɹ���������
 */
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY)
{
    static uint8_t _buf[16];
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
    {
        if (PcdAnticoll(SN) == MI_OK) // ȡ��Ƭ���к�
        {
            if (PcdSelect(SN) == MI_OK) //ѡ���˿�
            {
                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //��֤��Կ
                {
                    if (PcdRead(Block_address, _buf) == MI_OK) //����
                    {
                        return _buf;
                    }
                }
            }
        }
    }
    return '\0';
}

/**
 * @description: RFIDд��
 * @param {uint8_t} Block_address�����ַ
 * @param {uint8_t} *KEY����Կ������Կ��Ϊ""
 * @param {uint8_t} *data��д������ֵ
 * @return {*}
 */
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data)
{
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
    {
        if (PcdAnticoll(SN) == MI_OK) // ����ײ�ɹ�
        {
            if (PcdSelect(SN) == MI_OK) //ѡ���˿�
            {
                if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY, SN) == MI_OK) //��֤����0����ԿA
                {
                    if (PcdWrite(Block_address, data) == MI_OK) //���� ���ַ1
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/**
 * @description: ��⵽����ʹ�� ��ȡ�����RC_Get_buffer()��ȡ
 * @param {uint8_t} *KEY RFID����Կ ""ΪĬ��
 * @return {*}0.Ѱ����Ƭ�˳�Ѱ�� 1.����ָ������ֵ�˳�Ѱ�� 2.�ۼƶ�ȡ3��ʧ��
 */
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp)
{
    uint8_t break_flag = 0, r = 0, count = 0;
    int _mp = 0;
    MasterCar_Stop();
    TIM_Cmd(TIM5, DISABLE); //��ֹTIM5�ж�δ��������

    while (r == 0)
    {
        count++; //ͳ�ƶ�ȡ����
        r = RC_check_read(Block_address, KEY);
        //Send_Debug_string("r1=");
        //Send_Debug_num2(r);
        if (!r)
        {
            if (RC_check())
            {
                r = RC_check_read(Block_address, K_A);
                //Send_Debug_num2(1);
                //Send_Debug_string("r2=");
                //Send_Debug_num2(r);
            }
        }else
        {
            break;
        }
        MasterCar_Go(30);
        if (count >= 5)
        {
            break_flag = 2;
            break;
        }
        delay_ms(150);
        Send_UpMotor(0, 0);
        delay_ms(200);
        

    } //�ȴ�Ѱ�����

    MasterCar_Stop();
    return break_flag;
}

/**
 * @description: ��ָ�������ڲ⿨
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @return {*}1.��⵽�� 0.δ��⵽
 */
uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //Ŀ������ֵ
    while (1)
    {
        r = PID_Track3(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99)        //δ�ﵽ����LED��ȫ�� ���⵽��1
        {
            card_flag = 1; //��⵽����־
            //Send_UpMotor(0, 0);
            break;
        }
        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            card_flag = 0; //��������ֵδ��⵽����־
            //Send_UpMotor(0, 0);
            break;
        }
        delay_ms(1);
    }

    PID_Set_recovery(); //�ָ�PID_Setǰ����
    return card_flag;
}

/**
 * @description: ��ָ�������ڲ⿨ PID���ΰ�
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @return {*}1.��⵽�� 0.δ��⵽
 */
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance, float p, float i, float d)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    PID_Set(p, i, d);
    dt += CanHost_Mp; //Ŀ������ֵ

    while (1)
    {
        r = PID_Track3(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99)        //δ�ﵽ����LED��ȫ�� ���⵽��1
        {
            card_flag = 1; //��⵽����־
            //Send_UpMotor(0, 0);
            break;
        }
        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            card_flag = 0; //��������ֵδ��⵽����־
            //Send_UpMotor(0, 0);
            break;
        }
        delay_ms(1);
    }

    PID_Set_recovery(); //�ָ�PID_Setǰ����
    return card_flag;
}

/**
 * @description: ��ָ�������ڶ��� ���۶����ɹ���ʧ�ܶ���ʻָ������
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @return {*}1.�����ɹ� 0.ʧ��  RC_Get_buffer();��ȡ��Ƭ����
 */
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //Ŀ������ֵ
    while (1)
    {
        r = PID_Track4(36);            //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99 && card_flag != 1) //δ�ﵽ����LED��ȫ������δ�����ɹ�״̬
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //�������ɹ�����1
            {
                card_flag = 1; //�����ɹ���־
            }
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            Send_UpMotor(0, 0); //ͣ��
            break;
        }
        delay_ms(1);
    }
    PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: ��ָ�������ڶ�N�ſ� ���۶����ɹ���ʧ�ܶ���ʻָ������
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @return {*} 0.ʧ�� >0�򷵻ض����ɹ��ĸ���  RC_Get_bufferN();��ȡ��Ƭ����
 */
uint8_t RC_Card_checkRangeReadN(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //Ŀ������ֵ
    while (1)
    {
        r = PID_Track4(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99)        //δ�ﵽ����LED��ȫ������δ�����ɹ�״̬
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //�������ɹ�����1
            {
                sprintf(bufferCard[card_flag], "%s", RC_Get_buffer());
                card_flag++; //�����ɹ���־
            }
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            Send_UpMotor(0, 0); //ͣ��
            break;
        }
        delay_ms(1);
    }
    PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: ��ָ�������ڶ��� ��RC_Card_checkRangeRead();�����ϣ����ĩβ��������
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @return {*}1.�����ɹ� 0.ʧ��
 */
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;      //���������־��ѭ����������

    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //Ŀ������ֵ
    while (1)
    {
        r = PID_Track4(36);            //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99 && card_flag != 1) //δ�ﵽ����LED��ȫ������δ�����ɹ�״̬
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //�������ɹ�����1
            {
                card_flag = 1; //�����ɹ���־
            }
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            Send_UpMotor(0, 0); //ͣ��
            break;
        }
        delay_ms(1);
    }

    if (CanHost_Mp > dt) //�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed, CanHost_Mp - dt);
    }
    else if (CanHost_Mp < dt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed, dt - CanHost_Mp);
    }

    PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: ��ָ�������ڶ��� ���۶����ɹ���ʧ�ܶ���ʻָ������ ��ѭ���ư�
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @param {uint8_t} n 0.�����ƶ�ȡ  >=1��ȡn��
 * @return {*}1.�����ɹ� 0.ʧ�� �ɹ���ȡ���ŵ����ݴ�RC_Get_bufferN();
 */

uint8_t RC_Card_checkRangeReadPlus(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //��ʻ����

    uint8_t card_flag = 0;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //Ŀ������ֵ CanHost_Mp������Χ�为��

    TIM_PIDTrack_Set(2);         //����ѭ���߳�

    while (1)
    {
        if (((card_flag < n) || n == 0) && RC_check()) //��⿨�ɹ���δ��ȡ
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //ֹͣ
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //����
                card_flag++;
            }

            MasterCar_GoMP(30, 200); //���۶�ȡ�Ƿ�ɹ�����ǰ200��ֹ�ظ���ȡͬһ�ſ�
            TIM_PIDTrack_Set(2); //����ǰ��
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //�ر�ѭ���߳�

    if (CanHost_Mp > dt) //�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed, CanHost_Mp - dt);
    }
    else if (CanHost_Mp < dt)
    {
        MasterCar_GoMP(MasterCar_GoSpeed, dt - CanHost_Mp);
    }
    //PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}


/**
 * @description: ��ָ�������ڶ��� ���۶����ɹ���ʧ�ܶ���ʻָ������ ѭ���ư�
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @param {uint8_t} n 0.�����ƶ�ȡ  >=1��ȡn��
 * @param {uint8_t} startAdd ������ݵ���ʼ��ַ 
 * @return {*}1.�����ɹ� 0.ʧ�� �ɹ���ȡ���ŵ����ݴ�RC_Get_bufferN();
 */
#define Plus_Track 1 //0.��ѭ��

uint8_t RC_Card_checkRangeReadPlus2(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //��ʻ����

    uint8_t card_flag = 0;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //Ŀ������ֵ CanHost_Mp������Χ�为��
    TIM_PIDTrack_Set(1); //����ѭ���߳�
    while (1)
    {
        if (((card_flag < n) || n == 0) && RC_check()) //��⿨�ɹ���δ��ȡ
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //ֹͣ
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //����
                card_flag++;
            }
            
            MasterCar_GoMP(30, 200); //���۶�ȡ�Ƿ�ɹ�����ǰ200��ֹ�ظ���ȡͬһ�ſ�
            TIM_PIDTrack_Set(1); //����ѭ��ǰ��
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //�ر�ѭ���߳�

    if (CanHost_Mp > dt) //�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed, CanHost_Mp - dt);
    }
    else if (CanHost_Mp < dt)
    {
        MasterCar_GoMP(MasterCar_GoSpeed, dt - CanHost_Mp);
    }
    //PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}


/**
 * @description: ��ָ�������ڶ��� ���۶����ɹ���ʧ�ܶ���ʻָ������ ѭ���ư�3 ����ʧ���ٴζ�ȡ ֻ��ȡһ�ſ�
 * @param {uint16_t} init_distance  ��ʼ���� mp
 * @param {uint16_t} distance ��Χ���� mp
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @param {uint8_t} n �ٴζ�ȡ����
 * @param {uint8_t} startAdd ������ݵ���ʼ��ַ 
 * @return {*}1.�����ɹ� 0.ʧ�� �ɹ���ȡ���ŵ����ݴ�RC_Get_bufferN();
 */

uint8_t RC_Card_checkRangeReadPlus3(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //��ʻ����

    uint8_t card_flag = 0,back_flag = n;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //Ŀ������ֵ CanHost_Mp������Χ�为��
    TIM_PIDTrack_Set(1); //����ѭ���߳�
    while (1)
    {
        if (((card_flag < 1)) && RC_check()) //��⿨�ɹ���δ��ȡ
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //ֹͣǰ��
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //����
                card_flag++;
                
                MasterCar_GoMP(30, 200); //��ȡ�ɹ�����ǰ200��ֹ�ظ���ȡͬһ�ſ�
            }else
            {
                if(back_flag--)
                {
                    TIM_PIDTrack_Set(0); //ֹͣǰ��
                    MasterCar_BackMP(30,200);
                }
            }
            
            TIM_PIDTrack_Set(1); //����ѭ��ǰ��
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //�ر�ѭ���߳�

    if (CanHost_Mp > dt) //�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed, CanHost_Mp - dt);
    }
    else if (CanHost_Mp < dt)
    {
        MasterCar_GoMP(MasterCar_GoSpeed, dt - CanHost_Mp);
    }
    //PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: ����ѭ���߳�
 * @param {uint8_t} flag 0.ֹͣѭ��
 * @return {*}
 */
void TIM_PIDTrack_Set(uint8_t flag)
{
    TIM_PIDTrackFlag = flag;
    TIM_Cmd(TIM8, (flag > 0) ? 1 : 0);
    MasterCar_Stop();
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM8, TIM_IT_Update) == SET)
    {
        switch (TIM_PIDTrackFlag)
        {
        case 1: //ѭ����ǰ
            PID_TrackMP(30);
            break;
        case 2: //��ǰ
            MasterCar_Go(30);
            break;
        default:
            break;
        }
    }
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
}
