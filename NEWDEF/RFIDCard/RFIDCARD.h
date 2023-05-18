/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-11 10:56:30
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2023-03-14 15:23:47
 */
#ifndef __RFIDCARD_H
#define __RFIDCARD_H
#include "stdint.h"
extern uint8_t K_A[6]; // 默认A密钥

uint8_t RC_Card_checkCard(void);                                      //通过白色阈值判断是否为白卡 误判率太高
uint8_t RC_Get_address(uint8_t S, uint8_t Block);                     //地址计算
uint8_t RC_check(void);                                               //检测RFID卡
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY);           //将已检测的卡片内容读取
uint8_t *RC_Get_buffer(void);                                         //find完毕缓存区
uint8_t *RC_Get_bufferN(uint8_t position);                            //find完毕缓存区 多卡
void *RC_Set_bufferN(uint8_t position,char *data);                    //设置缓存数据
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY);                //读取RFID卡数据 寻到卡后才可读取数据
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data); //向RFID卡写数据

void TIM_PIDTrack_Set(uint8_t flag);
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp); //检测到卡后使用

uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance); //在指定距离范围内测卡
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance, float p, float i, float d);
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);                //在指定距离内读卡
uint8_t RC_Card_checkRangeReadN(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);               //在指定距离内读N张卡
uint8_t RC_Card_checkRangeReadPlus(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //在指定距离内读卡非循迹灯版
uint8_t RC_Card_checkRangeReadPlus2(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //在指定距离内读卡循迹灯版
uint8_t RC_Card_checkRangeReadPlus3(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY, uint8_t n,uint8_t startAdd); //在指定距离内读卡循迹灯版
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance, uint8_t Block_address, uint8_t *KEY);

#endif
