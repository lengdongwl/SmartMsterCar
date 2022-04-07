/*
 * @Description: ����RC522���� RFIDCard�Ĳ���
 * @Autor: 309
 * @Date: 2021-10-11 10:56:06
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-07 14:52:45
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
#define CARDTESTMODE 1 //1.���������Ϣ 0.�����������Ϣ
#define CARDWIDTH 200 //��ɫ�����
#define CARDHEIGHT 350 //��ɫ���߶�
//#define white_limitMAX 300000 //��ɫ�����ֵ
//#define white_limitMIN 200000 //��ɫ��С��ֵ
#define MP_DX 1300 //ͨ��������ε�����ֵ 
uint8_t K_A[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Ĭ��A��Կ
uint8_t K_B[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Ĭ��B��Կ
uint8_t buffer_data[99];                                  //RC_check_read���ݻ�����
uint8_t bufferCard[20][99];                                  //�����Ƭ��Ż�����
void RC_Clean_buffer(void);
void RC_Clean_bufferN(void);

/**
 * @description: ��ַ����
 * @param {uint8_t} S ����
 * @param {uint8_t} Block ��
 * @return {*}
 */
uint8_t RC_Get_address(uint8_t S, uint8_t Block)
{
    return S * 4 + Block-1;
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
 * @description: ��⿨Ƭ����ȡ���� 
 * @param {uint8_t} Block_address
 * @param {uint8_t} *KEY
 * @return {*}1.��ȡ�ɹ� 0.��ȡʧ��
 */
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY)
{
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
 * @description: ������鿨�Ļ�������
 * @return {*}
 */
void RC_Clean_bufferN(void)
{
    for (int i = 0; i < 20; i++)
    {
        sprintf((char*)bufferCard[i],"%s",(char*)0);
    }
    
}
void RC_Clean_buffer(void)
{
    for (int i = 0; i < 99; i++)
    {
        buffer_data[i]=0;
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
    uint8_t *_buf;
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //Ѱ��
    {
        if (PcdAnticoll(SN) == MI_OK) // ȡ��Ƭ���к�
        {
            if (PcdSelect(SN) == MI_OK) //ѡ���˿�
            {           
                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //��֤��Կ
                {
                    if (PcdRead(Block_address, READ_RFID) == MI_OK) //����
                    {
                        _buf = READ_RFID;
                    }
                }
            }
        }
    }
    return _buf;
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
 * @description: ����ѭ��Ѱ�� ��ȡ�����RC_Get_buffer()��ȡ
 * @param {uint8_t} *KEY RFID����Կ ""ΪĬ��
 * @return {*}1.����ָ������ֵ�˳�Ѱ�� 0.Ѱ����Ƭ�˳�Ѱ��
 */
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp)
{
    uint8_t break_flag = 0, r = 0;
    int _mp = 0;
    Roadway_mp_syn();
    _mp = Roadway_mp_Get();
    TIM_Cmd(TIM5, DISABLE); //��ֹTIM5�ж�δ��������
    PID_reset();
    PID_Set(1, 0.1, 0);
    while (r == 0)
    {
        r = RC_check_read(Block_address, KEY);
        _mp = Roadway_mp_Get();
        PID_TrackMP(26); //ѭ�� ���Ժڰ���

        if (_mp >= mp)
        {
            break_flag = 1;
            break;
        }
        /*
        �ο�
        delay_ms(100);
		Send_UpMotor(0, 0);
        delay_ms(100);
        */
        delay_ms(150);
        Send_UpMotor(0, 0);
        delay_ms(50);
    }                   //�ȴ�Ѱ�����
                        //Send_Debug_string(RC_Get_buffer());
    PID_Set_recovery(); //�ָ�PID_Setǰ����
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
    dt+=CanHost_Mp;//Ŀ������ֵ
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
    dt+=CanHost_Mp;//Ŀ������ֵ

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
 * @description: ���˼�⿨�����(���м�Ϊ�����ߵ���) ��⵽�������ؽ��
 * |------card or DX------|
 * @param {uint16_t} init_distance ��ʼ����
 * @param {uint16_t} distance �����ܾ���
 * @return {*}
 */
uint8_t RC_Card_checkCardOrDX(int16_t init_distance, uint16_t distance)
{
    int dt=0,r=0,card_flag=0;
    int position_DX = distance*3/8+CanHost_Mp-init_distance;;
    PID_Set(25, 0, 300);
    dt+=distance/2+CanHost_Mp-init_distance;//Ŀ������ֵ
    while (1)
    {
        r = PID_Track3(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99)        //δ�ﵽ����LED��ȫ�� ���⵽��1
        {
            if(CanHost_Mp<=position_DX)card_flag = 1; //��⵽���α�־
            else card_flag = 2;//��⵽����־
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
 * @return {*}1.�����ɹ� 0.ʧ��
 */
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //���ñ������̼��ٽ������� �����ϴ�
    //PID_Set(50, 0, 1 2000);      //���ñ������̼��ٽ������� �����λ����ȶ�
    PID_Set(25, 0, 300);
    dt+=CanHost_Mp;//Ŀ������ֵ
    while (1)
    {
        r = PID_Track3(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99 && card_flag!=1)        //δ�ﵽ����LED��ȫ������δ�����ɹ�״̬
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
            if(MasterCar_findCar3(Block_address,KEY,80)==0)//�������ɹ�����1
            {
                card_flag = 1; //�����ɹ���־
            }
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            Send_UpMotor(0,0);//ͣ��
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
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt = distance - init_distance; //��ʻ����
    uint8_t card_flag = 0, r = 0 ;//���������־��ѭ����������

    PID_Set(25, 0, 300);
    dt+=CanHost_Mp;//Ŀ������ֵ
    while (1)
    {
        r = PID_Track3(36); //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99 && card_flag!=1)        //δ�ﵽ����LED��ȫ������δ�����ɹ�״̬
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
            if(MasterCar_findCar3(Block_address,KEY,80)==0)//�������ɹ�����1
            {
                card_flag = 1; //�����ɹ���־
            }
        }

        if (CanHost_Mp >= dt) //�ﵽ�����Ƴ�Ѱ��
        {
            Send_UpMotor(0,0);//ͣ��
            break;
        }
        delay_ms(1);
    }
    
    if(CanHost_Mp>dt)//�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed,CanHost_Mp-dt);
    }else if(CanHost_Mp<dt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,dt-CanHost_Mp);
    }

    PID_Set_recovery();
    MasterCar_Stop(); 
    return card_flag;
}


/**
 * @description: ���ζ�˫�����ж϶�����λ����Ϣ  
 * @param {uint16_t} init_distance ��ʼ������Ϣ
 * @param {uint16_t} distance   ���� (�ο�:�̵�1800������2250)
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ
 * @return {*}1.B2�ɹ� 2.C3��ȡ�ɹ�
 *       �м��
 *         |
 * A1      B2      C3
 * |---|---|---|---|
 *     ^   k   ^   k
 *     r       r
 * 
���>>>B2>>>>>>C3>>>
 */
uint8_t RC_Card_oneFunc2(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int frist_distance=distance/2;//��һ�μ��ľ���
    int second_distance=distance/4;//�ڶ��μ��ľ���
    //int dt=distance+CanHost_Mp;
    PID_Set(20,0,20);

    MasterCar_SmartRunMP(MasterCar_GoSpeed,second_distance-init_distance);//��ʻ����һ�ο�ʼ����λ��
    if(RC_Card_checkRangePID(0,frist_distance,20,0,20))//��һ�β⵽��
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
        if(MasterCar_findCar3(Block_address,KEY,80)==0)//�������ɹ�����1
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,second_distance);//����ȡ�ɹ�����ʻ�����������
            return 1;
        }

    }else if(RC_Card_checkRangePID(0,second_distance,20,0,20))
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
        if(MasterCar_findCar3(Block_address,KEY,80)==0)
        {
            return 2;
        }
    }
    return 0;

}



