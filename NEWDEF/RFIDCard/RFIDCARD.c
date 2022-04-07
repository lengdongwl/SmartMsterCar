/*
 * @Description: 基于RC522驱动 RFIDCard的操作
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
#define CARDTESTMODE 1 //1.输出调试信息 0.不输出调试信息
#define CARDWIDTH 200 //白色卡宽度
#define CARDHEIGHT 350 //白色卡高度
//#define white_limitMAX 300000 //白色最大阈值
//#define white_limitMIN 200000 //白色最小阈值
#define MP_DX 1300 //通过特殊地形的码盘值 
uint8_t K_A[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 默认A密钥
uint8_t K_B[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // 默认B密钥
uint8_t buffer_data[99];                                  //RC_check_read数据缓存区
uint8_t bufferCard[20][99];                                  //多个卡片存放缓冲区
void RC_Clean_buffer(void);
void RC_Clean_bufferN(void);

/**
 * @description: 地址计算
 * @param {uint8_t} S 扇区
 * @param {uint8_t} Block 块
 * @return {*}
 */
uint8_t RC_Get_address(uint8_t S, uint8_t Block)
{
    return S * 4 + Block-1;
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
 * @description: 检测卡片并读取内容 
 * @param {uint8_t} Block_address
 * @param {uint8_t} *KEY
 * @return {*}1.读取成功 0.读取失败
 */
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY)
{
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
 * @description: 清除多组卡的缓存数据
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
 * @description: 读取RFID卡 寻到卡后才可读取数据
 * 扇区=(int)(Block_address/4)
 * 扇区块密码区=扇区*4+3
 * @param {uint8_t} Block_address 块地址
 * @param {uint8_t} *KEY 密钥，无密钥则为""
 * @return {*} 成功返回数据
 */
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY)
{
    uint8_t *_buf;
    if (PcdRequest(PICC_REQALL, CT) == MI_OK) //寻卡
    {
        if (PcdAnticoll(SN) == MI_OK) // 取卡片序列号
        {
            if (PcdSelect(SN) == MI_OK) //选定此卡
            {           
                if (PcdAuthState(0x60, (int)(Block_address / 4) * 4 + 3, KEY, SN) == MI_OK) //验证密钥
                {
                    if (PcdRead(Block_address, READ_RFID) == MI_OK) //读卡
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
 * @description: 码盘循迹寻卡 获取结果从RC_Get_buffer()获取
 * @param {uint8_t} *KEY RFID卡密钥 ""为默认
 * @return {*}1.到达指定码盘值退出寻卡 0.寻到卡片退出寻卡
 */
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp)
{
    uint8_t break_flag = 0, r = 0;
    int _mp = 0;
    Roadway_mp_syn();
    _mp = Roadway_mp_Get();
    TIM_Cmd(TIM5, DISABLE); //防止TIM5中断未结束干扰
    PID_reset();
    PID_Set(1, 0.1, 0);
    while (r == 0)
    {
        r = RC_check_read(Block_address, KEY);
        _mp = Roadway_mp_Get();
        PID_TrackMP(26); //循迹 忽略黑白线

        if (_mp >= mp)
        {
            break_flag = 1;
            break;
        }
        /*
        参考
        delay_ms(100);
		Send_UpMotor(0, 0);
        delay_ms(100);
        */
        delay_ms(150);
        Send_UpMotor(0, 0);
        delay_ms(50);
    }                   //等待寻卡完成
                        //Send_Debug_string(RC_Get_buffer());
    PID_Set_recovery(); //恢复PID_Set前数据
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
    dt+=CanHost_Mp;//目标码盘值
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
    dt+=CanHost_Mp;//目标码盘值

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
 * @description: 单端检测卡与地形(当中间为卡或者地形) 检测到立即返回结果
 * |------card or DX------|
 * @param {uint16_t} init_distance 初始距离
 * @param {uint16_t} distance 单段总距离
 * @return {*}
 */
uint8_t RC_Card_checkCardOrDX(int16_t init_distance, uint16_t distance)
{
    int dt=0,r=0,card_flag=0;
    int position_DX = distance*3/8+CanHost_Mp-init_distance;;
    PID_Set(25, 0, 300);
    dt+=distance/2+CanHost_Mp-init_distance;//目标码盘值
    while (1)
    {
        r = PID_Track3(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99)        //未达到距离LED灯全亮 则检测到卡1
        {
            if(CanHost_Mp<=position_DX)card_flag = 1; //检测到地形标志
            else card_flag = 2;//检测到卡标志
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
 * @return {*}1.读卡成功 0.失败
 */
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0;
    //PID_Set(50, 0, 0);      //设置比例码盘急速矫正车身 抖动较大
    //PID_Set(50, 0, 1 2000);      //设置比例码盘急速矫正车身 过地形还算稳定
    PID_Set(25, 0, 300);
    dt+=CanHost_Mp;//目标码盘值
    while (1)
    {
        r = PID_Track3(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99 && card_flag!=1)        //未达到距离LED灯全亮，且未读卡成功状态
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
            if(MasterCar_findCar3(Block_address,KEY,80)==0)//若读卡成功返回1
            {
                card_flag = 1; //读卡成功标志
            }
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            Send_UpMotor(0,0);//停车
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
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt = distance - init_distance; //行驶距离
    uint8_t card_flag = 0, r = 0 ;//读卡结果标志，循迹反馈数据

    PID_Set(25, 0, 300);
    dt+=CanHost_Mp;//目标码盘值
    while (1)
    {
        r = PID_Track3(36); //全灭返回99 其余灯全部忽略照样直行
        if (r == 99 && card_flag!=1)        //未达到距离LED灯全亮，且未读卡成功状态
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
            if(MasterCar_findCar3(Block_address,KEY,80)==0)//若读卡成功返回1
            {
                card_flag = 1; //读卡成功标志
            }
        }

        if (CanHost_Mp >= dt) //达到距离推出寻卡
        {
            Send_UpMotor(0,0);//停车
            break;
        }
        delay_ms(1);
    }
    
    if(CanHost_Mp>dt)//补偿差距
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
 * @description: 单段读双卡且判断读卡的位置信息  
 * @param {uint16_t} init_distance 初始码盘信息
 * @param {uint16_t} distance   码盘 (参考:短的1800，长的2250)
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥
 * @return {*}1.B2成功 2.C3读取成功
 *       中间段
 *         |
 * A1      B2      C3
 * |---|---|---|---|
 *     ^   k   ^   k
 *     r       r
 * 
起点>>>B2>>>>>>C3>>>
 */
uint8_t RC_Card_oneFunc2(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int frist_distance=distance/2;//第一次检测的距离
    int second_distance=distance/4;//第二次检测的距离
    //int dt=distance+CanHost_Mp;
    PID_Set(20,0,20);

    MasterCar_SmartRunMP(MasterCar_GoSpeed,second_distance-init_distance);//行驶到第一次开始检测的位置
    if(RC_Card_checkRangePID(0,frist_distance,20,0,20))//第一次测到卡
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
        if(MasterCar_findCar3(Block_address,KEY,80)==0)//若读卡成功返回1
        {
            MasterCar_SmartRunMP(MasterCar_GoSpeed,second_distance);//若读取成功则行驶到距离结束点
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
 * @description: 单次读卡，循迹往前读卡且分辨P1.P2
 * @param {uint16_t} distance       行驶距离(参考:短的1800，长的2250)
 * @param {uint8_t} *Block_address1 块地址P1
 * @param {uint8_t} *Block_address2 块地址P2
 * @return {*}
 * 
 * >>>>>>>>>>卡x
 * (读卡判断该块地址1是否正确 错误则后退 更换读取块地址2再次读取)
 */
uint8_t RC_Card_final_P1P2(int16_t init_distance,uint16_t distance,uint8_t Block_address1,uint8_t Block_address2)
{
    int dt = distance; //显示最终距离
    uint8_t car_exit=0; //存放是否读卡成功
    dt+=CanHost_Mp-init_distance;//目标码盘值
    
    if(RC_Card_checkRangePID(0,distance,25,0,300))
    {
        MasterCar_SmartRunMP(MasterCar_GoSpeed,250);
        if (MasterCar_findCar3(Block_address1, K_A, 80) == 0)
        {
            if(RC_Get_buffer()!="")car_exit+=1;//读P1成功标志
        }
    }
    if(car_exit!=1)//若卡P2失败则更换为P2地址
    {
        MasterCar_BackMP(MasterCar_GoSpeed,500);
        if(RC_Card_checkRangeReadBack(0,dt-CanHost_Mp,Block_address2,K_A))
        {
            if(RC_Get_buffer()!="")car_exit+=1;//读P1成功标志
        }
    }
    if(CanHost_Mp>dt)//补偿差距
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
 * @description: 循迹往前读卡且分辨P1.P2
 * @param {uint16_t} distance       行驶距离(参考:短的1800，长的2250)
 * @param {uint8_t} *Block_address1 块地址P1
 * @param {uint8_t} *Block_address2 块地址P2
 * @param {uint8_t} *KEY1           第1次读取卡的KEY
 * @param {uint8_t} *KEY2           第2次读取卡的KEY
 * @return {*}
 * 
 * >>>>>>>>>>卡x
 * (读卡判断该块地址1是否正确 错误则后退 更换读取块地址2再次读取)
 */
uint8_t RC_Card_final_P1P2_2(uint16_t distance,uint8_t Block_address1,uint8_t Block_address2,uint8_t *KEY1,uint8_t *KEY2)
{
    //不同KEY不同地址版本备用
}


/*
2020江苏省赛
  位置0     1       2
  r1       r2       r3
  |        |        |
  |--------|--------|
十字路口--中间段---十字路口
*/
/**
 * @description: 单段读三次卡，不同位置的卡地址不同
 * 注：仅限于纯寻卡不可用于有干扰的地形
 * @param {uint16_t} distance   单段距离(参考:短的1800，长的2250)
 * @param {uint8_t} *Block_address 块地址数组
 * @param {uint8_t} *KEY  卡的KEY地址必须一致
 * @return {*}
 */
type_CardRBuf RC_Card_oneFunc1(uint16_t distance,uint8_t *Block_address,uint8_t *KEY)
{
    int dt = distance; //显示最终距离
    int  position = 0, r = 0;
    type_CardRBuf  type_cardRBuf;
    int init_mp=CanHost_Mp;
    dt+=CanHost_Mp;//目标码盘值
    RC_Clean_bufferN();
    type_cardRBuf.success_count=0;

    //CanHost_Mp
    PID_Set(20, 0, 20);

    while (CanHost_Mp<dt)
    {
        r = PID_Track3(36);     //全灭返回99 其余灯全部忽略照样直行
        if(CanHost_Mp>=init_mp+distance/5)//超过1/5距离第二张卡
        {
            position=1;
        }
        if(CanHost_Mp>=init_mp+(distance/2))//超过1/2距离第三张卡
        {
            position=2;
        }
        if (r == 99 && type_cardRBuf.success_position[2] != 1) //LED灯全亮 则开始读卡 且第三张卡未读
        {
            MasterCar_SmartRunMP(36, 260); //往前行驶260mp在读卡
            if (MasterCar_findCar3(Block_address[position], KEY, 80) == 0)
            {
                sprintf((char*)bufferCard[position],"%s",(char*)RC_Get_buffer());//拷贝数据到多组卡存放的缓存区
                type_cardRBuf.success_position[position] = 1; //读卡成功标志
                type_cardRBuf.success_count+=1;//统计读卡成功的总数
                if(type_cardRBuf.success_position[2] == 1)
                {
                    
                }
            }
            else
            {     
                type_cardRBuf.success_position[position] = 0; //读卡失败标志
            }
            type_cardRBuf.total_count+=1;//统计检测到白线的次数
        }
        if ( dt<=CanHost_Mp)
        {
            break;
        }
       delay_ms(1);
    }
    if(CanHost_Mp>dt)//补偿差距
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
例
    uint8_t Block_address[]={4,4,4};//卡1，卡2，卡三读取地址
	uint8_t key[]={0x00,0x04,0x00,0x02,0x05,0x00};//KEY
	type_cardRBuf=RC_Card_oneFunc1(2250,Block_address,key);
	if(type_cardRBuf.success_count!=0)//判断是否读卡超过0张
	{
        if(type_cardRBuf.success_position[0]!=0)
        {
            //在第一个位置读到卡
        }
		else if(type_cardRBuf.success_position[1]!=0)
		{
            //在第二个位置读到卡
			Send_Debug_string(RC_Get_bufferN(1));
		}else if(type_cardRBuf.success_position[2]!=0)
		{
            //在第三个位置读到卡
			Send_Debug_string(RC_Get_bufferN(2));
		}else
		{
            //读取失败
		}
	}
*/
}


/*
                      k?       k?
                      |        |            
-------------路障---------------|
               |
            中间路段
*/
/**
 * @description: 单双段，过地形读单次卡
 * @param {uint16_t} distance   总距离(参考:短的1800，长的2250)
 * @param {uint8_t} *Block_address 块地址为坐标点
 * @param {uint8_t} *KEY  密钥
 * @return {*}
 */
type_CardRBuf RC_Card_oneFunc3(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int dt=0,frist_checkMP=0;
    type_CardRBuf  type_cardRBuf;
    dt=distance+CanHost_Mp-init_distance;//总距离
    frist_checkMP=distance*3/4-init_distance+CanHost_Mp;
    RC_Clean_bufferN();//清除缓存卡组
    type_cardRBuf.success_count=0;
    
    MasterCar_SmartRunMP(MasterCar_GoSpeed,(int)(distance/4)-init_distance);
    OFlag_DX_carGo(MasterCar_GoSpeed,MP_DX);//过特殊地形
    if(RC_Card_checkRangeRead(CanHost_Mp,frist_checkMP,Block_address,KEY))
    {
        sprintf((char*)bufferCard[0],"%s",(char*)RC_Get_buffer());//拷贝数据到多组卡存放的缓存区
        type_cardRBuf.success_position[0] = 1; //读卡1成功标志
        type_cardRBuf.success_count=1;//统计读卡成功的总数
    }
    if(RC_Card_checkRangeReadBack(CanHost_Mp,dt,Block_address,KEY))
    {
        sprintf((char*)bufferCard[1],"%s",(char*)RC_Get_buffer());//拷贝数据到多组卡存放的缓存区
        type_cardRBuf.success_position[1] = 1; //读卡1成功标志
        type_cardRBuf.success_count+=1;//统计读卡成功的总数
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
 * @description: 通过白色阈值判断是否为白卡
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

        if(Get_Host_UpTrack(TRACK_H8)==0xff)//进入白色区域
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
  ------可能有卡----                            ------可能有卡----
  |                |                           |                |
  |----------------|-------------路障-----------|----------------|
                                  |
                               中间路段
*/
/**
 * @description: 双段检测中间路障，读单卡
 * @param {uint16_t} distance 双段总距离 (参考:短的1800*2，长的2250*2)
 * @param {uint8_t} Block_address 块地址
 * @param {uint8_t} *KEY 密钥
 * @return {*}
 */
type_CardRBuf RC_Card_twoFunc1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY)
{
    int frist_checkMP=0,dt=0;
    type_CardRBuf  type_cardRBuf;

    frist_checkMP=(int)(distance/4)+250; //起始点到1/4距离为第一张卡检测区域
    dt=distance+CanHost_Mp-init_distance;//总距离
    RC_Clean_bufferN();//清除缓存卡组
    type_cardRBuf.success_count=0;

    if(RC_Card_checkRangeRead(init_distance,frist_checkMP,Block_address,KEY))
    {
        sprintf((char*)bufferCard[0],"%s",(char*)RC_Get_buffer());//拷贝数据到多组卡存放的缓存区
        type_cardRBuf.success_position[0] = 1; //读卡1成功标志
        type_cardRBuf.success_count=1;//统计读卡成功的总数
    }
    OFlag_DX_carGo(MasterCar_GoSpeed,MP_DX);//过特殊地形
    if(RC_Card_checkRangeReadBack(0,dt-CanHost_Mp,Block_address,KEY))
    {
        sprintf((char*)bufferCard[1],"%s",(char*)RC_Get_buffer());//拷贝数据到多组卡存放的缓存区
        type_cardRBuf.success_position[1] = 1; //读卡1成功标志
        type_cardRBuf.success_count+=1;//统计读卡成功的总数
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
例
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
 * @description: 在指定码盘范围读N张卡 
 * @param {uint16_t} distance   码盘
 * @param {uint8_t}  Block_address 数据块地址
 * @param {uint8_t} *KEY RFID卡密钥 ""为默认
 * @param {uint8_t} count 读取卡成功次数 读出成功超过后所循迹白线一律不处理
 * @return {*}
 */
type_CardRBuf RC_Card_ALLFun1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY,uint8_t count)
{
    int dt = distance; //显示最终距离
    int  i = 0, r = 0;
    type_CardRBuf  type_cardRBuf;
    type_cardRBuf.total_count=0;//记录读取次数
    type_cardRBuf.success_count=0;//记录成功次数
    RC_Clean_bufferN();//清除缓存
    dt+=CanHost_Mp-init_distance;//目标码盘值
    PID_Set(25, 0, 300);
    //PID_Set(10, 0, 0);

    while (CanHost_Mp<dt)
    {
        r = PID_Track3(36);     //全灭返回99 其余灯全部忽略照样直行
        if (r == 99 && type_cardRBuf.success_count<count) //LED灯全亮 则开始读卡
        {
            MasterCar_SmartRunMP(36, 250); //往前行驶250mp在读卡
            if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
            {
                if(RC_Get_buffer()!="")//即使读卡成功数据为空 也认定失败
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
    if(CanHost_Mp>dt)//补偿差距
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

/*                                卡
                                  or                            停止点 无论是否有卡都不操作
  |----------------|-------------路障-----------|----------------|
                                  |
                               中间路段
*/
/**
 * @description: 单段检测中间位置为卡或路障 
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
    dt+=distance/2+CanHost_Mp-init_distance;//1/2距离
    finaldt=distance+CanHost_Mp-init_distance;//最终距离
    while (1)
    {

        if (PID_Track3(36) == 99) //未达到距离LED灯全亮 则检测到卡1
        {
            if(CanHost_Mp<=position_DX)card_flag = 1; //检测到地形标志
            else card_flag = 2;//检测到卡标志
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
    if(card_flag==1)
    {
        MasterCar_GoMP(MasterCar_GoSpeed ,1300);
    }else if(card_flag==2)
    {
        MasterCar_SmartRunMP(36, 250); //往前行驶250mp在读卡
        if (MasterCar_findCar3(Block_address, KEY, 80) == 0)
        {
            if(RC_Get_buffer()!="")//即使读卡成功数据为空 也认定失败
            {
                card_flag=2;
            }else
            {
                card_flag=0;
            }
        }
    }
    PID_Set_recovery(); //恢复PID_Set前数据
     if(CanHost_Mp>finaldt)//补偿差距
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