/*
 * @Description: 基于RC522驱动 RFIDCard的操作
 * @Autor: 309
 * @Date: 2021-10-11 10:56:06
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 16:47:53

RFID读写卡说明（Mifare S50 RFID）
容量：8Kb
16个扇区  64个地址块  每个块存储16个字节 每个扇区独立的密码访问
--------------------------------------------------|
	     |块0|	扇区0 块0 不可更改	  |数据块| 0  |
  扇区0  |块1|					      |数据块| 1  |
         |块2|				    	  |数据块| 2  |
	     |块3|	密码A 控制块 密码B    |控制块| 3  |
--------------------------------------------------|
	     |块0|					      |数据块| 4  |
  扇区1  |块1|					      |数据块| 5  |
         |块2|				    	  |数据块| 6  |
	     |块3|	密码A 控制块 密码B    |控制块| 7  |
--------------------------------------------------|
。。。。。。。。。。。。。。。。。。。。。。。。。|
--------------------------------------------------|
         |块0|					      |数据块| 60 |
  扇区15 |块1|					      |数据块| 61 |
         |块2|				    	  |数据块| 62 |
	     |块3|	密码A 控制块 密码B    |控制块| 63 |
--------------------------------------------------|
RFID控制块内容：
0xff,0xff,0xff,0xff,0xff,0xff
密码A（默认密码）
0xff,0x07,0x80,0x69,0xff
存取控制 
0xff,0xff,0xff,0xff,0xff
密码B	 						
（更改密码需验证密钥后，更改需要修改的扇区块3）
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
#include "stdarg.h"//无限制参数
#define CARDTESTMODE 1 //1.输出调试信息 0.不输出调试信息
#define CARDWIDTH 200  //白色卡宽度
#define CARDHEIGHT 350 //白色卡高度
//#define white_limitMAX 300000 //白色最大阈值
//#define white_limitMIN 200000 //白色最小阈值
#define MP_DX 1300                                     //通过特殊地形的码盘值
uint8_t K_A[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 默认A密钥
uint8_t K_B[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 默认B密钥
static uint8_t buffer_data[99];                               //RC_check_read数据缓存区
static uint8_t bufferCard[20][99];                            //多个卡片存放缓冲区
void RC_Clean_buffer(void);
void RC_Clean_bufferN(void);
uint8_t TIM_PIDTrackFlag = 0;
/**
 * @description: 地址计算
 * @param {uint8_t} S 扇区
 * @param {uint8_t} Block 块 块0=第一块数据块
 * @return {*}
 */
uint8_t RC_Get_address(uint8_t S, uint8_t Block)
{
    //return S * 4 + Block-1;
    return S * 4 + Block;
}

/**
 * @description: 检测RFID卡
 * @param {*} 
 * @return {*}0.失败 1.成功
 */
uint8_t RC_check(void)
{
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
    {
        return 1;
    }
    return 0;
}

