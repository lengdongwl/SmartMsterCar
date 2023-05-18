/*
 * @Description: 主车通讯协议操作库
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-04-27 10:02:43
 * @version: 2021
 */
#ifndef __OperationFlag_H
#define __OperationFlag_H
#include "CanP_HostCom.h"

#define zNum_MasterCar 0x01 //主车
#define zNum_SlaveCar 0x02  //从车
#define zNum_DZ 0x03        //道闸标志物
#define zNum_LED 0x04       //LED显示标志物
#define zNum_CK_B 0x05      //立体车库B标志物
#define zNum_YY 0x06        //语音播报标志物
#define zNum_alarm 0x07     //烽火台警报器标志物
#define zNum_TFT_B 0x08     //TFTB显示标志物
#define zNum_light 0x09     //智能路灯标志物
#define zNum_WX 0x0A        //无线充电标志物
#define zNum_TFT_A 0x0B     //TFTA显示标志物
#define zNum_ETC 0x0C       //ETC系统标志物
#define zNum_CK_A 0x0D      //立体车库A标志物
#define zNum_JT_A 0x0E      //智能交通标志物A
#define zNum_JT_B 0x0F      //智能交通标志物B
#define zNum_DX 0x10        //特殊地形标志物
#define zNum_LT 0x11        //立体显示标志物

extern uint8_t OFlag_SLAVEflag;
extern uint8_t OFlag_ETCflag;
uint8_t OFlag_GetCmd(uint8_t *status);  //获取主指令
uint8_t OFlag_GetCmd1(uint8_t *status); //获取副指令1
uint8_t OFlag_GetCmd2(uint8_t *status); //获取副指令2
uint8_t OFlag_GetCmd3(uint8_t *status); //获取副指令3

unsigned int OFlag_check(uint8_t *data);                                                         //检测是否为Zigbee节点的包
void Agreement_Send(unsigned char mode, uint8_t *data);                                          //协议选择发送方式发送数据
void OFlag_Zigbee(uint8_t zigbee_num, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //Zigbee通讯

unsigned int OFlag_light(unsigned int gear);  //智能路灯标志物档位设置
unsigned int OFlag_light2(unsigned int gear); //智能路灯标志物档位设置（判断红外线是否发射成功版）

void OFlag_alarm(unsigned char mode);         //烽火台警报标志物 警报开启关闭操作
void OFlag_alarm_open(unsigned char *KEY);    //发送开启码开启烽火台
void OFlag_alarm_change(uint8_t *data);       //烽火台修改开启码
uint8_t OFlag_alarm_getP(uint8_t t);          //烽火台获取随机救援坐标

void OFlag_ltDisplay(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4); //立体显示标志物数据帧操作
void OFlag_ltDisplay_show(uint8_t *CP, uint8_t *coordinate);                                   //立体显示标志物车牌及坐标显示
void OFlag_ltDisplay_Text(uint8_t *str);                                                       //立体显示标志物自定义文本
void OFlag_LED(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3);                      //LED显示标志物控制
void OFlag_LED_time(uint8_t cmd);                                                              //LED显示标志物控制 计时模式
void OFlag_LED_show(uint8_t row, uint8_t data12, uint8_t data34, uint8_t data56);              //LED显示标志物控制 指定行显示指定数据
void OFlag_LED_jl(unsigned int mm);                                                            //LED显示标志物控制 显示距离模式

void OFlag_DZ(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //道闸标志物控制
void OFlag_DZ_open(uint8_t *str);                                        //道闸开启
void OFlag_DZ_close(void);                                               //道闸关闭
void OFlag_DZ_show(uint8_t *str);                                        //道闸显示车牌
unsigned int OFlag_DZ_wait(uint8_t count, uint8_t *str);                 //打开道闸且等待道闸处于打开状态
unsigned int OFlag_DZ_status(uint8_t *status);                           //道闸标志物控制 道闸状态解析

void OFlag_WX_open(void); //无线充电标志物开启 10秒后自动关闭
void OFlag_WX_close(void);//无线充电标志物关闭

void OFlag_YY(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //语音播报标志物控制
void OFlag_YY_cmd(uint8_t cmd);                                          //语音播报0.随机 1-7指定编号播报
void OFlag_YY_set(uint8_t mode, uint8_t t1, uint8_t t2, uint8_t t3);     //设置起始日期、时间、天气、温度
uint8_t *OFlag_YY_get(uint8_t mode, uint32_t count);                     //读取起始日期、时间、天气、温度
uint8_t OFlag_YY_play(uint8_t *str,uint8_t encode);                      //语音播报标志物合成指定语音文本

void OFlag_TFT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //TFT标志物控制
void OFlag_TFT_show(uint8_t mode, uint8_t *str);                                        //TFT显示车牌
void OFlag_TFT_showHEX(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6);
void OFlag_TFT_jl(uint8_t mode, uint32_t data); //TFT显示距离

void OFlag_JT(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //智能交通灯标志物控制
void OFlag_JT_cmd(uint8_t mode, uint8_t sb);                                           //智能交通灯标志物控制 进入识别模式
unsigned int OFlag_JT_status(uint8_t *status, uint8_t mode);                           //智能交通灯标志物控制 状态解析
//unsigned int OFlag_JT_Wait(uint8_t mode);                                              //智能交通灯进入识别模式且等待识别结果 等待时长10s

void OFlag_CK(uint8_t mode, uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //立体车库标志物控制
void OFlag_CK_cmd(uint8_t mode, uint8_t num);                                          //立体车库标志物 到达指定层次
uint8_t OFlag_CK_statusGet(uint8_t mode, uint8_t num, uint8_t count);                  //立体车库标志物 请求返回车库状态信息
unsigned int OFlag_CK_Wait(uint8_t mode, uint8_t num, uint8_t count);                  //立体车库标志物到达指定层次并且等待到达完毕
unsigned int OFlag_CK_status(uint8_t *status, uint8_t CK, uint8_t mode);               //立体车库标志物状态解析

void OFlag_ETC(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //ETC标志物控制
void OFlag_ETC_cmd(uint8_t mode, uint8_t cmd);                            //ETC标志物左右闸门控制
unsigned int OFlag_ETC_status(uint8_t *status);                           //获取闸门状态 10秒闸门自动关闭
void OFlag_ETC_wait(void);                                                //开启ETC检测运行

void OFlag_DX(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //特殊地形标志物控制
void OFlag_DX_statusGet(void);                                           //请求查询车辆通行状态
unsigned int OFlag_DX_status(uint8_t *status);                           //获取车辆通行状态
unsigned int OFlag_DX_wait(uint8_t count);                               //特殊地形等待获取车辆通行状态
void OFlag_DX_carGo(unsigned int speed, unsigned int refMP);             //过白色双线特殊地形
void OFlag_YYupload(uint8_t num);                                        //竞赛平台主车向自动评分系统上传语音编号

void OFlag_SlaveRun(void);                                                            //主车发送Zigbee指令启动副车
uint8_t OFlag_SlaveRun_wait(uint8_t count);                                           //处理副车请求并等待副车启动主车(有bug暂未使用)
uint8_t OFlag_SlaveRun_wait_flag(uint8_t count,uint8_t flag);
void OFlag_SlaveSendZigbee(uint8_t cmd, uint8_t fcmd1, uint8_t fcmd2, uint8_t fcmd3); //主车发送Zigbee指令到副车
#endif