/**
 * @description: ���ζ�����ѭ����ǰ�����ҷֱ�P1.P2
 * @param {uint16_t} distance       ��ʻ����(�ο�:�̵�1800������2250)
 * @param {uint8_t} *Block_address1 ���ַP1
 * @param {uint8_t} *Block_address2 ���ַP2
 * @return {*}
 * 
 * >>>>>>>>>>��x
 * (�����жϸÿ��ַ1�Ƿ���ȷ ��������� ������ȡ���ַ2�ٴζ�ȡ)
 */
uint8_t RC_Card_final_P1P2(int16_t init_distance,uint16_t distance,uint8_t Block_address1,uint8_t Block_address2)
{
    int dt = distance; //��ʾ���վ���
    uint8_t car_exit=0; //����Ƿ�����ɹ�
    dt+=CanHost_Mp-init_distance;//Ŀ������ֵ
    
    if(RC_Card_checkRangePID(0,distance,25,0,300))
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
        if (MasterCar_findCar3(Block_address1, K_A, 80) == 0)
        {
            if(RC_Get_buffer()!="")car_exit+=1;//��P1�ɹ���־
        }
    }
    if(car_exit!=1)//����P2ʧ�������ΪP2��ַ
    {
        MasterCar_BackMP(MasterCar_GoSpeed,500);
        if(RC_Card_checkRangeReadBack(0,dt-CanHost_Mp,Block_address2,K_A))
        {
            if(RC_Get_buffer()!="")car_exit+=1;//��P1�ɹ���־
        }
    }
    if(CanHost_Mp>dt)//�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed,CanHost_Mp-dt);
    }else if(CanHost_Mp<dt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,dt-CanHost_Mp);
    }
    PID_Set_recovery();
    MasterCar_Stop();
