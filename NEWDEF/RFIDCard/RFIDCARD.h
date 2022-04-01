/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-11 10:56:30
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-19 20:22:45
 */
#ifndef __RFIDCARD_H
#define __RFIDCARD_H
#include "stdint.h"
extern uint8_t K_A[6]; // 默认A密钥
typedef struct 
{
    uint8_t success_count;//读卡成功的总数量
    uint8_t total_count;//读取的总次数(可用于遍历读出卡数据)
    uint8_t success_position[50];//读卡成功的位置
    /*uint8_t **key_data;//KEY数组
    uint8_t *block_address_data;//块数组*/
    //uint8_t **ref_data;//参数数组(该卡读卡成功后赋值结果)
}type_CardRBuf;//读卡返回结果类型

uint8_t RC_Card_checkCard(void);                  //通过白色阈值判断是否为白卡
uint8_t RC_Get_address(uint8_t S, uint8_t Block); //地址计算
uint8_t RC_check(void);                           //检测RFID卡
uint8_t RC_check_read(uint8_t Block_address, uint8_t *KEY);
uint8_t *RC_Get_buffer(void);                                         //find完毕缓存区
uint8_t *RC_Get_bufferN(uint8_t position);                             //find完毕缓存区 多卡
uint8_t *RC_read(uint8_t Block_address, uint8_t *KEY);            //读取RFID卡数据 寻到卡后才可读取数据
uint8_t RC_write(uint8_t Block_address, uint8_t *KEY, uint8_t *data); //向RFID卡写数据

uint8_t MasterCar_findCar(uint8_t Block_address, uint8_t *KEY);               //主车直线循迹循迹寻卡
uint8_t MasterCar_findCar2(uint8_t Block_address, uint8_t *KEY);              //主车直线循迹循迹寻卡(白线停止版)
uint8_t MasterCar_findCar3(uint8_t Block_address, uint8_t *KEY, uint32_t mp); //主车直线循迹码盘版 忽略黑白线

uint8_t RC_Card_checkRange(int16_t init_distance, uint16_t distance); //在指定距离范围内测卡
uint8_t RC_Card_checkRangePID(int16_t init_distance, uint16_t distance,float p,float i,float d);
uint8_t RC_Card_checkRangeRead(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);//在指定距离内读卡
uint8_t RC_Card_checkRangeReadBack(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);
uint8_t RC_Card_checkCardOrDX(int16_t init_distance, uint16_t distance);//指定范围内 检测中间物体为卡还是路障 若无物体则显示1/2距离

/*读卡函数 
final 有卡:读取成功退出 无卡: 行驶完距离值
one 单段
two 双段
*/
uint8_t RC_Card_final_P1P2(int16_t init_distance,uint16_t distance,uint8_t Block_address1,uint8_t Block_address2);//2020重庆省赛分辨出卡1.卡2
uint8_t RC_Card_final_P1P2_2(uint16_t distance,uint8_t Block_address1,uint8_t Block_address2,uint8_t *KEY1,uint8_t *KEY2);
type_CardRBuf RC_Card_oneFunc1(uint16_t distance,uint8_t *Block_address,uint8_t *KEY);//2020江苏省赛单段读三次卡，块地址为坐标点,可通过返回结果读取出读卡成功的位置
uint8_t RC_Card_oneFunc2(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//单段读双卡且判断读卡的位置信息
type_CardRBuf RC_Card_oneFunc3(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//单双段，过地形读单次卡
uint8_t RC_Card_oneFunc4(int16_t init_distance, uint16_t distance,uint8_t Block_address,uint8_t *KEY);

type_CardRBuf RC_Card_twoFunc1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY);//双段判断路障读卡
type_CardRBuf RC_Card_ALLFun1(int16_t init_distance,uint16_t distance,uint8_t Block_address,uint8_t *KEY,uint8_t count);

#endif
