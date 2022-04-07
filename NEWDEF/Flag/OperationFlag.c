/*
 * @Description: 标志物操控
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-04-07 15:04:38
 * @version: 2021.05.20协议
 */
#include "OperationFlag.H"
#include "delay.h"
#include "can_user.h"
#include "infrared.h"
#include "Function.h" //目标档位
#include "PID.h"
#include "MasterCarCotrol.h"
#include "MyString.h"
#include "string.h"
#define Agreement_mode_infrared 1
#define Agreement_mode_Zigbee 2

uint8_t DZ_flag = 0;            //道闸标志物开启
uint8_t CK_flag = 0;            //车库标志物下降完毕
uint8_t DX_flag = 0;            //特殊地形方向
uint8_t YY_flag[3] = {0, 0, 0}; //语音标志物日期时间读取
uint8_t OFlag_SLAVEflag = 0;    //等待副车
uint8_t OFlag_ETCflag = 0;      //等待ETC
void Operation_Zigbee(void);
uint8_t OFlag_xx(uint8_t *cmd, uint8_t *flag, uint8_t time);
/*红外通讯*/
#if 0
uint8_t light_1[4] = {0x00, 0xff, 0x0c, 0xf3};              //智能路灯标志物  光源档位+1
uint8_t light_2[4] = {0x00, 0xff, 0x18, 0xe7};              //智能路灯标志物  光源档位+2
uint8_t light_3[4] = {0x00, 0xff, 0x5e, 0xa1};              //智能路灯标志物  光源档位+3
#else
uint8_t light_1[4] = {0x00, 0xFF, 0x0C, ~(0x0C)}; // 智能路灯 光源挡位加1
uint8_t light_2[4] = {0x00, 0xFF, 0x18, ~(0x18)}; // 智能路灯 光源挡位加2
uint8_t light_3[4] = {0x00, 0xFF, 0x5E, ~(0x5E)}; // 智能路灯 光源挡位加3
#endif

uint8_t alarm_open[6] = {0x03, 0x05, 0x14, 0x45, 0xDE, 0x92};  // 烽火台(警报器) 默认开启码
uint8_t alarm_close[6] = {0x67, 0x34, 0x78, 0xA2, 0xFD, 0x27}; // 烽火台(警报器) 关闭
uint8_t lt_display[6] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};  // 立体显示标志物 |帧头|主指令|数据1|数据2|数据3|数据4|