#if CARDTESTMODE==1
    if(car_exit!=0)
    {
        if(car_exit==1)
        {
            Send_Debug_string("card.1 ok");
        }else
        {
            Send_Debug_string("card.2 ok");
        }
    }else
    {
        Send_Debug_string("card.1,2 error");

    }

#endif    
    return car_exit;  
}

/**
 * @description: ѭ����ǰ�����ҷֱ�P1.P2
 * @param {uint16_t} distance       ��ʻ����(�ο�:�̵�1800������2250)
 * @param {uint8_t} *Block_address1 ���ַP1
 * @param {uint8_t} *Block_address2 ���ַP2
 * @param {uint8_t} *KEY1           ��1�ζ�ȡ����KEY
 * @param {uint8_t} *KEY2           ��2�ζ�ȡ����KEY
 * @return {*}
 * 
 * >>>>>>>>>>��x
 * (�����жϸÿ��ַ1�Ƿ���ȷ ��������� ������ȡ���ַ2�ٴζ�ȡ)
 */
uint8_t RC_Card_final_P1P2_2(uint16_t distance,uint8_t Block_address1,uint8_t Block_address2,uint8_t *KEY1,uint8_t *KEY2)
{
    //��ͬKEY��ͬ��ַ�汾����
}


/*
2020����ʡ��
  λ��0     1       2
  r1       r2       r3
  |        |        |
  |--------|--------|
ʮ��·��--�м��---ʮ��·��
*/
/**
 * @description: ���ζ����ο�����ͬλ�õĿ���ַ��ͬ
 * ע�������ڴ�Ѱ�����������и��ŵĵ���
 * @param {uint16_t} distance   ���ξ���(�ο�:�̵�1800������2250)
 * @param {uint8_t} *Block_address ���ַ����
 * @param {uint8_t} *KEY  ����KEY��ַ����һ��
 * @return {*}
 */
type_CardRBuf RC_Card_oneFunc1(uint16_t distance,uint8_t *Block_address,uint8_t *KEY)
{
    int dt = distance; //��ʾ���վ���
    int  position = 0, r = 0;
    type_CardRBuf  type_cardRBuf;
    int init_mp=CanHost_Mp;
    dt+=CanHost_Mp;//Ŀ������ֵ
    RC_Clean_bufferN();
    type_cardRBuf.success_count=0;

    //CanHost_Mp
    PID_Set(20, 0, 20);

    while (CanHost_Mp<dt)
    {
        r = PID_Track3(36);     //ȫ�𷵻�99 �����ȫ����������ֱ��
        if(CanHost_Mp>=init_mp+distance/5)//����1/5����ڶ��ſ�
        {
            position=1;
        }
        if(CanHost_Mp>=init_mp+(distance/2))//����1/2��������ſ�
        {
            position=2;
        }
        if (r == 99 && type_cardRBuf.success_position[2] != 1) //LED��ȫ�� ��ʼ���� �ҵ����ſ�δ��
        {
            MasterCar_SmartRunMP(36, 260); //��ǰ��ʻ260mp�ڶ���
            if (MasterCar_findCar3(Block_address[position], KEY, 80) == 0)
            {
                sprintf((char*)bufferCard[position],"%s",(char*)RC_Get_buffer());//�������ݵ����鿨��ŵĻ�����
                type_cardRBuf.success_position[position] = 1; //�����ɹ���־
                type_cardRBuf.success_count+=1;//ͳ�ƶ����ɹ�������
                if(type_cardRBuf.success_position[2] == 1)
                {
                    
                }
            }
            else
            {     
                type_cardRBuf.success_position[position] = 0; //����ʧ�ܱ�־
            }
            type_cardRBuf.total_count+=1;//ͳ�Ƽ�⵽���ߵĴ���
        }
        if ( dt<=CanHost_Mp)
        {
            break;
        }
       delay_ms(1);
    }
    if(CanHost_Mp>dt)//�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed,CanHost_Mp-dt);
    }
    else if(CanHost_Mp<dt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,dt-CanHost_Mp);
    }
    PID_Set_recovery();
    MasterCar_Stop();