/**
 * @description: 将已检测的卡片内容读取 
 * @param {uint8_t} Block_address
 * @param {uint8_t} *KEY
 * @return {*}1.读取成功 0.读取失败
 */
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY)
{
#if 0
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
    {
        if (PcdAnticoll(SN) == MI_OK) // 取卡片序列号
        {
            if (PcdSelect(SN) == MI_OK) //选定此卡
            {

                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //验证密钥
                {
                    if (PcdRead(Block_address, buffer_data) == MI_OK) //读卡
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
#else

    if (PcdAnticoll(SN) == MI_OK) // 取卡片序列号
    {
        if (PcdSelect(SN) == MI_OK) //选定此卡
        {

            if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //验证密钥
            {
                if (PcdRead(Block_address, buffer_data) == MI_OK) //读卡
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
 * @description: 读取RC_check_read()的数据缓存
 * @param {*}
 * @return {*}
 */
uint8_t *RC_Get_buffer()
{
    return buffer_data;
}

/**
 * @description: 读取多组卡缓存的数据缓存
 * @param {*}
 * @return {*}
 */
uint8_t *RC_Get_bufferN(uint8_t position)
{
    return bufferCard[position];
}

/**
 * @description: 设置数据缓存
 * @param {*}
 * @return {*}
 */
void *RC_Set_bufferN(uint8_t position,char *data)
{
    sprintf(bufferCard[position], "%s", data); //复制
}

/**
 * @description: 清除多组卡的缓存数据
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
 * @description: 读取RFID卡 寻到卡后才可读取数据
 * 扇区=(int)(Block_address/4)
 * 扇区块密码区=扇区*4+3
 * @param {uint8_t} Block_address 块地址
 * @param {uint8_t} *KEY 密钥，无密钥则为""
 * @return {*} 成功返回数据
 */
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY)
{
    static uint8_t _buf[16];
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
    {
        if (PcdAnticoll(SN) == MI_OK) // 取卡片序列号
        {
            if (PcdSelect(SN) == MI_OK) //选定此卡
            {
                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //验证密钥
                {
                    if (PcdRead(Block_address, _buf) == MI_OK) //读卡
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
 * @description: RFID写卡
 * @param {uint8_t} Block_address　块地址
 * @param {uint8_t} *KEY　密钥，无密钥则为""
 * @param {uint8_t} *data　写入数据值
 * @return {*}
 */
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data)
{
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
    {
        if (PcdAnticoll(SN) == MI_OK) // 防冲撞成功
        {
            if (PcdSelect(SN) == MI_OK) //选定此卡
            {
                if (PcdAuthState(0x60, Block_address / 4 * 4 + 3, KEY, SN) == MI_OK) //验证扇区0的密钥A
                {
                    if (PcdWrite(Block_address, data) == MI_OK) //读卡 块地址1
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
 * @description: 检测到卡后使用 获取结果从RC_Get_buffer()获取
 * @param {uint8_t} *KEY RFID卡密钥 ""为默认
 * @return {*}0.寻到卡片退出寻卡 1.到达指定码盘值退出寻卡 2.累计读取3此失败
 */
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp)
{
    uint8_t break_flag = 0, r = 0, count = 0;
    int _mp = 0;
    MasterCar_Stop();
    TIM_Cmd(TIM5, DISABLE); //防止TIM5中断未结束干扰

    while (r == 0)
    {
        count++; //统计读取次数
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
        

    } //等待寻卡完成

    MasterCar_Stop();
    return break_flag;
}

/**
 * @description: 在指定距离内测卡
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @return {*}1.检测到卡 0.未检测到
 */
uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //目标码盘值
    while (1)
    {
        r = PID_Track3(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99)        //未达到距离LED灯全亮 则检测到卡1
        {
            card_flag = 1; //检测到卡标志
            //Send_UpMotor(0, 0);
            break;
        }
        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            card_flag = 0; //跑完码盘值未检测到卡标志
            //Send_UpMotor(0, 0);
            break;
        }
        delay_ms(1);
    }

    PID_Set_recovery(); //恢复PID_Set前数据
    return card_flag;
}

/**
 * @description: 在指定距离内测卡 PID调参版
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @return {*}1.检测到卡 0.未检测到
 */
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance, float p, float i, float d)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    PID_Set(p, i, d);
    dt += CanHost_Mp; //目标码盘值

    while (1)
    {
        r = PID_Track3(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99)        //未达到距离LED灯全亮 则检测到卡1
        {
            card_flag = 1; //检测到卡标志
            //Send_UpMotor(0, 0);
            break;
        }
        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            card_flag = 0; //跑完码盘值未检测到卡标志
            //Send_UpMotor(0, 0);
            break;
        }
        delay_ms(1);
    }

    PID_Set_recovery(); //恢复PID_Set前数据
    return card_flag;
}

/**
 * @description: 在指定距离内读卡 无论读卡成功与失败都行驶指定距离
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @return {*}1.读卡成功 0.失败  RC_Get_buffer();读取卡片数据
 */
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //目标码盘值
    while (1)
    {
        r = PID_Track4(36);            //全灭返回99 其余灯全部忽略照样直行
        if (r == 99 && card_flag != 1) //未达到距离LED灯全亮，且未读卡成功状态
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //若读卡成功返回1
            {
                card_flag = 1; //读卡成功标志
            }
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            Send_UpMotor(0, 0); //停车
            break;
        }
        delay_ms(1);
    }
    PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: 在指定距离内读N张卡 无论读卡成功与失败都行驶指定距离
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @return {*} 0.失败 >0则返回读卡成功的个数  RC_Get_bufferN();读取卡片数据
 */
uint8_t RC_Card_checkRangeReadN(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //目标码盘值
    while (1)
    {
        r = PID_Track4(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99)        //未达到距离LED灯全亮，且未读卡成功状态
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //若读卡成功返回1
            {
                sprintf(bufferCard[card_flag], "%s", RC_Get_buffer());
                card_flag++; //读卡成功标志
            }
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            Send_UpMotor(0, 0); //停车
            break;
        }
        delay_ms(1);
    }
    PID_Set_recovery();
    MasterCar_Stop();
    return card_flag;
}

/**
 * @description: 在指定距离内读卡 在RC_Card_checkRangeRead();基础上，添加末尾读卡倒退
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @return {*}1.读卡成功 0.失败
 */
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;      //读卡结果标志，循迹反馈数据

    PID_Set(25, 0, 300);
    dt += CanHost_Mp; //目标码盘值
    while (1)
    {
        r = PID_Track4(36);            //全灭返回99 其余灯全部忽略照样直行
        if (r == 99 && card_flag != 1) //未达到距离LED灯全亮，且未读卡成功状态
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed, 250);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0) //若读卡成功返回1
            {
                card_flag = 1; //读卡成功标志
            }
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            Send_UpMotor(0, 0); //停车
            break;
        }
        delay_ms(1);
    }

    if (CanHost_Mp > dt) //补偿差距
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
 * @description: 在指定距离内读卡 无论读卡成功与失败都行驶指定距离 非循迹灯版
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @param {uint8_t} n 0.无限制读取  >=1读取n张
 * @return {*}1.读卡成功 0.失败 成功读取多张的数据从RC_Get_bufferN();
 */

uint8_t RC_Card_checkRangeReadPlus(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //行驶距离

    uint8_t card_flag = 0;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //目标码盘值 CanHost_Mp超出范围变负数

    TIM_PIDTrack_Set(2);         //开启循迹线程

    while (1)
    {
        if (((card_flag < n) || n == 0) && RC_check()) //检测卡成功且未读取
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //停止
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //复制
                card_flag++;
            }

            MasterCar_GoMP(30, 200); //无论读取是否成功都往前200防止重复读取同一张卡
            TIM_PIDTrack_Set(2); //继续前进
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //关闭循迹线程

    if (CanHost_Mp > dt) //补偿差距
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
 * @description: 在指定距离内读卡 无论读卡成功与失败都行驶指定距离 循迹灯版
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @param {uint8_t} n 0.无限制读取  >=1读取n张
 * @param {uint8_t} startAdd 存放数据的起始地址 
 * @return {*}1.读卡成功 0.失败 成功读取多张的数据从RC_Get_bufferN();
 */
#define Plus_Track 1 //0.不循迹

uint8_t RC_Card_checkRangeReadPlus2(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //行驶距离

    uint8_t card_flag = 0;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //目标码盘值 CanHost_Mp超出范围变负数
    TIM_PIDTrack_Set(1); //开启循迹线程
    while (1)
    {
        if (((card_flag < n) || n == 0) && RC_check()) //检测卡成功且未读取
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //停止
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //复制
                card_flag++;
            }
            
            MasterCar_GoMP(30, 200); //无论读取是否成功都往前200防止重复读取同一张卡
            TIM_PIDTrack_Set(1); //继续循迹前进
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //关闭循迹线程

    if (CanHost_Mp > dt) //补偿差距
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
 * @description: 在指定距离内读卡 无论读卡成功与失败都行驶指定距离 循迹灯版3 读卡失败再次读取 只读取一张卡
 * @param {uint16_t} init_distance  初始距离 mp
 * @param {uint16_t} distance 范围距离 mp
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @param {uint8_t} n 再次读取次数
 * @param {uint8_t} startAdd 存放数据的起始地址 
 * @return {*}1.读卡成功 0.失败 成功读取多张的数据从RC_Get_bufferN();
 */

uint8_t RC_Card_checkRangeReadPlus3(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd)
{
    int dt = distance - init_distance; //行驶距离

    uint8_t card_flag = 0,back_flag = n;
    int count = startAdd;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    //PID_Set(25, 0, 300);

    dt += CanHost_Mp; //目标码盘值 CanHost_Mp超出范围变负数
    TIM_PIDTrack_Set(1); //开启循迹线程
    while (1)
    {
        if (((card_flag < 1)) && RC_check()) //检测卡成功且未读取
        {
            MasterCar_Stop();
            TIM_PIDTrack_Set(0); //停止前进
            MasterCar_BackMP(30,100);
            delay_ms(100);
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                sprintf(bufferCard[count++], "%s", RC_Get_buffer()); //复制
                card_flag++;
                
                MasterCar_GoMP(30, 200); //读取成功都往前200防止重复读取同一张卡
            }else
            {
                if(back_flag--)
                {
                    TIM_PIDTrack_Set(0); //停止前进
                    MasterCar_BackMP(30,200);
                }
            }
            
            TIM_PIDTrack_Set(1); //继续循迹前进
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            break;
        }
    }

    TIM_PIDTrack_Set(0); //关闭循迹线程

    if (CanHost_Mp > dt) //补偿差距
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
 * @description: 开启循迹线程
 * @param {uint8_t} flag 0.停止循迹
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
        case 1: //循迹向前
            PID_TrackMP(30);
            break;
        case 2: //向前
            MasterCar_Go(30);
            break;
        default:
            break;
        }
    }
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
}
