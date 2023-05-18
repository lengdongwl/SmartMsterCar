/*
 * @Description: 
 * @Autor: 309 Mushroom
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-10-29 14:18:48
 */
#ifndef __FUNCTION_H
#define __FUNCTION_H
#include "sys.h"

#define east ((uint8_t)0x01)  //东
#define south ((uint8_t)0x02) //南
#define west ((uint8_t)0x03)  //西
#define north ((uint8_t)0x04) //北

/*自由路径坐标*/
#define B1 ((uint8_t)0xB1)
#define B2 ((uint8_t)0xB2)
#define B4 ((uint8_t)0xB4)
#define B6 ((uint8_t)0xB6)
#define B7 ((uint8_t)0xB7)

#define D1 ((uint8_t)0xD1)
#define D2 ((uint8_t)0xD2)
#define D4 ((uint8_t)0xD4)
#define D6 ((uint8_t)0xD6)
#define D7 ((uint8_t)0xD7)

#define F1 ((uint8_t)0xF1)
#define F2 ((uint8_t)0xF2)
#define F4 ((uint8_t)0xF4)
#define F6 ((uint8_t)0xF6)
#define F7 ((uint8_t)0xF7)
/***********************************/
extern uint8_t Two_car[16];      //读两张卡标志位
extern uint8_t make_Yes;         //全自动标志位
extern uint16_t TIM3_Flag;       //定时器3标志位
extern uint8_t Read_Car_Address; //读卡的块地址
extern uint8_t KEY_Open[6];      //RFID卡

void SYN_Play(char *Pst);                                                  //调试语音控制
uint8_t Dimming(uint8_t target);                                           //调光
uint16_t Stadiometry(void);                                                //测距
void ETC_detection(void);                                                  //ETC检测
void Eliminate_Flag(void);                                                 //清除wifi/zigbee接收完成标志位
uint8_t Light_djustment(uint8_t Sun_gear);                                 //自动调光
uint16_t measure_display(void);                                            //超声波测距
void Hardware_Init(void);                                                  //硬件初始化
void MP_Tracking(uint8_t sp, u16 mp);                                      //全自动码盘循迹
void Full_Tracking(uint8_t sp);                                            //全自动循迹
void Full_STOP(void);                                                      //停下来
void Full_RL_little(uint8_t RL, uint8_t JY);                               //微调位置
void Full_adjust(void);                                                    //转弯后微调
void Full_Left(u8 sp);                                                     //全自动左转
void Left(u8 sp);                                                          //左转
void Full_Right(u8 sp);                                                    //全自动右转
void Right(u8 sp);                                                         //右转
void Free_path(uint8_t start, uint8_t finish, uint8_t direction);          //自由路径 1.0A
void Free_path2(uint8_t start, uint8_t finish, uint8_t direction);         //自由路径 1.0B
void Head_on_adjustment(uint8_t direction, uint8_t need_direction);        //车头朝向调整
void Directional_X(int difference_X, int difference_Y, uint8_t direction); //坐标差值车头朝向调整（X轴优先）
void Directional_Y(int difference_X, int difference_Y, uint8_t direction); //坐标差值车头朝向调整（X轴优先）
void TIM3_Int_Init(u16 arr, u16 psc);                                      //定时器3初始化
void RFID_terrain_detection(uint16_t distance);                            //RFID检测-地形检测
void Digital_tube_TEXT(void);                                              //任务板测试
void RFID_detection_motion(void);                                          //RFID微动检测
uint16_t Full_Measure(void);                                               //多次测距函数
void Road_Test(uint16_t distance);                                         //路况检测(两段)
void Read_Car_single(uint16_t distance);                                   //单段读卡
void voice(uint16_t number, char *unt);                                    //语音播报数字加单位
void Running_indicator_light(void);                                        //运行指示灯
void Battery_power_detection(void);                                        //电池电量检测
void Into_the_garage(void);                                                //让车库达到第一层
void Garage(uint8_t Tier);                                                 //车库上升层
void Beacon_change_password(uint8_t PassWord[6]);                          //烽火台改密码
void Voice_control(void);                                                  //小创语言识控制

void Right_hand_bend(u8 sp);
void Left_hand_bend(u8 sp);

void Right_MP(u8 sp);
void Left_MP(u8 sp);

#endif