#if CARDTESTMODE==1
    for (int i = 0; i < 3; i++)
    {
        Send_Debug_string("card.");
        Send_Debug_num(i);
        if(type_cardRBuf.success_position[i] == 1)
        {
            Send_Debug_string2(" ok");
        }else
        {
            
            Send_Debug_string2(" error");
        }
    }
#endif   
    
    return type_cardRBuf;

/*
��
    uint8_t Block_address[]={4,4,4};//��1����2��������ȡ��ַ
	uint8_t key[]={0x00,0x04,0x00,0x02,0x05,0x00};//KEY
	type_cardRBuf=RC_Card_oneFunc1(2250,Block_address,key);
	if(type_cardRBuf.success_count!=0)//�ж��Ƿ��������0��
	{
        if(type_cardRBuf.success_position[0]!=0)
        {
            //�ڵ�һ��λ�ö�����
        }
		else if(type_cardRBuf.success_position[1]!=0)
		{
            //�ڵڶ���λ�ö�����
			Send_Debug_string(RC_Get_bufferN(1));
		}else if(type_cardRBuf.success_position[2]!=0)
		{
            //�ڵ�����λ�ö�����
			Send_Debug_string(RC_Get_bufferN(2));
		}else
		{
            //��ȡʧ��
		}
	}
*/
}


/*
                      k?       k?
                      |        |            
-------------·��---------------|
               |
            �м�·��
*/
/**
 * @description: ��˫�Σ������ζ����ο�
 * @param {uint16_t} distance   �ܾ���(�ο�:�̵�1800������2250)
 * @param {uint8_t} *Block_address ���ַΪ�����
 * @param {uint8_t} *KEY  ��Կ
 * @return {*}
 */
type_CardRBuf RC_Card_oneFunc3(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt=0,frist_checkMP=0;
    type_CardRBuf  type_cardRBuf;
    dt=distance+CanHost_Mp-init_distance;//�ܾ���
    frist_checkMP=distance*3/4-init_distance+CanHost_Mp;
    RC_Clean_bufferN();//������濨��
    type_cardRBuf.success_count=0;
    
    MasterCar_SmartRunMP(MasterCar_GoSpeed,(int)(distance/4)-init_distance);
    OFlag_DX_carGo(MasterCar_GoSpeed,MP_DX);//���������
    if(RC_Card_checkRangeRead(CanHost_Mp,frist_checkMP,Block_address,KEY))
    {
        sprintf((char*)bufferCard[0],"%s",(char*)RC_Get_buffer());//�������ݵ����鿨��ŵĻ�����
        type_cardRBuf.success_position[0] = 1; //����1�ɹ���־
        type_cardRBuf.success_count=1;//ͳ�ƶ����ɹ�������
    }
    if(RC_Card_checkRangeReadBack(CanHost_Mp,dt,Block_address,KEY))
    {
        sprintf((char*)bufferCard[1],"%s",(char*)RC_Get_buffer());//�������ݵ����鿨��ŵĻ�����
        type_cardRBuf.success_position[1] = 1; //����1�ɹ���־
        type_cardRBuf.success_count+=1;//ͳ�ƶ����ɹ�������
    }
    MasterCar_Stop();
#if CARDTESTMODE==1
    for (int i = 0; i < 2; i++)
    {
        Send_Debug_string("card.");
        Send_Debug_num(i);
        if(type_cardRBuf.success_position[i] == 1)
        {
            Send_Debug_string2(" ok");
        }else
        {
            
            Send_Debug_string2(" error");
        }
    }
#endif
    
    return type_cardRBuf;
}