/*Zigbee通讯协议格式
----------------------------------------------------------------
|帧头|     01-17       | 0xXX | 0xXX | 0xXX  | 0xXX | 0xXX |帧尾|
----------------------------------------------------------------
|0x55|Zigbee终端节点编号|主指令|副指令1|副指令2|副指令3|校验和|0xbb|
----------------------------------------------------------------
校验和=(主指令+副指令1+副指令2+副指令3)%256
*/
uint8_t alarm_change[8] = {0x55, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB}; //烽火台(警报器)修改开启码
uint8_t LED_cmd[8] = {0x55, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};      //LED显示标志物
uint8_t DZ_cmd[8] = {0x55, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //道闸标志物控制指令与回传结构
uint8_t WX_cmd[8] = {0x55, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //无线充电标志物
uint8_t YY_cmd[8] = {0x55, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //语音播报标志物
uint8_t TFT_cmd_A[8] = {0x55, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};    //智能TFT显示标志物A
uint8_t TFT_cmd_B[8] = {0x55, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};    //智能TFT显示标志物B
uint8_t JT_cmd_A[8] = {0x55, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //智能交通灯标志物A
uint8_t JT_cmd_B[8] = {0x55, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //智能交通灯标志物B
uint8_t CK_cmd_A[8] = {0x55, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //立体车库标志物A
uint8_t CK_cmd_B[8] = {0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};     //立体车库标志物B
uint8_t ETC_cmd[8] = {0x55, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};      //ETC系统标志物控制与回传
uint8_t DX_cmd[8] = {0x55, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};       //特殊地形标志物
/*
******红外通讯********
1.智能路灯标志物
2.报警器标志物
3.立体显示器标志物（指令多参考通信协议）
4.LCD显示标志物
*****Zigbee通讯*******
1.LED显示标志物
2.道闸标志物
3.无线充电标志物---
4.语音播报标志物（指令多参考通信协议）
5.TFT显示器标志物（指令多参考通信协议）
6.智能交通灯标志物
7.控制从车
8.立体车库标志物
9.ETC系统标志位数据---
10.烽火台报警修改开启码---
11.地形检测标志物
12.竞赛平台自动评分系统上传语音编号---
*/

/**
 * @description: 校验和计算 在本协议中校验和=(主指令+副指令1+副指令2+副指令3)%256
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
 * @description: 检测是否为Zigbee节点的包
 * @param {uint8_t*} data
 * @return {*} 失败返回0 成功返回节点编号
 */
unsigned int OFlag_check(uint8_t *data)
{
    if (data[0] == 0x55 && data[7] == 0xbb) //帧头 帧尾
    {
        if (data[1]) //zigbee节点编号不为0
        {
            return data[1];
        }
    }
    return 0;
}

/**
 * @description: 协议选择发送方式发送数据 6位红外8位Zigbee
 * @param {unsigned char} mode 2=Agreement_mode_infrared or 1=Agreement_mode_Zigbee
 * @param {uint8_t*} data 欲发送的数据
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
 * @description: Zigbee通讯
 * @param {uint8_t} zigbee_num zigbee终端节点编号 查询编号对照表
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_Zigbee(uint8_t zigbee_num, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = 0x55;                                      //帧头
    dt[1] = zigbee_num;                                //zigbee终端节点编号
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = 0xbb;                                      //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 智能路灯标志物档位设置(会受到光照度读取影响)
 * 特别注意:不得靠灯太近 影响红外线
 * @param {unsigned int} gear 档位
 * @return {*}初始档位
 */
unsigned int OFlag_light(unsigned int gear)
{
    int light_buf[4], i, j;
    int init_light = 0; //初始档位
    int k = 0;          //初始档位下标保存
    int b = 0;
    /*原理：+1挡 获取四个档位的光照度大小 按光照强度排档位*/
    Get_Bh_Value(); //必须读一次 否则影响结果
    delay_ms(500);
    for (i = 0; i < 4; i++) //读取各个档位的值
    {

        light_buf[i] = TaskBoard_BH();
        delay_ms(1);
        if (i == 0)
        {
            init_light = light_buf[0]; //保存初始档位光照值
        }
        Infrared_Send(light_1, 4); //档位+1
        //Send_Debug_num(i);
        //Send_Debug_string("\n");
        delay_ms(500);
        delay_ms(500);
        delay_ms(500);
    }
    for (i = 0; i < 4; i++) //排序档位
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
        if (light_buf[i] == init_light) //找到初始档位读下标+1 就为初始档位的值
        {
            k = i + 1;
            break;
        }
    }

    //调节到目标档位 ~(初始档位-目标档位)
    if (k <= gear) //目标档位大于等于当前档位 则调节档位为 b=gear-k
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
 * @description: 智能路灯标志物档位设置 第二版(利用读取光照度检测红外线是否调节成功)
 * @param {unsigned int} gear 档位
 * @return {*}初始档位
 */
unsigned int OFlag_light2(unsigned int gear)
{
    int light_buf[4];
    int light_initV = 0; //初始档位光照值
    int index = 0;       //初始档位下标保存
    int target = 0;      //目标档位
    int error = 30;      //光照度误差值
    int light_value = 0; //光照度
    /*原理：+1挡 获取四个档位的光照度值 按光照强度排序得出档位*/
    TaskBoard_BH();
    TaskBoard_BH();
    delay_ms(200);
    for (int i = 0; i < 4; i++) //读取各个档位的值
    {
        if (i == 0) //首次读初始档光照值
        {
            light_initV = light_value = TaskBoard_BH();
            delay_ms(1);
        }
        light_buf[i] = light_value;
        Infrared_Send(light_1, 4); //档位+1
        delay_ms(500);
        delay_ms(500);
        delay_ms(500);
        for (int n = 0; n < 6; n++)
        {
            light_value = TaskBoard_BH(); //对比光照度
            delay_ms(1);
            //利用光照度判断是否读换挡成功
            if ((light_buf[i] - error <= light_value) && (light_value <= light_buf[i] + error))
            {
                Infrared_Send(light_1, 4); //档位+1
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
    arr_sort(light_buf, 4); //将光照值进行排序

    /*for (int i = 0; i < 4; i++)
    {
        sprintf(buf, "r[%d]=%d\n", i, light_buf[i]);
        Send_Debug_string(buf);
    }*/
    for (int d = 0; d < 4; d++) //根据排序结果下标读出初始档位值
    {
        if (light_buf[d] == light_initV)
        {
            index = d + 1;
            break;
        }
    }
    //计算调节档位差
    if (index <= gear) //目标档位大于等于当前档位，则：调节档位=目标档位-初始档位
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
    //根据当前档位与目标档位进行最终调节
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

    return index; //返回初始档位
}

/**
 * @description: 烽火台警报标志物 默认开启码开启关闭操作
 * @param {unsigned char} mode 1.开启 0.关闭
 * @return {*}
 */
void OFlag_alarm(unsigned char mode)
{
    if (mode)
    {
        Agreement_Send(Agreement_mode_infrared, alarm_open); //开启烽火台警报
    }
    else
    {
        Agreement_Send(Agreement_mode_infrared, alarm_close); //关闭烽火台警报
    }
}

//发送开启码开启烽火台
void OFlag_alarm_open(unsigned char *KEY)
{
    Agreement_Send(Agreement_mode_infrared, KEY);
}

/**
 * @description: 烽火台修改开启码
 * @param {uint8_t*} data 开启码字符串
 * @return {*}
 */
void OFlag_alarm_change(uint8_t *data)
{
    uint8_t dt[8];
    dt[0] = alarm_change[0];                           //帧头1
    dt[1] = alarm_change[1];                           //帧头2
    dt[2] = 0x10;                                      //主指令
    dt[3] = alarm_open[0] = data[0];                   //开启码数据1
    dt[4] = alarm_open[1] = data[1];                   //开启码数据2
    dt[5] = alarm_open[2] = data[2];                   //开启码数据3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = alarm_change[7];                           //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);         //发送前三位开启码数据
    dt[2] = 0x11;                                      //主指令
    dt[3] = alarm_open[3] = data[3];                   //开启码数据4
    dt[4] = alarm_open[4] = data[4];                   //开启码数据5
    dt[5] = alarm_open[5] = data[5];                   //开启码数据6
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    Agreement_Send(Agreement_mode_Zigbee, dt);         //发送后三位开启码数据
}

/**
 * @description: 立体显示标志物数据帧操作 
 * @param {uint8_t} cmd 主指令数据 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} data1 数据x
 * @param {uint8_t} data2
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @return {*}
 */
void OFlag_ltDisplay(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    uint8_t dt[6];
    dt[0] = lt_display[0]; //帧头
    dt[1] = cmd;           //主指令
    dt[2] = data1;         //数据1
    dt[3] = data2;         //数据2
    dt[4] = data3;         //数据3
    dt[5] = data4;         //数据4
    Agreement_Send(Agreement_mode_infrared, dt);
}

/**
 * @description: 立体显示标志物 车牌及坐标
 * @param {uint8_t} *CP 六位车牌字符串
 * @param {uint8_t} *coordinate 坐标字符串
 * @return {*}
 * 例:OFlag_ltDisplay_show("A12345","F1");
 */
void OFlag_ltDisplay_show(uint8_t *CP, uint8_t *coordinate)
{
    OFlag_ltDisplay(32, CP[0], CP[1], CP[2], CP[3]); //车牌前四位
    delay_ms(500);
    OFlag_ltDisplay(16, CP[4], CP[5], coordinate[0], coordinate[1]); //车牌后两位及坐标
    delay_ms(100);
}

/**
 * @description: LED显示标志物控制
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_LED(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = LED_cmd[0];                                //帧头1
    dt[1] = LED_cmd[1];                                //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = LED_cmd[7];                                //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: LED显示标志物控制 计时模式
 * @param {uint8_t} cmd 0.关闭 1.打开 2.清零
 * @return {*}
 */
void OFlag_LED_time(uint8_t cmd)
{
    OFlag_LED(0x03, cmd, 0x00, 0x00);
}

/**
 * @description: LED显示标志物控制 指定行显示指定数据(十六进制)
 * @param {uint8_t} row 1.第一行 2.第二行
 * @param {uint8_t} data12 数据[1]、数据[2]
 * @param {uint8_t} data34 数据[3]、数据[4]
 * @param {uint8_t} data56 数据[5]、数据[6]
 * @return {*}
 */
void OFlag_LED_show(uint8_t row, uint8_t data12, uint8_t data34, uint8_t data56)
{
    OFlag_LED(row, data12, data34, data56);
}

/**
 * @description: LED显示标志物控制 显示距离模式
 * @param {unsigned int} mm 距离数据 十进制 单位mm
 * @return {*}
 */
void OFlag_LED_jl(unsigned int mm)
{
    OFlag_LED(0x04, 0x00, (int)mm / 100, mm / 10 % 10 * 16 + mm % 10);
}

/**
 * @description:道闸标志物控制
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_DZ(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = DZ_cmd[0];                                 //帧头1
    dt[1] = DZ_cmd[1];                                 //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = DZ_cmd[7];                                 //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}
/**
 * @description: 道闸开启
 * 测试时任意车牌都可开启道闸，比赛时需指定车牌号
 * @param {uint8_t*} str 车牌字符串
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
 * @description: 道闸关闭
 * @param {*}
 * @return {*}
 */
void OFlag_DZ_close(void)
{
    OFlag_DZ(0x01, 0x02, 0x00, 0x00);
}
/**
 * @description: 道闸显示车牌
 * @param {uint8_t*} str 车牌字符串
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
 * @description: 道闸标志物控制 道闸状态解析
 * @param {uint8_t*} status 数据帧
 * @return {*} 1.道闸处于开启状态
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
 * @description: 打开道闸且等待道闸处于打开状态
 * @param {uint8_t} count 等待时间 单位：秒
 * @param {uint8_t} *str 车牌 无需车牌传入""
 * @return {*} 1.成功 0.失败
 */
unsigned int OFlag_DZ_wait(uint8_t count, uint8_t *str)
{
    uint8_t _cmd[8] = {0x55, 0x03, 0x20, 0x01, 0, 0, calc_CheckSum(0x20, 0x01, 0, 0), 0xbb};
    OFlag_DZ_open(str); //请求打开道闸
    delay_ms(200);
    OFlag_DZ_open(str); //请求打开道闸
    return OFlag_xx(_cmd, &DZ_flag, count);
}

/**
 * @description: 无线充电标志物开启 10秒后自动关闭
 * @param {*}
 * @return {*}
 */
void OFlag_WX_open(void)
{
    uint8_t dt[8];
    dt[0] = WX_cmd[0];                                 //帧头1
    dt[1] = WX_cmd[1];                                 //帧头2
    dt[2] = 0x01;                                      //主指令
    dt[3] = 0x01;                                      //副指令1
    dt[4] = 0x00;                                      //副指令2
    dt[5] = 0x00;                                      //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = WX_cmd[7];                                 //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description:语音播报标志物控制
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_YY(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = YY_cmd[0];                                 //帧头1
    dt[1] = YY_cmd[1];                                 //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = YY_cmd[7];                                 //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 语音播报标志物控制 语音播报
 * @param {uint8_t} cmd 0.随机播报语音编号0~7  1-7播报指定编号的语音
 * @return {*}
 */
void OFlag_YY_cmd(uint8_t cmd)
{
    if (cmd == 0)
    {
        OFlag_YY(0x20, 0x01, 0, 0); //随机播报
    }
    else
    {
        OFlag_YY(0x10, cmd, 0, 0); //指定播报
    }
}

/**
 * @description: 语音播报标志物设置起始日期或时间
 * @param {uint8_t} mode 0.日期设置 1.时间设置
 * @param {uint8_t} t1 年|时 十六进制
 * @param {uint8_t} t2 月|分
 * @param {uint8_t} t3 日|秒
 * @return {*}
 * 例:日期2021年5月20日 OFlag_YY_setTime(0,0x21,0x05,0x20);
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
 * @description: 语音播报标志物读取起始日期或时间
 * @param {uint8_t} mode 0.日期设置 1.时间设置
 * @param {uint8_t} count 超时时长/s
 * @return {*}
 * 显示字符串形式转换例子
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
    YY_flag[0] = 0; //清除缓存
    YY_flag[1] = 0;
    YY_flag[2] = 0;

    TIM_Cmd(TIM4, ENABLE); //读取数据线程
    while (count--)        //超时判断
    {
        if (mode == 1)
        {
            OFlag_YY(0x41, 1, 0, 0);
        }
        else
        {
            OFlag_YY(0x31, 1, 0, 0);
        }
        if (YY_flag[0] != 0) //读取完毕退出
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
 * @description: 语音播报标志物合成指定语音文本
 * @param {uint8_t} *str
 * @return {*}
 * |0xFD|0xXX |0xXX |·····|
 * |帧头|高字节|低字节|数据区|
 *         0     0     2     停止合成语音
 *         0     0     2     暂停合成语音
 *         0     0     2     恢复合成语音
 * 语音忙碌|0x55|0x06|0x01|0x4E|0|0|
 * 语音空闲|0x55|0x06|0x01|0x4F|0|0|
 */
uint8_t OFlag_YY_play(uint8_t *str)
{
    uint8_t data[100];
    uint8_t Length = 0, dataLength = 0;

    Length = strlen((char *)str);  //计算文本长度
    dataLength = Length + 2;       //数据区长度=文本长度+2(命令字与文本编码格式)
    data[0] = 0xFD;                //帧头
    data[1] = dataLength >> 8;     //数据区长度高8位
    data[2] = 0x00ff & dataLength; //数据区长度低8位
    data[3] = 0x01;                //命令字
    data[4] = 0x00;                //0.GB2312 1.GBK 2.BIG5 3.Unicode

    for (int i = 0; i < Length; i++)
    {
        data[i + 5] = str[i];
    }
    Send_ZigbeeData_To_Fifo(data, 5 + Length);
}

/**
 * @description: TFT标志物控制
 * @param {uint8_t} mode 'A'.选择TFTA 'B'.选择TFTB
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_TFT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = TFT_cmd_A[0];                                           //帧头1
    dt[1] = mode == 1 || mode == 'A' ? TFT_cmd_A[1] : TFT_cmd_B[1]; //帧头2
    dt[2] = cmd;                                                    //主指令
    dt[3] = fcmd1;                                                  //副指令1
    dt[4] = fcmd2;                                                  //副指令2
    dt[5] = fcmd3;                                                  //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]);              //校验和
    dt[7] = TFT_cmd_A[7];                                           //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}
/**
 * @description: TFT标志物控制 显示车牌
 * @param {uint8_t} mode 'A'.选择TFTA 'B'.选择TFTB
 * @param {uint8_t*} str 车牌字符串 字母只允许显示大写
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
 * @description: TFT标志物控制 显示十六进制
 * @param {uint8_t} mode  'A'.选择TFTA 'B'.选择TFTB  
 * @param {uint8_t} data1 数据x
 * @param {uint8_t} data2 
 * @param {uint8_t} data3
 * @param {uint8_t} data4
 * @param {uint8_t} data5
 * @param {uint8_t} data6
 * @return {*}
 * 例OFlag_TFT_showHEX('A',0xA,1,0xB,2,0xC,3); 显示A1B2C3
 */
void OFlag_TFT_showHEX(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6)
{
    OFlag_TFT(mode, 0x40, data1 * 16 + data2, data3 * 16 + data4, data5 * 16 + data6);
    delay_ms(200);
    OFlag_TFT(mode, 0x40, data1 * 16 + data2, data3 * 16 + data4, data5 * 16 + data6);
    delay_ms(200);
}

/**
 * @description: TFT标志物控制 显示距离
 * @param {uint8_t} mode 'A'.选择TFTA 'B'.选择TFTB  
 * @param {uint8_t} data 十进制数据 长度只支持三位
 * @return {*}
 */
void OFlag_TFT_jl(uint8_t mode, uint32_t data)
{
    OFlag_TFT(mode, 0x50, 0, data / 100, ((data / 10 % 10) * 16) + (data % 10));
}

/**
 * @description: 智能交通灯标志物控制
 * @param {uint8_t} mode 1.选择交通灯A 0.选择交通灯B
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_JT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = JT_cmd_A[0];                               //帧头1
    dt[1] = mode == 1 ? JT_cmd_A[1] : JT_cmd_B[1];     //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = JT_cmd_A[7];                               //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 智能交通灯标志物控制 进入识别模式随机点亮灯
 * @param {uint8_t} mode 1.选择交通灯A 0.选择交通灯B
 * @param {uint8_t} sb 0.进入识别模式 1.红色识别结果确认 2.绿色 3.黄色
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
 * @description: 智能交通灯标志物控制 状态解析
 * @param {uint8_t*} status 数据帧
 * @param {uint8_t} mode 1.智能交通灯标志物A 0.智能交通灯标志物B 
 * @return {*} 0.错误 1.成功 2.失败
 */
unsigned int OFlag_JT_status(uint8_t *status, uint8_t mode)
{
    uint8_t _mode;

    _mode = mode == 1 ? JT_cmd_A[1] : JT_cmd_B[1]; //智能交通灯标志物A或智能交通灯标志物B
    if (status[0] == JT_cmd_A[0] && status[1] == _mode && status[7] == JT_cmd_A[7])
    {
        if (status[2] == 0x01) //主指令
        {
            if (status[3] == 0x01) //副指令1
            {
                if (status[4] == 0x07) //进入识别模式成功
                {
                    return 1;
                }
                else if (status[4] == 0x08) //失败
                {
                    return 2;
                }
            }
        }
    }
    return 0;
}
/**
 * @description: 智能交通灯进入识别模式且等待识别结果 等待时长10s
 * @param {uint8_t} mode 1.灯A 0.灯B
 * @return {*}1.成功 0.失败
 */
unsigned int OFlag_JT_Wait(uint8_t mode)
{
    unsigned int t = 0, result = 1, _buffer = 0;

    OFlag_JT_cmd(mode, 0); //请求进入识别模式
    while (_buffer != 1)   //等待进入识别模式成功
    {
        _buffer = OFlag_JT_status(Zigb_Rx_Buf, mode);
        t++;
        if (t >= 20)
        {
            result = 0;
            break;
        }
        delay_ms(500);
        OFlag_JT_cmd(mode, 0); //请求进入识别模式
    }
    return result;
}

/**
 * @description: 立体车库标志物控制
 * @param {uint8_t} mode 1.选择车库A 0.选择车库B
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_CK(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = CK_cmd_A[0];                               //帧头1
    dt[1] = mode == 1 ? CK_cmd_A[1] : CK_cmd_B[1];     //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = CK_cmd_A[7];                               //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 立体车库标志物 到达指定层次
 * @param {uint8_t} mode 1.选择车库A 0.选择车库B
 * @param {uint8_t} num 选择层次1-4
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
 * @description: 立体车库标志物 请求返回车库状态信息
 * @param {uint8_t} mode 1.选择车库A 0.选择车库B
 * @param {uint8_t} num 1.请求返回当前车库层次 2.请求返回车库前后侧红外状态
 * @return {*}
 */
void OFlag_CK_statusGet(uint8_t mode, uint8_t num)
{
    OFlag_CK(mode, 0x02, num, 0x00, 0x00);
}

/**
 * @description: 立体车库标志物状态解析
 * @param {uint8_t} *status 数据帧
 * @param {uint8_t} CK 1.车库A 0.车库B
 * @param {uint8_t} mode 0.返回车库当前层次 1.前侧红外线状态 2.后侧红外线状态 3.前后红外状态
 * @return {unsigned int} 0.失败/车库当前层次1-4/红外状态1.被触发 2.未被触发
 */
unsigned int OFlag_CK_status(uint8_t *status, uint8_t CK, uint8_t mode)
{
    uint8_t _ck;
    _ck = CK == 1 ? CK_cmd_A[1] : CK_cmd_B[1]; //车库A或车库B

    if (status[0] == CK_cmd_A[0] && status[1] == _ck && status[7] == CK_cmd_A[7]) //车库A帧头 帧尾
    {
        if (status[2] == 0x03) //接收主指令
        {
            if (status[3] == 0x01 && mode == 0) //副指令1 0x01
            {
                return status[4]; //返回层次
            }
            else if (status[3] == 0x02) //副指令1 0x02
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
 * @description: 立体车库标志物到达指定层次且等待到达完毕
 * @param {uint8_t} mode 1.车库A 0.车库B
 * @param {uint8_t} num 选择层次1-4
 * @param {uint8_t} count 等待次数 1次1s+
 * @return {*}1.成功 0.失败
 */
unsigned int OFlag_CK_Wait(uint8_t mode, uint8_t num, uint8_t count)
{
    uint8_t _cmd[8] = {0x55, 0, 0x02, 0x01, 0, 0, calc_CheckSum(0x20, 0x01, 0, 0), 0xbb};

    uint8_t r = 1, t = 0;
    OFlag_CK_cmd(mode, num); //车库到num层
    delay_ms(100);
    OFlag_CK_cmd(mode, num); //车库到num层
    delay_ms(100);
    if (mode == 'A' || mode == 1)
    {
        _cmd[1] = 0x0D;
    }
    else
    {
        _cmd[1] = 0x05;
    }
    CK_flag = 0;                                 //重置接收状态
    TIM_Cmd(TIM4, ENABLE);                       //开启接收
    Agreement_Send(Agreement_mode_Zigbee, _cmd); //发送多次请求回传
    delay_ms(1);
    while (CK_flag != num) //等待回传结果
    {
        t++;
        if (t >= count) //超时跳出
        {
            r = 0; //0.返回超时标志1.正常结束
            break;
        }
        if (CK_flag == num) //接收完毕跳出
        {
            break;
        }
        delay_ms(500);
        delay_ms(500);
        Agreement_Send(Agreement_mode_Zigbee, _cmd); //发送多次请求回传
    }
    TIM_Cmd(TIM4, DISABLE);
    return r;
}

/**
 * @description: ETC标志物控制
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_ETC(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = ETC_cmd[0];                                //帧头1
    dt[1] = ETC_cmd[1];                                //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = ETC_cmd[7];                                //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: ETC标志物左右闸门控制
 * @param {uint8_t} mode 1.左侧闸门 0.右侧闸门 2.全部
 * @param {uint8_t} cmd 1.上升 0.下降 
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
 * @description: 开启等待ETC 若ETC开启则执行task_ETC();
 * @param {*}
 * @return {*}
 */

void OFlag_ETC_wait(void)
{
    OFlag_ETCflag = 1;
}

/**
 * @description: 获取闸门状态 10秒闸门自动关闭
 * @param {uint8_t*} status 欲判断的帧数据
 * @return {*} 0.失败 1.闸门为开启状态
 */
unsigned int OFlag_ETC_status(uint8_t *status)
{

    if (status[2] == 0x01) //主指令
    {
        if (status[3] == 0x01) //副指令1
        {
            if (status[4] == 0x06) //副指令2
            {
                if (status[4] == 0x00) //副指令3
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

/**
 * @description: 特殊地形标志物控制
 * @param {uint8_t} cmd 主指令 具体说明查阅智能嵌入式实训系统通信协议
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
 * @return {*}
 */
void OFlag_DX(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    uint8_t dt[8];
    dt[0] = DX_cmd[0];                                 //帧头1
    dt[1] = DX_cmd[1];                                 //帧头2
    dt[2] = cmd;                                       //主指令
    dt[3] = fcmd1;                                     //副指令1
    dt[4] = fcmd2;                                     //副指令2
    dt[5] = fcmd3;                                     //副指令3
    dt[6] = calc_CheckSum(dt[2], dt[3], dt[4], dt[5]); //校验和
    dt[7] = DX_cmd[7];                                 //帧尾
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 请求查询车辆通行状态
 * @param {*}
 * @return {*}
 */
void OFlag_DX_statusGet(void)
{
    OFlag_DX(0x10, 0x01, 0x00, 0x00);
}

/**
 * @description: 获取车辆通行状态
 * @param {uint8_t*} status 帧数据
 * @return {*} 0.失败
 * 0x31:车辆顺路通过，方向A->B    
 * 0x32:车辆顺路通过，方向B->A
 * 0x33:车辆未顺利通过
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
 * @description: 特殊地形等待获取车辆通行状态
 * @param {uint8_t} count 等待时间 单位：秒
 * @return {*} 0.失败
 * 0x31:车辆顺路通过，方向A->B    
 * 0x32:车辆顺路通过，方向B->A
 * 0x33:车辆未顺利通过
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
 * @description: 主车过特殊地形 检测到白色线再次行驶n米(参考码盘值1300)
 * @param {unsigned int} speed 行驶速度
 * @param {unsigned int} refMP 参考码盘
 * @return {*} 
 */
void OFlag_DX_carGo(unsigned int speed, unsigned int refMP)
{
    PID_Set(25, 0, 300); //上地形前快速调整车身
    while (PID_Track3(speed) != 99)
    {
        delay_ms(1);
    } //行驶到白色线停止循迹
    MasterCar_Stop();
    PID_Set_recovery();
    MasterCar_GoMP(speed, refMP);
}

/**
 * @description: 竞赛平台主车向自动评分系统上传语音编号
 * @param {uint8_t} num 语音编号
 * @return {*}
 */
void OFlag_YYupload(uint8_t num)
{
    uint8_t dt[8] = {0xaf, 0x06, 0x00, 0x02, 0x00, 0x00, 0x01, 0xbb};
    dt[2] = num;
    Agreement_Send(Agreement_mode_Zigbee, dt);
}

/**
 * @description: 主车发送Zigbee指令启动副车
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
 * @description: 处理副车请求并等待副车启动主车 超时响应
 * @param {uint8_t} count 等待时长 单位：s
 * @return {*}1.成功 0.失败
 * 
 */
uint8_t OFlag_SlaveRun_wait(uint8_t count)
{
    TaskBoard_Time(count);
    OFlag_SLAVEflag = 1; //重置副车启动主车标志
}

/**
 * @description: 主车发送Zigbee指令到副车
 * @param {uint8_t} cmd 主指令
 * -----------------------------------------------
 * 2021通信协议v2.0
 * 0x00	启动副车	   0x00       0x00   0x00
 * 0x05	车库层数	   层数（1-4） 0x00   0x00		
 * 0x07	路灯档位	   档位	    档位	档位
 * 0x08	自由路径坐标1  坐标[0]	坐标[1]	坐标[2]
 * 0x09	自由路径坐标2  坐标[3]	坐标[4]	坐标[5]
 * 0x0A	发送避让坐标   坐标	    坐标	坐标
 * 0X0B	发送入库坐标   坐标	    坐标	坐标
 * 0X0C	车头朝向       坐标	    坐标	坐标
 * 0X0D	RFID卡位置	  0xA1	   ‘A’	   ‘6’
 * 0X0E	烽火开启码1	  开启码[0]	开启码[1]	开启码[2]
 * 0X0F	烽火开启码2	  开启码[3]	开启码[4]	开启码[5]
 * -----------------------------------------------
 * @param {uint8_t} fcmd1 副指令x
 * @param {uint8_t} fcmd2
 * @param {uint8_t} fcmd3
*/
void OFlag_SlaveSendZigbee(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3)
{
    OFlag_Zigbee(0x02, cmd, fcmd1, fcmd2, fcmd3);
}

/**
 * @description: 获取主指令
 * @param {uint8_t} *status 数据帧
 * @return {*}
 */
uint8_t OFlag_GetCmd(uint8_t *status)
{
    return status[2];
}
/**
 * @description: 获取副指令1
 * @param {uint8_t} *status 数据帧
 * @return {*}
 */
uint8_t OFlag_GetCmd1(uint8_t *status)
{
    return status[3];
}
/**
 * @description: 获取副指令2
 * @param {uint8_t} *status 数据帧
 * @return {*}
 */
uint8_t OFlag_GetCmd2(uint8_t *status)
{
    return status[4];
}
/**
 * @description: 获取副指令3
 * @param {uint8_t} *status 数据帧
 * @return {*}
 */
uint8_t OFlag_GetCmd3(uint8_t *status)
{
    return status[5];
}

/**
 * @description: zigbee通用请求发送接收处理
 * @param {uint8_t} *cmd 协议包
 * @param {uint8_t} *flag 标志
 * @param {uint8_t} time 超时时长
 * @return {*} 1.成功 0.超时
 */
uint8_t OFlag_xx(uint8_t *cmd, uint8_t *flag, uint8_t time)
{
    uint8_t r = 1, t = 0;
    *flag = 0;                                  //重置接收状态
    TIM_Cmd(TIM4, ENABLE);                      //开启接收
    Agreement_Send(Agreement_mode_Zigbee, cmd); //发送多次请求回传
    //delay_ms(1);
    while (*flag == 0) //等待回传结果
    {
        t++;
        if (t >= time) //超时跳出
        {
            r = 0; //0.返回超时标志1.正常结束
            break;
        }
        if (*flag != 0) //接收完毕跳出
        {
            break;
        }
        delay_ms(500);
        delay_ms(500);
        Agreement_Send(Agreement_mode_Zigbee, cmd); //发送多次请求回传
    }
    TIM_Cmd(TIM4, DISABLE);
    return r;
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        if (Zigbee_Rx_flag) //接收到zigbee消息
        {
            Operation_Zigbee();
        }
    }
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

void Operation_Zigbee(void)
{
    switch (OFlag_check(Zigb_Rx_Buf)) //Ziebee节点反馈选择
    {
    case zNum_DZ: //道闸
        DZ_flag = OFlag_DZ_status(Zigb_Rx_Buf);
        break;
    case zNum_CK_A: //车库A
        CK_flag = OFlag_CK_status(Zigb_Rx_Buf, 1, 0);
        break;
    case zNum_CK_B: //车库B
        CK_flag = OFlag_CK_status(Zigb_Rx_Buf, 0, 0);
        break;
    case zNum_DX: //特殊地形
        DX_flag = OFlag_DX_status(Zigb_Rx_Buf);
        break;
    case zNum_YY: //语音播报标志物
        YY_flag[0] = Zigb_Rx_Buf[3];
        YY_flag[1] = Zigb_Rx_Buf[4];
        YY_flag[2] = Zigb_Rx_Buf[5];
        break;
    case zNum_MasterCar: //来自从车向主车的指令（！主车失控）
        /*if(SLAVE_flag==0)
        {
        SlaveCar_TaskRunThread(Zigb_Rx_Buf);
        }*/
        break;
    default: //未定义zigbee编号
        //Send_Debug_num(OFlag_check(Zigb_Rx_Buf));
        break;
    }

    Zigbee_Rx_flag = 0;
}

void OFlag_resetWaitFlag(void)
{
    OFlag_SLAVEflag = OFlag_ETCflag = 0;
}