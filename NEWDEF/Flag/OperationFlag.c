/*
 * @Description: ��־��ٿ�
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-07 15:04:38
 * @version: 2021.05.20Э��
 */
#include "OperationFlag.H"
#include "delay.h"
#include "can_user.h"
#include "infrared.h"
#include "Function.h" //Ŀ�굵λ
#include "PID.h"
#include "MasterCarCotrol.h"
#include "MyString.h"
#include "string.h"
#define Agreement_mode_infrared 1
#define Agreement_mode_Zigbee 2

uint8_t DZ_flag = 0;            //��բ��־�￪��
uint8_t CK_flag = 0;            //�����־���½����
uint8_t DX_flag = 0;            //������η���
uint8_t YY_flag[3] = {0, 0, 0}; //������־������ʱ���ȡ
uint8_t OFlag_SLAVEflag = 0;    //�ȴ�����
uint8_t OFlag_ETCflag = 0;      //�ȴ�ETC
void Operation_Zigbee(void);
uint8_t OFlag_xx(uint8_t *cmd, uint8_t *flag, uint8_t time);
/*����ͨѶ*/
#if 0
uint8_t light_1[4] = {0x00, 0xff, 0x0c, 0xf3};              //����·�Ʊ�־��  ��Դ��λ+1
uint8_t light_2[4] = {0x00, 0xff, 0x18, 0xe7};              //����·�Ʊ�־��  ��Դ��λ+2
uint8_t light_3[4] = {0x00, 0xff, 0x5e, 0xa1};              //����·�Ʊ�־��  ��Դ��λ+3
#else
uint8_t light_1[4] = {0x00, 0xFF, 0x0C, ~(0x0C)}; // ����·�� ��Դ��λ��1
uint8_t light_2[4] = {0x00, 0xFF, 0x18, ~(0x18)}; // ����·�� ��Դ��λ��2
uint8_t light_3[4] = {0x00, 0xFF, 0x5E, ~(0x5E)}; // ����·�� ��Դ��λ��3
#endif

uint8_t alarm_open[6] = {0x03, 0x05, 0x14, 0x45, 0xDE, 0x92};  // ���̨(������) Ĭ�Ͽ�����
uint8_t alarm_close[6] = {0x67, 0x34, 0x78, 0xA2, 0xFD, 0x27}; // ���̨(������) �ر�
uint8_t lt_display[6] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};  // ������ʾ��־�� |֡ͷ|��ָ��|����1|����2|����3|����4|