/**
 * @description: ͨ����ɫ��ֵ�ж��Ƿ�Ϊ�׿�
 * @param {*}
 * @return {*}
 */
/*
uint8_t RC_Card_checkCard(void)
{
    int flag=0,k=0;
    Send_UpMotor(36,36);
    while (1)
    {

        if(Get_Host_UpTrack(TRACK_H8)==0xff)//�����ɫ����
        {
            k++;
            flag=1;
        }
        if(flag==1)
        {
            if(Get_Host_UpTrack(TRACK_H8)!=0xff)
            {
                Send_UpMotor(0,0);
                break;
            }
        }
    }
    Send_Debug_num(k);
    Send_Debug_string("\n");
    if(white_limitMIN<k && k<white_limitMAX)
    {
        return 1;
    }else
    
    return 0;

}
*/


/*
  ------�����п�----                            ------�����п�----
  |                |                           |                |
  |----------------|-------------·��-----------|----------------|
                                  |
                               �м�·��
*/
/**
 * @description: ˫�μ���м�·�ϣ�������
 * @param {uint16_t} distance ˫���ܾ��� (�ο�:�̵�1800*2������2250*2)
 * @param {uint8_t} Block_address ���ַ
 * @param {uint8_t} *KEY ��Կ
 * @return {*}
 */
type_CardRBuf RC_Card_twoFunc1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int frist_checkMP=0,dt=0;
    type_CardRBuf  type_cardRBuf;

    frist_checkMP=(int)(distance/4)+250; //��ʼ�㵽1/4����Ϊ��һ�ſ��������
    dt=distance+CanHost_Mp-init_distance;//�ܾ���
    RC_Clean_bufferN();//������濨��
    type_cardRBuf.success_count=0;

    if(RC_Card_checkRangeRead(init_distance,frist_checkMP,Block_address,KEY))
    {
        sprintf((char*)bufferCard[0],"%s",(char*)RC_Get_buffer());//�������ݵ����鿨��ŵĻ�����
        type_cardRBuf.success_position[0] = 1; //����1�ɹ���־
        type_cardRBuf.success_count=1;//ͳ�ƶ����ɹ�������
    }
    OFlag_DX_carGo(MasterCar_GoSpeed,MP_DX);//���������
    if(RC_Card_checkRangeReadBack(0,dt-CanHost_Mp,Block_address,KEY))
    {
        sprintf((char*)bufferCard[1],"%s",(char*)RC_Get_buffer());//�������ݵ����鿨��ŵĻ�����
        type_cardRBuf.success_position[1] = 1; //����1�ɹ���־
        type_cardRBuf.success_count+=1;//ͳ�ƶ����ɹ�������
    }
    MasterCar_Stop();
    
#if CARDTESTMODE==1
    for (int i = 0; i < 2; i++)
    {
        Send_Debug_string("card.");
        Send_Debug_num(i);
        if(type_cardRBuf.success_position[i] == 1)
        {
            Send_Debug_string2(" ok");
        }else
        {
            
            Send_Debug_string2(" error");
        }
    }
#endif
    return type_cardRBuf;

/*
��
    type_CardRBuf type_cardRBuf;
	type_cardRBuf=RC_Card_twoFunc1(4500,1,K_A);
	if(type_cardRBuf.success_count>0)
	{
		if(type_cardRBuf.success_position[0]==1)
		{
			Send_Debug_string(RC_Get_bufferN(0));
		}else
		{
			Send_Debug_string(RC_Get_bufferN(1));
		}
	}
*/        
}



/**
 * @description: ��ָ�����̷�Χ��N�ſ� 
 * @param {uint16_t} distance   ����
 * @param {uint8_t}  Block_address ���ݿ��ַ
 * @param {uint8_t} *KEY RFID����Կ ""ΪĬ��
 * @param {uint8_t} count ��ȡ���ɹ����� �����ɹ���������ѭ������һ�ɲ�����
 * @return {*}
 */
