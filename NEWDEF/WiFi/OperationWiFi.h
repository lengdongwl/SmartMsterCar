/*
 * @Description: WiFi接收处理库
 * @Autor: 309
 * @Date: 2021-10-15 16:34:03
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-03-13 22:05:54
 */
#ifndef __OperationWiFi_H
#define __OperationWiFi_H
#include "can_user.h"

#define OWifi_Number_len 0xff+1
#define Owifi_String_len 99

extern char OWifi_Number[OWifi_Number_len];  //接收多类数值
extern char Owifi_String[Owifi_String_len];  //接收字符串数据

extern uint8_t OWiFi_END;         //WIFI完成接收结束标志
extern uint8_t OWifi_CRCode[7];   //接收二维码
extern uint8_t OWifi_TFTCP[7];    //接收TFT车牌
extern uint8_t OWifi_TFTShape[6]; ////[0]:矩形,[1]:圆形,[2]:三角,[3]:菱形,[4]:五角星
extern uint8_t OWifi_alarm[7];    //接收烽火台 1.
extern uint8_t OWifi_JT;          //接收交通灯数据
extern uint8_t OWifi_TFTJTFLAG;   //接收TFT识别的交通标志2.掉头 3.右转 4.直行 5.左转 6.禁止通行 7.禁止直行

void OWiFi_Send(uint8_t instruct, uint8_t coord1, uint8_t coord2, uint8_t coord3); //主车发送数据到平板
void OWiFi_Send_test(uint8_t mode, uint8_t data1, uint8_t data2, uint8_t data3);   //向平板上传调试信息

void Operation_WiFi(void);                            // WiFi识别静态标志物、车牌、图形等结果
uint8_t OWiFi_cmd(uint8_t cmd, uint32_t time);        //wifi发送主指令识别请求
uint8_t OWiFi_cmds(uint8_t cmd, uint8_t f1,uint8_t f2,uint8_t f3,uint32_t time);
uint8_t OWiFi_QRCode1(uint32_t time);                 //请求识别静态标志物1
uint8_t OWiFi_QRCode2(uint32_t time);                 //请求识别静态标志物2
uint8_t OWiFi_TFT(uint8_t AorB, uint32_t time);       //请求识别TFT车牌、形状
uint8_t OWiFi_JTlight(uint8_t AorB);                  //请求交通灯识别 并向标志发送确认识别结果
uint8_t OWiFi_CodeCalc(uint8_t *data, uint32_t time); //请求数据压缩算法计算
uint8_t OWiFi_upAscll(char *str, uint32_t time);      //请求计算结果发送字符串
uint8_t OWiFi_toZigbee(void);                         //WiFi请求发送Zigbee消息
void Operation_WiFi_Master(uint8_t *buf);             //WIFI操控主车

#endif