/*ZigbeeͨѶЭ���ʽ
----------------------------------------------------------------
|֡ͷ|     01-17       | 0xXX | 0xXX | 0xXX  | 0xXX | 0xXX |֡β|
----------------------------------------------------------------
|0x55|Zigbee�ն˽ڵ���|��ָ��|��ָ��1|��ָ��2|��ָ��3|У���|0xbb|
----------------------------------------------------------------
У���=(��ָ��+��ָ��1+��ָ��2+��ָ��3)%256
*/
uint8_t alarm_change[8] = {0x55, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB}; //���̨(������)�޸Ŀ�����
uint8_t LED_cmd[8] = {0x55, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};      //LED��ʾ��־��
uint8_t DZ_cmd[8] = {0x55, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //��բ��־�����ָ����ش��ṹ
uint8_t WX_cmd[8] = {0x55, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //���߳���־��
uint8_t YY_cmd[8] = {0x55, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //����������־��
uint8_t TFT_cmd_A[8] = {0x55, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};    //����TFT��ʾ��־��A
uint8_t TFT_cmd_B[8] = {0x55, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};    //����TFT��ʾ��־��B
uint8_t JT_cmd_A[8] = {0x55, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //���ܽ�ͨ�Ʊ�־��A
uint8_t JT_cmd_B[8] = {0x55, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //���ܽ�ͨ�Ʊ�־��B
uint8_t CK_cmd_A[8] = {0x55, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //���峵���־��A
uint8_t CK_cmd_B[8] = {0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //���峵���־��B
uint8_t ETC_cmd[8] = {0x55, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};      //ETCϵͳ��־�������ش�
uint8_t DX_cmd[8] = {0x55, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //������α�־��
/*
******����ͨѶ********
1.����·�Ʊ�־��
2.��������־��
3.������ʾ����־�ָ���ο�ͨ��Э�飩
4.LCD��ʾ��־��
*****ZigbeeͨѶ*******
1.LED��ʾ��־��
2.��բ��־��
3.���߳���־��---
4.����������־�ָ���ο�ͨ��Э�飩
5.TFT��ʾ����־�ָ���ο�ͨ��Э�飩
6.���ܽ�ͨ�Ʊ�־��
7.���ƴӳ�
8.���峵���־��
9.ETCϵͳ��־λ����---
10.���̨�����޸Ŀ�����---
11.���μ���־��
12.����ƽ̨�Զ�����ϵͳ�ϴ��������---
*/

/**
 * @description: У��ͼ��� �ڱ�Э����У���=(��ָ��+��ָ��1+��ָ��2+��ָ��3)%256
 * @param {uint8_t} data1
 * @param {uint8_t} data2
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @return {*}
 */
uint8_t calc_CheckSum(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    return (data1 + data2 + data3 + data4) % 256;
}

/**
 * @description: ����Ƿ�ΪZigbee�ڵ�İ�
 * @param {uint8_t*} data
 * @return {*} ʧ�ܷ���0 �ɹ����ؽڵ���
 */
unsigned int OFlag_check(uint8_t *data)
{
    if (data[0] == 0x55 && data[7] == 0xbb) //֡ͷ ֡β
    {
        if (data[1]) //zigbee�ڵ��Ų�Ϊ0
        {
            return data[1];
        }
    }
    return 0;
}

/**
 * @description: Э��ѡ���ͷ�ʽ�������� 6λ����8λZigbee
 * @param {unsigned char} mode 2=Agreement_mode_infrared or 1=Agreement_mode_Zigbee
 * @param {uint8_t*} data �����͵�����
 * @return {*}
 */
void Agreement_Send(unsigned char mode, uint8_t *data)
{
    if (mode == Agreement_mode_infrared)
    {
        TaskBoard_RISend(data, 6);
    }
    if (mode == Agreement_mode_Zigbee)
    {
        /* Send_Debug_HEX(data[0]);
        Send_Debug_HEX(data[1]);
        Send_Debug_HEX(data[2]);
        Send_Debug_HEX(data[3]);
        Send_Debug_HEX(data[4]);
        Send_Debug_HEX(data[5]);
        Send_Debug_HEX(data[6]);
        Send_Debug_HEX(data[7]);
        Send_Debug_string("\r\n");*/
        Send_ZigbeeData_To_Fifo(data, 8);
    }
    //Send_ZigbeeData_To_Fifo(data, 8);
    //delay_ms(50);
}

/**
 * @description: ZigbeeͨѶ
 * @param {uint8_t} zigbee_num zigbee�ն˽ڵ��� ��ѯ��Ŷ��ձ�
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_Zigbee(uint8_t zigbee_num, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = 0x55;                                      //֡ͷ
    dt[1] = zigbee_num;                                //zigbee�ն˽ڵ���
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = 0xbb;                                      //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ����·�Ʊ�־�ﵵλ����(���ܵ����նȶ�ȡӰ��)
 * �ر�ע��:���ÿ���̫�� Ӱ�������
 * @param {unsigned int} gear ��λ
 * @return {*}��ʼ��λ
 */
unsigned int OFlag_light(unsigned int gear)
{
    int light_buf[4], i, j;
    int init_light = 0; //��ʼ��λ
    int k = 0;          //��ʼ��λ�±걣��
    int b = 0;
    /*ԭ��+1�� ��ȡ�ĸ���λ�Ĺ��նȴ�С ������ǿ���ŵ�λ*/
    Get_Bh_Value(); //�����һ�� ����Ӱ����
    delay_ms(500);
    for (i = 0; i < 4; i++) //��ȡ������λ��ֵ
    {

        light_buf[i] = TaskBoard_BH();
        delay_ms(1);
        if (i == 0)
        {
            init_light = light_buf[0]; //�����ʼ��λ����ֵ
        }
        Infrared_Send(light_1, 4); //��λ+1
        //Send_Debug_num(i);
        //Send_Debug_string("\n");
        delay_ms(500);
        delay_ms(500);
        delay_ms(500);
    }
    for (i = 0; i < 4; i++) //����λ
    {
        for (j = 0; j < 4 - i - 1; j++)
        {
            if (light_buf[j] > light_buf[j + 1])
            {
                b = light_buf[j];
                light_buf[j] = light_buf[j + 1];
                light_buf[j + 1] = b;
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        if (light_buf[i] == init_light) //�ҵ���ʼ��λ���±�+1 ��Ϊ��ʼ��λ��ֵ
        {
            k = i + 1;
            break;
        }
    }

    //���ڵ�Ŀ�굵λ ~(��ʼ��λ-Ŀ�굵λ)
    if (k <= gear) //Ŀ�굵λ���ڵ��ڵ�ǰ��λ ����ڵ�λΪ b=gear-k
    {
        b = gear - k;
        //Send_Debug_string("<=\n");
    }
    else
    {
        // Send_Debug_string(">\n");
        switch (gear)
        {
        case 1:
            if (k == 4)
            {
                b = 1;
            }
            else if (k == 3)
            {
                b = 2;
            }
            else if (k == 2)
            {
                b = 3;
            }
            break;
        case 2:
            if (k == 4)
            {
                b = 2;
            }
            else if (k == 3)
            {
                b = 3;
            }
            break;
        case 3:
            b = 3;
            break;
        default:
            break;
        }
    }
    /*  Send_Debug_string("\nc=");
    Send_Debug_num(k);
    Send_Debug_string("\nt=");
    Send_Debug_num(gear);
    Send_Debug_string("\nb=");
    Send_Debug_num(b);
    Send_Debug_string("\n");
*/
    switch (b)
    {
    case 0:
        break;
    case 1:
        Infrared_Send(light_1, 4);
        break;
    case 2:
        Infrared_Send(light_2, 4);
        break;
    case 3:
        Infrared_Send(light_3, 4);
        break;
    default:
        break;
    }
    return k;
}

/**
 * @description: ����·�Ʊ�־�ﵵλ���� �ڶ���(���ö�ȡ���նȼ��������Ƿ���ڳɹ�)
 * @param {unsigned int} gear ��λ
 * @return {*}��ʼ��λ
 */
unsigned int OFlag_light2(unsigned int gear)
{
    int light_buf[4];
    int light_initV = 0; //��ʼ��λ����ֵ
    int index = 0;       //��ʼ��λ�±걣��
    int target = 0;      //Ŀ�굵λ
    int error = 30;      //���ն����ֵ
    int light_value = 0; //���ն�
    /*ԭ��+1�� ��ȡ�ĸ���λ�Ĺ��ն�ֵ ������ǿ������ó���λ*/
    TaskBoard_BH();
    TaskBoard_BH();
    delay_ms(200);
    for (int i = 0; i < 4; i++) //��ȡ������λ��ֵ
    {
        if (i == 0) //�״ζ���ʼ������ֵ
        {
            light_initV = light_value = TaskBoard_BH();
            delay_ms(1);
        }
        light_buf[i] = light_value;
        Infrared_Send(light_1, 4); //��λ+1
        delay_ms(500);
        delay_ms(500);
        delay_ms(500);
        for (int n = 0; n < 6; n++)
        {
            light_value = TaskBoard_BH(); //�Աȹ��ն�
            delay_ms(1);
            //���ù��ն��ж��Ƿ�������ɹ�
            if ((light_buf[i] - error <= light_value) && (light_value <= light_buf[i] + error))
            {
                Infrared_Send(light_1, 4); //��λ+1
                delay_ms(500);
                delay_ms(500);
                delay_ms(500);
            }
            else
            {
                break;
            }
        }
        //sprintf(buf, "buf[%d]=%d\n", i, light_buf[i]);
        //Send_Debug_string(buf);
    }
    arr_sort(light_buf, 4); //������ֵ��������

    /*for (int i = 0; i < 4; i++)
    {
        sprintf(buf, "r[%d]=%d\n", i, light_buf[i]);
        Send_Debug_string(buf);
    }*/
    for (int d = 0; d < 4; d++) //�����������±������ʼ��λֵ
    {
        if (light_buf[d] == light_initV)
        {
            index = d + 1;
            break;
        }
    }
    //������ڵ�λ��
    if (index <= gear) //Ŀ�굵λ���ڵ��ڵ�ǰ��λ���򣺵��ڵ�λ=Ŀ�굵λ-��ʼ��λ
    {
        target = gear - index;
    }
    else
    {
        switch (gear)
        {
        case 1:
            if (index == 4)
            {
                target = 1;
            }
            else if (index == 3)
            {
                target = 2;
            }
            else if (index == 2)
            {
                target = 3;
            }
            break;
        case 2:
            if (index == 4)
            {
                target = 2;
            }
            else if (index == 3)
            {
                target = 3;
            }
            break;
        case 3:
            target = 3;
            break;
        default:
            break;
        }
    }
    //���ݵ�ǰ��λ��Ŀ�굵λ�������յ���
    switch (target)
    {
    case 0:
        break;
    case 1:
        Infrared_Send(light_1, 4);
        break;
    case 2:
        Infrared_Send(light_2, 4);
        break;
    case 3:
        Infrared_Send(light_3, 4);
        break;
    default:
        break;
    }

    return index; //���س�ʼ��λ
}

/**
 * @description: ���̨������־�� Ĭ�Ͽ����뿪���رղ���
 * @param {unsigned char} mode 1.���� 0.�ر�
 * @return {*}
 */
void OFlag_alarm(unsigned char mode)
{
    if (mode)
    {
        Agreement_Send(Agreement_mode_infrared, alarm_open); //�������̨����
    }
    else
    {
        Agreement_Send(Agreement_mode_infrared, alarm_close); //�رշ��̨����
    }
}

//���Ϳ����뿪�����̨
void OFlag_alarm_open(unsigned char *KEY)
{
    Agreement_Send(Agreement_mode_infrared, KEY);
}

/**
 * @description: ���̨�޸Ŀ�����
 * @param {uint8_t*} data �������ַ���
 * @return {*}
 */
void OFlag_alarm_change(uint8_t *data)
{
    uint8_t dt[8];
    dt[0] = alarm_change[0];                           //֡ͷ1
    dt[1] = alarm_change[1];                           //֡ͷ2
    dt[2] = 0x10;                                      //��ָ��
    dt[3] = alarm_open[0] = data[0];                   //����������1
    dt[4] = alarm_open[1] = data[1];                   //����������2
    dt[5] = alarm_open[2] = data[2];                   //����������3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = alarm_change[7];                           //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);         //����ǰ��λ����������
    dt[2] = 0x11;                                      //��ָ��
    dt[3] = alarm_open[3] = data[3];                   //����������4
    dt[4] = alarm_open[4] = data[4];                   //����������5
    dt[5] = alarm_open[5] = data[5];                   //����������6
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    Agreement_Send(Agreement_mode_Zigbee, dt);         //���ͺ���λ����������
}

/**
 * @description: ������ʾ��־������֡���� 
 * @param {uint8_t} cmd ��ָ������ ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} data1 ����x
 * @param {uint8_t} data2
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @return {*}
 */
void OFlag_ltDisplay(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    uint8_t dt[6];
    dt[0] = lt_display[0]; //֡ͷ
    dt[1] = cmd;           //��ָ��
    dt[2] = data1;         //����1
    dt[3] = data2;         //����2
    dt[4] = data3;         //����3
    dt[5] = data4;         //����4
    Agreement_Send(Agreement_mode_infrared, dt);
}

/**
 * @description: ������ʾ��־�� ���Ƽ�����
 * @param {uint8_t} *CP ��λ�����ַ���
 * @param {uint8_t} *coordinate �����ַ���
 * @return {*}
 * ��:OFlag_ltDisplay_show("A12345","F1");
 */
void OFlag_ltDisplay_show(uint8_t *CP, uint8_t *coordinate)
{
    OFlag_ltDisplay(32, CP[0], CP[1], CP[2], CP[3]); //����ǰ��λ
    delay_ms(500);
    OFlag_ltDisplay(16, CP[4], CP[5], coordinate[0], coordinate[1]); //���ƺ���λ������
    delay_ms(100);
}

/**
 * @description: LED��ʾ��־�����
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_LED(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = LED_cmd[0];                                //֡ͷ1
    dt[1] = LED_cmd[1];                                //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = LED_cmd[7];                                //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: LED��ʾ��־����� ��ʱģʽ
 * @param {uint8_t} cmd 0.�ر� 1.�� 2.����
 * @return {*}
 */
void OFlag_LED_time(uint8_t cmd)
{
    OFlag_LED(0x03, cmd, 0x00, 0x00);
}

/**
 * @description: LED��ʾ��־����� ָ������ʾָ������(ʮ������)
 * @param {uint8_t} row 1.��һ�� 2.�ڶ���
 * @param {uint8_t} data12 ����[1]������[2]
 * @param {uint8_t} data34 ����[3]������[4]
 * @param {uint8_t} data56 ����[5]������[6]
 * @return {*}
 */
void OFlag_LED_show(uint8_t row, uint8_t data12, uint8_t data34, uint8_t data56)
{
    OFlag_LED(row, data12, data34, data56);
}

/**
 * @description: LED��ʾ��־����� ��ʾ����ģʽ
 * @param {unsigned int} mm �������� ʮ���� ��λmm
 * @return {*}
 */
void OFlag_LED_jl(unsigned int mm)
{
    OFlag_LED(0x04, 0x00, (int)mm / 100, mm / 10 % 10 * 16 + mm % 10);
}

/**
 * @description:��բ��־�����
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_DZ(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = DZ_cmd[0];                                 //֡ͷ1
    dt[1] = DZ_cmd[1];                                 //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = DZ_cmd[7];                                 //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}
/**
 * @description: ��բ����
 * ����ʱ���⳵�ƶ��ɿ�����բ������ʱ��ָ�����ƺ�
 * @param {uint8_t*} str �����ַ���
 * @return {*}
 */
void OFlag_DZ_open(uint8_t *str)
{
    if (*str == 0)
    {
        OFlag_DZ(0x01, 0x01, 0x00, 0x00);
    }
    else
    {
        OFlag_DZ_show(str);
    }
}

/**
 * @description: ��բ�ر�
 * @param {*}
 * @return {*}
 */
void OFlag_DZ_close(void)
{
    OFlag_DZ(0x01, 0x02, 0x00, 0x00);
}
/**
 * @description: ��բ��ʾ����
 * @param {uint8_t*} str �����ַ���
 * @return {*}
 */
void OFlag_DZ_show(uint8_t *str)
{
    OFlag_DZ(0x10, str[0], str[1], str[2]);
    delay_ms(400);
    OFlag_DZ(0x11, str[3], str[4], str[5]);
    delay_ms(100);
}
/**
 * @description: ��բ��־����� ��բ״̬����
 * @param {uint8_t*} status ����֡
 * @return {*} 1.��բ���ڿ���״̬
 */
unsigned int OFlag_DZ_status(uint8_t *status)
{

    if (status[2] == 0x01)
    {
        if (status[3] == 0x00)
        {
            if (status[4] == 0x05)
            {
                return 1;
            }
        }
    }
    return 0;
}
/**
 * @description: �򿪵�բ�ҵȴ���բ���ڴ�״̬
 * @param {uint8_t} count �ȴ�ʱ�� ��λ����
 * @param {uint8_t} *str ���� ���賵�ƴ���""
 * @return {*} 1.�ɹ� 0.ʧ��
 */
unsigned int OFlag_DZ_wait(uint8_t count, uint8_t *str)
{
    uint8_t _cmd[8] = {0x55, 0x03, 0x20, 0x01, 0, 0, calc_CheckSum(0x20, 0x01, 0, 0), 0xbb};
    OFlag_DZ_open(str); //����򿪵�բ
    delay_ms(200);
    OFlag_DZ_open(str); //����򿪵�բ
    return OFlag_xx(_cmd, &DZ_flag, count);
}

/**
 * @description: ���߳���־�￪�� 10����Զ��ر�
 * @param {*}
 * @return {*}
 */
void OFlag_WX_open(void)
{
    uint8_t dt[8];
    dt[0] = WX_cmd[0];                                 //֡ͷ1
    dt[1] = WX_cmd[1];                                 //֡ͷ2
    dt[2] = 0x01;                                      //��ָ��
    dt[3] = 0x01;                                      //��ָ��1
    dt[4] = 0x00;                                      //��ָ��2
    dt[5] = 0x00;                                      //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = WX_cmd[7];                                 //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description:����������־�����
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_YY(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = YY_cmd[0];                                 //֡ͷ1
    dt[1] = YY_cmd[1];                                 //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = YY_cmd[7];                                 //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ����������־����� ��������
 * @param {uint8_t} cmd 0.��������������0~7  1-7����ָ����ŵ�����
 * @return {*}
 */
void OFlag_YY_cmd(uint8_t cmd)
{
    if (cmd == 0)
    {
        OFlag_YY(0x20, 0x01, 0, 0); //�������
    }
    else
    {
        OFlag_YY(0x10, cmd, 0, 0); //ָ������
    }
}

/**
 * @description: ����������־��������ʼ���ڻ�ʱ��
 * @param {uint8_t} mode 0.�������� 1.ʱ������
 * @param {uint8_t} t1 ��|ʱ ʮ������
 * @param {uint8_t} t2 ��|��
 * @param {uint8_t} t3 ��|��
 * @return {*}
 * ��:����2021��5��20�� OFlag_YY_setTime(0,0x21,0x05,0x20);
 */
void OFlag_YY_setTime(uint8_t mode, uint8_t t1, uint8_t t2, uint8_t t3)
{
    if (mode)
    {
        OFlag_YY(0x40, t1, t2, t3);
    }
    else
    {
        OFlag_YY(0x30, t1, t2, t3);
    }
}

/**
 * @description: ����������־���ȡ��ʼ���ڻ�ʱ��
 * @param {uint8_t} mode 0.�������� 1.ʱ������
 * @param {uint8_t} count ��ʱʱ��/s
 * @return {*}
 * ��ʾ�ַ�����ʽת������
    char str[9],*time;
    time= OFlag_YY_getTime(0,10);
    str[0]=get_hexToChar(time[0])[0];
    str[1]=get_hexToChar(time[0])[1];
    str[3]=get_hexToChar(time[1])[0];
    str[4]=get_hexToChar(time[1])[1];
    str[6]=get_hexToChar(time[2])[0];
    str[7]=get_hexToChar(time[2])[1];
    str[2]=':';
    str[5]=':';
    str[8]='\0';
    Send_Debug_string(str);
 */
uint8_t *OFlag_YY_getTime(uint8_t mode, uint32_t count)
{
    YY_flag[0] = 0; //�������
    YY_flag[1] = 0;
    YY_flag[2] = 0;

    TIM_Cmd(TIM4, ENABLE); //��ȡ�����߳�
    while (count--)        //��ʱ�ж�
    {
        if (mode == 1)
        {
            OFlag_YY(0x41, 1, 0, 0);
        }
        else
        {
            OFlag_YY(0x31, 1, 0, 0);
        }
        if (YY_flag[0] != 0) //��ȡ����˳�
        {
            break;
        }
        delay_ms(500);
        delay_ms(500);
    }
    TIM_Cmd(TIM4, DISABLE);
    return YY_flag;
}

/**
 * @description: ����������־��ϳ�ָ�������ı�
 * @param {uint8_t} *str
 * @return {*}
 * |0xFD|0xXX |0xXX |����������|
 * |֡ͷ|���ֽ�|���ֽ�|������|
 *         0     0     2     ֹͣ�ϳ�����
 *         0     0     2     ��ͣ�ϳ�����
 *         0     0     2     �ָ��ϳ�����
 * ����æµ|0x55|0x06|0x01|0x4E|0|0|
 * ��������|0x55|0x06|0x01|0x4F|0|0|
 */
uint8_t OFlag_YY_play(uint8_t *str)
{
    uint8_t data[100];
    uint8_t Length = 0, dataLength = 0;

    Length = strlen((char *)str);  //�����ı�����
    dataLength = Length + 2;       //����������=�ı�����+2(���������ı������ʽ)
    data[0] = 0xFD;                //֡ͷ
    data[1] = dataLength >> 8;     //���������ȸ�8λ
    data[2] = 0x00ff & dataLength; //���������ȵ�8λ
    data[3] = 0x01;                //������
    data[4] = 0x00;                //0.GB2312 1.GBK 2.BIG5 3.Unicode

    for (int i = 0; i < Length; i++)
    {
        data[i + 5] = str[i];
    }
    Send_ZigbeeData_To_Fifo(data, 5 + Length);
}

/**
 * @description: TFT��־�����
 * @param {uint8_t} mode 'A'.ѡ��TFTA 'B'.ѡ��TFTB
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_TFT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = TFT_cmd_A[0];                                           //֡ͷ1
    dt[1] = mode == 1 || mode == 'A' ? TFT_cmd_A[1] : TFT_cmd_B[1]; //֡ͷ2
    dt[2] = cmd;                                                    //��ָ��
    dt[3] = fcmd1;                                                  //��ָ��1
    dt[4] = fcmd2;                                                  //��ָ��2
    dt[5] = fcmd3;                                                  //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]);              //У���
    dt[7] = TFT_cmd_A[7];                                           //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}
/**
 * @description: TFT��־����� ��ʾ����
 * @param {uint8_t} mode 'A'.ѡ��TFTA 'B'.ѡ��TFTB
 * @param {uint8_t*} str �����ַ��� ��ĸֻ������ʾ��д
 * @return {*}
 */
void OFlag_TFT_show(uint8_t mode, uint8_t *str)
{
    OFlag_TFT(mode, 0x20, str[0], str[1], str[2]);
    delay_ms(500);
    OFlag_TFT(mode, 0x21, str[3], str[4], str[5]);
    delay_ms(100);
}

/**
 * @description: TFT��־����� ��ʾʮ������
 * @param {uint8_t} mode  'A'.ѡ��TFTA 'B'.ѡ��TFTB  
 * @param {uint8_t} data1 ����x
 * @param {uint8_t} data2 
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @param {uint8_t} data5
 * @param {uint8_t} data6
 * @return {*}
 * ��OFlag_TFT_showHEX('A',0xA,1,0xB,2,0xC,3); ��ʾA1B2C3
 */
void OFlag_TFT_showHEX(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6)
{
    OFlag_TFT(mode, 0x40, data1 * 16 + data2, data3 * 16 + data4, data5 * 16 + data6);
    delay_ms(200);
    OFlag_TFT(mode, 0x40, data1 * 16 + data2, data3 * 16 + data4, data5 * 16 + data6);
    delay_ms(200);
}

/**
 * @description: TFT��־����� ��ʾ����
 * @param {uint8_t} mode 'A'.ѡ��TFTA 'B'.ѡ��TFTB  
 * @param {uint8_t} data ʮ�������� ����ֻ֧����λ
 * @return {*}
 */
void OFlag_TFT_jl(uint8_t mode, uint32_t data)
{
    OFlag_TFT(mode, 0x50, 0, data / 100, ((data / 10 % 10) * 16) + (data % 10));
}

/**
 * @description: ���ܽ�ͨ�Ʊ�־�����
 * @param {uint8_t} mode 1.ѡ��ͨ��A 0.ѡ��ͨ��B
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_JT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = JT_cmd_A[0];                               //֡ͷ1
    dt[1] = mode == 1 ? JT_cmd_A[1] : JT_cmd_B[1];     //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = JT_cmd_A[7];                               //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ���ܽ�ͨ�Ʊ�־����� ����ʶ��ģʽ���������
 * @param {uint8_t} mode 1.ѡ��ͨ��A 0.ѡ��ͨ��B
 * @param {uint8_t} sb 0.����ʶ��ģʽ 1.��ɫʶ����ȷ�� 2.��ɫ 3.��ɫ
 * @return {*}
 */
void OFlag_JT_cmd(uint8_t mode, uint8_t sb)
{
    if (sb != 0)
    {
        OFlag_JT(mode, 0x02, sb, 0, 0);
    }
    else
    {
        OFlag_JT(mode, 0x01, 0, 0, 0);
    }
}

/**
 * @description: ���ܽ�ͨ�Ʊ�־����� ״̬����
 * @param {uint8_t*} status ����֡
 * @param {uint8_t} mode 1.���ܽ�ͨ�Ʊ�־��A 0.���ܽ�ͨ�Ʊ�־��B 
 * @return {*} 0.���� 1.�ɹ� 2.ʧ��
 */
unsigned int OFlag_JT_status(uint8_t *status, uint8_t mode)
{
    uint8_t _mode;

    _mode = mode == 1 ? JT_cmd_A[1] : JT_cmd_B[1]; //���ܽ�ͨ�Ʊ�־��A�����ܽ�ͨ�Ʊ�־��B
    if (status[0] == JT_cmd_A[0] && status[1] == _mode && status[7] == JT_cmd_A[7])
    {
        if (status[2] == 0x01) //��ָ��
        {
            if (status[3] == 0x01) //��ָ��1
            {
                if (status[4] == 0x07) //����ʶ��ģʽ�ɹ�
                {
                    return 1;
                }
                else if (status[4] == 0x08) //ʧ��
                {
                    return 2;
                }
            }
        }
    }
    return 0;
}
/**
 * @description: ���ܽ�ͨ�ƽ���ʶ��ģʽ�ҵȴ�ʶ���� �ȴ�ʱ��10s
 * @param {uint8_t} mode 1.��A 0.��B
 * @return {*}1.�ɹ� 0.ʧ��
 */
unsigned int OFlag_JT_Wait(uint8_t mode)
{
    unsigned int t = 0, result = 1, _buffer = 0;

    OFlag_JT_cmd(mode, 0); //�������ʶ��ģʽ
    while (_buffer != 1)   //�ȴ�����ʶ��ģʽ�ɹ�
    {
        _buffer = OFlag_JT_status(Zigb_Rx_Buf, mode);
        t++;
        if (t >= 20)
        {
            result = 0;
            break;
        }
        delay_ms(500);
        OFlag_JT_cmd(mode, 0); //�������ʶ��ģʽ
    }
    return result;
}

/**
 * @description: ���峵���־�����
 * @param {uint8_t} mode 1.ѡ�񳵿�A 0.ѡ�񳵿�B
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_CK(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = CK_cmd_A[0];                               //֡ͷ1
    dt[1] = mode == 1 ? CK_cmd_A[1] : CK_cmd_B[1];     //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = CK_cmd_A[7];                               //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ���峵���־�� ����ָ�����
 * @param {uint8_t} mode 1.ѡ�񳵿�A 0.ѡ�񳵿�B
 * @param {uint8_t} num ѡ����1-4
 * @return {*}
 */
void OFlag_CK_cmd(uint8_t mode, uint8_t num)
{

    if (mode == 'A')
    {
        mode = 1;
    }
    if (mode == 'B')
    {
        mode = 0;
    }
    OFlag_CK(mode, 0x01, num, 0x00, 0x00);
}

/**
 * @description: ���峵���־�� ���󷵻س���״̬��Ϣ
 * @param {uint8_t} mode 1.ѡ�񳵿�A 0.ѡ�񳵿�B
 * @param {uint8_t} num 1.���󷵻ص�ǰ������ 2.���󷵻س���ǰ������״̬
 * @return {*}
 */
void OFlag_CK_statusGet(uint8_t mode, uint8_t num)
{
    OFlag_CK(mode, 0x02, num, 0x00, 0x00);
}

/**
 * @description: ���峵���־��״̬����
 * @param {uint8_t} *status ����֡
 * @param {uint8_t} CK 1.����A 0.����B
 * @param {uint8_t} mode 0.���س��⵱ǰ��� 1.ǰ�������״̬ 2.��������״̬ 3.ǰ�����״̬
 * @return {unsigned int} 0.ʧ��/���⵱ǰ���1-4/����״̬1.������ 2.δ������
 */
unsigned int OFlag_CK_status(uint8_t *status, uint8_t CK, uint8_t mode)
{
    uint8_t _ck;
    _ck = CK == 1 ? CK_cmd_A[1] : CK_cmd_B[1]; //����A�򳵿�B

    if (status[0] == CK_cmd_A[0] && status[1] == _ck && status[7] == CK_cmd_A[7]) //����A֡ͷ ֡β
    {
        if (status[2] == 0x03) //������ָ��
        {
            if (status[3] == 0x01 && mode == 0) //��ָ��1 0x01
            {
                return status[4]; //���ز��
            }
            else if (status[3] == 0x02) //��ָ��1 0x02
            {
                if (mode == 1)
                {
                    return status[4];
                }
                else if (mode == 2)
                {
                    return status[5];
                }
                else if (mode == 3)
                {
                    if (status[4] == 0x01 && status[5] == 0x01)
                    {
                        return 0x01;
                    }
                    else
                    {
                        return 0x02;
                    }
                }
            }
        }
    }
    return 0;
}

/**
 * @description: ���峵���־�ﵽ��ָ������ҵȴ��������
 * @param {uint8_t} mode 1.����A 0.����B
 * @param {uint8_t} num ѡ����1-4
 * @param {uint8_t} count �ȴ����� 1��1s+
 * @return {*}1.�ɹ� 0.ʧ��
 */
unsigned int OFlag_CK_Wait(uint8_t mode, uint8_t num, uint8_t count)
{
    uint8_t _cmd[8] = {0x55, 0, 0x02, 0x01, 0, 0, calc_CheckSum(0x20, 0x01, 0, 0), 0xbb};

    uint8_t r = 1, t = 0;
    OFlag_CK_cmd(mode, num); //���⵽num��
    delay_ms(100);
    OFlag_CK_cmd(mode, num); //���⵽num��
    delay_ms(100);
    if (mode == 'A' || mode == 1)
    {
        _cmd[1] = 0x0D;
    }
    else
    {
        _cmd[1] = 0x05;
    }
    CK_flag = 0;                                 //���ý���״̬
    TIM_Cmd(TIM4, ENABLE);                       //��������
    Agreement_Send(Agreement_mode_Zigbee, _cmd); //���Ͷ������ش�
    delay_ms(1);
    while (CK_flag != num) //�ȴ��ش����
    {
        t++;
        if (t >= count) //��ʱ����
        {
            r = 0; //0.���س�ʱ��־1.��������
            break;
        }
        if (CK_flag == num) //�����������
        {
            break;
        }
        delay_ms(500);
        delay_ms(500);
        Agreement_Send(Agreement_mode_Zigbee, _cmd); //���Ͷ������ش�
    }
    TIM_Cmd(TIM4, DISABLE);
    return r;
}

/**
 * @description: ETC��־�����
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_ETC(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = ETC_cmd[0];                                //֡ͷ1
    dt[1] = ETC_cmd[1];                                //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = ETC_cmd[7];                                //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ETC��־������բ�ſ���
 * @param {uint8_t} mode 1.���բ�� 0.�Ҳ�բ�� 2.ȫ��
 * @param {uint8_t} cmd 1.���� 0.�½� 
 * @return {*}
 */
void OFlag_ETC_cmd(uint8_t mode, uint8_t cmd)
{
    if (mode == 0)
    {
        OFlag_ETC(0x08, 0x00, cmd == 1 ? 0x01 : 0x02, 0x00);
    }
    else if (mode == 1)
    {
        OFlag_ETC(0x08, cmd == 1 ? 0x01 : 0x02, 0x00, 0x00);
    }
    else if (mode == 2)
    {
        OFlag_ETC(0x08, cmd == 1 ? 0x01 : 0x02, cmd == 1 ? 0x01 : 0x02, 0x00);
    }
}

/**
 * @description: �����ȴ�ETC ��ETC������ִ��task_ETC();
 * @param {*}
 * @return {*}
 */

void OFlag_ETC_wait(void)
{
    OFlag_ETCflag = 1;
}

/**
 * @description: ��ȡբ��״̬ 10��բ���Զ��ر�
 * @param {uint8_t*} status ���жϵ�֡����
 * @return {*} 0.ʧ�� 1.բ��Ϊ����״̬
 */
unsigned int OFlag_ETC_status(uint8_t *status)
{

    if (status[2] == 0x01) //��ָ��
    {
        if (status[3] == 0x01) //��ָ��1
        {
            if (status[4] == 0x06) //��ָ��2
            {
                if (status[4] == 0x00) //��ָ��3
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

/**
 * @description: ������α�־�����
 * @param {uint8_t} cmd ��ָ�� ����˵����������Ƕ��ʽʵѵϵͳͨ��Э��
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_DX(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = DX_cmd[0];                                 //֡ͷ1
    dt[1] = DX_cmd[1];                                 //֡ͷ2
    dt[2] = cmd;                                       //��ָ��
    dt[3] = fcmd1;                                     //��ָ��1
    dt[4] = fcmd2;                                     //��ָ��2
    dt[5] = fcmd3;                                     //��ָ��3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //У���
    dt[7] = DX_cmd[7];                                 //֡β
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: �����ѯ����ͨ��״̬
 * @param {*}
 * @return {*}
 */
void OFlag_DX_statusGet(void)
{
    OFlag_DX(0x10, 0x01, 0x00, 0x00);
}

/**
 * @description: ��ȡ����ͨ��״̬
 * @param {uint8_t*} status ֡����
 * @return {*} 0.ʧ��
 * 0x31:����˳·ͨ��������A->B    
 * 0x32:����˳·ͨ��������B->A
 * 0x33:����δ˳��ͨ��
 */
unsigned int OFlag_DX_status(uint8_t *status)
{
    if (status[0] == DX_cmd[0] && status[1] == DX_cmd[1] && status[7] == DX_cmd[7])
    {
        if (status[2] == 0x10 && status[3] == 0x01)
        {
            return status[4];
        }
    }
    return 0;
}

/**
 * @description: ������εȴ���ȡ����ͨ��״̬
 * @param {uint8_t} count �ȴ�ʱ�� ��λ����
 * @return {*} 0.ʧ��
 * 0x31:����˳·ͨ��������A->B    
 * 0x32:����˳·ͨ��������B->A
 * 0x33:����δ˳��ͨ��
 */
unsigned int OFlag_DX_wait(uint8_t count)
{
    uint8_t _cmd[8] = {0x55, DX_cmd[1], 0x10, 0x01, 0, 0, calc_CheckSum(0x10, 0x01, 0, 0), 0xbb};
    if (OFlag_xx(_cmd, &DX_flag, count))
    {
        return DX_flag;
    }
    return 0;
}

/**
 * @description: ������������� ��⵽��ɫ���ٴ���ʻn��(�ο�����ֵ1300)
 * @param {unsigned int} speed ��ʻ�ٶ�
 * @param {unsigned int} refMP �ο�����
 * @return {*} 
 */
void OFlag_DX_carGo(unsigned int speed, unsigned int refMP)
{
    PID_Set(25, 0, 300); //�ϵ���ǰ���ٵ�������
    while (PID_Track3(speed) != 99)
    {
        delay_ms(1);
    } //��ʻ����ɫ��ֹͣѭ��
    MasterCar_Stop();
    PID_Set_recovery();
    MasterCar_GoMP(speed, refMP);
}

/**
 * @description: ����ƽ̨�������Զ�����ϵͳ�ϴ��������
 * @param {uint8_t} num �������
 * @return {*}
 */
void OFlag_YYupload(uint8_t num)
{
    uint8_t dt[8] = {0xaf, 0x06, 0x00, 0x02, 0x00, 0x00, 0x01, 0xbb};
    dt[2] = num;
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ��������Zigbeeָ����������
 * @param {*}
 * @return {*}
 */
void OFlag_SlaveRun(void)
{
    OFlag_Zigbee(0x02, 0, 0, 0, 0);
    delay_ms(300);
    OFlag_Zigbee(0x02, 0, 0, 0, 0);
    delay_ms(300);
    OFlag_Zigbee(0x02, 0, 0, 0, 0);
}

/**
 * @description: ���������󲢵ȴ������������� ��ʱ��Ӧ
 * @param {uint8_t} count �ȴ�ʱ�� ��λ��s
 * @return {*}1.�ɹ� 0.ʧ��
 * 
 */
uint8_t OFlag_SlaveRun_wait(uint8_t count)
{
    TaskBoard_Time(count);
    OFlag_SLAVEflag = 1; //���ø�������������־
}

/**
 * @description: ��������Zigbeeָ�����
 * @param {uint8_t} cmd ��ָ��
 * -----------------------------------------------
 * 2021ͨ��Э��v2.0
 * 0x00	��������	   0x00       0x00   0x00
 * 0x05	�������	   ������1-4�� 0x00   0x00		
 * 0x07	·�Ƶ�λ	   ��λ	    ��λ	��λ
 * 0x08	����·������1  ����[0]	����[1]	����[2]
 * 0x09	����·������2  ����[3]	����[4]	����[5]
 * 0x0A	���ͱ�������   ����	    ����	����
 * 0X0B	�����������   ����	    ����	����
 * 0X0C	��ͷ����       ����	    ����	����
 * 0X0D	RFID��λ��	  0xA1	   ��A��	   ��6��
 * 0X0E	�������1	  ������[0]	������[1]	������[2]
 * 0X0F	�������2	  ������[3]	������[4]	������[5]
 * -----------------------------------------------
 * @param {uint8_t} fcmd1 ��ָ��x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
*/
void OFlag_SlaveSendZigbee(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    OFlag_Zigbee(0x02, cmd, fcmd1, fcmd2, fcmd3);
}

/**
 * @description: ��ȡ��ָ��
 * @param {uint8_t} *status ����֡
 * @return {*}
 */
uint8_t OFlag_GetCmd(uint8_t *status)
{
    return status[2];
}
/**
 * @description: ��ȡ��ָ��1
 * @param {uint8_t} *status ����֡
 * @return {*}
 */
uint8_t OFlag_GetCmd1(uint8_t *status)
{
    return status[3];
}
/**
 * @description: ��ȡ��ָ��2
 * @param {uint8_t} *status ����֡
 * @return {*}
 */
uint8_t OFlag_GetCmd2(uint8_t *status)
{
    return status[4];
}
/**
 * @description: ��ȡ��ָ��3
 * @param {uint8_t} *status ����֡
 * @return {*}
 */
uint8_t OFlag_GetCmd3(uint8_t *status)
{
    return status[5];
}

/**
 * @description: zigbeeͨ�������ͽ��մ���
 * @param {uint8_t} *cmd Э���
 * @param {uint8_t} *flag ��־
 * @param {uint8_t} time ��ʱʱ��
 * @return {*} 1.�ɹ� 0.��ʱ
 */
uint8_t OFlag_xx(uint8_t *cmd, uint8_t *flag, uint8_t time)
{
    uint8_t r = 1, t = 0;
    *flag = 0;                                  //���ý���״̬
    TIM_Cmd(TIM4, ENABLE);                      //��������
    Agreement_Send(Agreement_mode_Zigbee, cmd); //���Ͷ������ش�
    //delay_ms(1);
    while (*flag == 0) //�ȴ��ش����
    {
        t++;
        if (t >= time) //��ʱ����
        {
            r = 0; //0.���س�ʱ��־1.��������
            break;
        }
        if (*flag != 0) //�����������
        {
            break;
        }
        delay_ms(500);
        delay_ms(500);
        Agreement_Send(Agreement_mode_Zigbee, cmd); //���Ͷ������ش�
    }
    TIM_Cmd(TIM4, DISABLE);
    return r;
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        if (Zigbee_Rx_flag) //���յ�zigbee��Ϣ
        {
            Operation_Zigbee();
        }
    }
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

void Operation_Zigbee(void)
{
    switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee�ڵ㷴��ѡ��
    {
    case zNum_DZ: //��բ
        DZ_flag = OFlag_DZ_status(Zigb_Rx_Buf);
        break;
    case zNum_CK_A: //����A
        CK_flag = OFlag_CK_status(Zigb_Rx_Buf, 1, 0);
        break;
    case zNum_CK_B: //����B
        CK_flag = OFlag_CK_status(Zigb_Rx_Buf, 0, 0);
        break;
    case zNum_DX: //�������
        DX_flag = OFlag_DX_status(Zigb_Rx_Buf);
        break;
    case zNum_YY: //����������־��
        YY_flag[0] = Zigb_Rx_Buf[3];
        YY_flag[1] = Zigb_Rx_Buf[4];
        YY_flag[2] = Zigb_Rx_Buf[5];
        break;
    case zNum_MasterCar: //���Դӳ���������ָ�������ʧ�أ�
        /*if(SLAVE_flag==0)
        {
        SlaveCar_TaskRunThread(Zigb_Rx_Buf);
        }*/
        break;
    default: //δ����zigbee���
        //Send_Debug_num(OFlag_check(Zigb_Rx_Buf));
        break;
    }

    Zigbee_Rx_flag = 0;
}

void OFlag_resetWaitFlag(void)
{
    OFlag_SLAVEflag = OFlag_ETCflag = 0;
}