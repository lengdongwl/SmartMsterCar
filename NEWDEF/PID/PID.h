/*
 * @Description: PID循迹
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-22 11:17:17
 */
#ifndef __PID_H
#define __PID_H
void PID_reset(void);//循迹完需重置参数
float PID_Track(unsigned int speed);//普通循迹
float PID_Track2(unsigned int speed);//寻卡循迹
float PID_Track3(unsigned int speed);//寻卡循迹特殊地形(白线返回99无操作)
float PID_Track4(unsigned int speed);//寻卡读卡版(白线直行返回99不停车)
float PID_Track5(unsigned int speed);//十字路口可能存在白卡版

float PID_TrackMP(unsigned int speed);//循迹码盘版
float PID_TrackMP2(unsigned int speed);//急速调整车身
void PID_Set(float K_P,float K_I,float K_D);//PID调参
void PID_Set_recovery(void);//恢复PID_Set();前参数

#endif