type_CardRBuf RC_Card_ALLFun1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY,uint8_t count)
{
    int dt = distance; //��ʾ���վ���
    int  i = 0, r = 0;
    type_CardRBuf  type_cardRBuf;
    type_cardRBuf.total_count=0;//��¼��ȡ����
    type_cardRBuf.success_count=0;//��¼�ɹ�����
    RC_Clean_bufferN();//�������
    dt+=CanHost_Mp-init_distance;//Ŀ������ֵ
    PID_Set(25, 0, 300);
    //PID_Set(10, 0, 0);

    while (CanHost_Mp<dt)
    {
        r = PID_Track3(36);     //ȫ�𷵻�99 �����ȫ����������ֱ��
        if (r == 99 && type_cardRBuf.success_count<count) //LED��ȫ�� ��ʼ����
        {
            MasterCar_SmartRunMP(36, 250); //��ǰ��ʻ250mp�ڶ���
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                if(RC_Get_buffer()!="")//��ʹ�����ɹ�����Ϊ�� Ҳ�϶�ʧ��
                {
                    sprintf((char*)bufferCard[type_cardRBuf.success_count],"%s",(char*)RC_Get_buffer());
                    type_cardRBuf.success_position[type_cardRBuf.success_count]=1;
                    type_cardRBuf.success_count+=1;
                }
            }
             type_cardRBuf.total_count+=1;   
        }
        if ( dt<=CanHost_Mp)
        {
            break;
        }
       delay_ms(1);
    }
    if(CanHost_Mp>dt)//�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed,CanHost_Mp-dt);
    }else if(CanHost_Mp<dt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,dt-CanHost_Mp);
    }
    PID_Set_recovery();
    MasterCar_Stop();
#if CARDTESTMODE==1
    if(type_cardRBuf.success_count>0)
    {
        for (int i = 0; i < type_cardRBuf.success_count; i++)
        {
            Send_Debug_string("card.");
            Send_Debug_num(i);
            if(type_cardRBuf.success_position[i] == 1)
            {
                Send_Debug_string2(" ok");
            }else
            {
                
                Send_Debug_string2(" error");
            }
        }
    }else
    {
        Send_Debug_string("card.ALL error");
    }
#endif    
    
    return type_cardRBuf;
}

/*                                ��
                                  or                            ֹͣ�� �����Ƿ��п���������
  |----------------|-------------·��-----------|----------------|
                                  |
                               �м�·��
*/
/**
 * @description: ���μ���м�λ��Ϊ����·�� 
 * @param {uint16_t} init_distance
 * @param {uint16_t} distance
 * @param {uint8_t} Block_address
 * @param {uint8_t} *KEY
 * @return {*}
 */
uint8_t RC_Card_oneFunc4(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt=0,finaldt=0,card_flag=0;
    int position_DX = distance*3/8+CanHost_Mp-init_distance;;
    PID_Set(25, 0, 300);
    dt+=distance/2+CanHost_Mp-init_distance;//1/2����
    finaldt=distance+CanHost_Mp-init_distance;//���վ���
    while (1)
    {

        if (PID_Track3(36) == 99) //δ�ﵽ����LED��ȫ�� ���⵽��1
        {
            if(CanHost_Mp<=position_DX)card_flag = 1; //��⵽���α�־
            else card_flag = 2;//��⵽����־
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
    if(card_flag==1)
    {
        MasterCar_GoMP(MasterCar_GoSpeed ,1300);
    }else if(card_flag==2)
    {
        MasterCar_SmartRunMP(36, 250); //��ǰ��ʻ250mp�ڶ���
        if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
        {
            if(RC_Get_buffer()!="")//��ʹ�����ɹ�����Ϊ�� Ҳ�϶�ʧ��
            {
                card_flag=2;
            }else
            {
                card_flag=0;
            }
        }
    }
    PID_Set_recovery(); //�ָ�PID_Setǰ����
     if(CanHost_Mp>finaldt)//�������
    {
        MasterCar_BackMP(MasterCar_GoSpeed,CanHost_Mp-finaldt);
    }else if(CanHost_Mp<finaldt)
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,finaldt-CanHost_Mp);
    }
    PID_Set_recovery();
    MasterCar_Stop();
#if CARDTESTMODE==1
    if(card_flag==1)
    {
        Send_Debug_string2("DX");
    }else if(card_flag==2)
    {
        Send_Debug_string2("Card.0 ok");
    }else
    {
        Send_Debug_string2("error");
    }
#endif    
    return card_flag;
}