/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-09-28 20:59:16
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-03-16 10:04:27
 */
#ifndef __MasterCarCotrol_H
#define __MasterCarCotrol_H

extern unsigned int MasterCar_GoMpValue; //拐弯时前进码盘值
extern unsigned int MasterCar_GoSpeed;   //主车默认循迹运行速度
void MasterCar_DebugKEY(void);                              
void MasterCar_Stop(void);                                   //主车暂停
void MasterCar_Right(unsigned int speed, unsigned int mode); //主车右转
void MasterCar_RightMP(unsigned int speed, unsigned int mp); //主车右转码盘
void MasterCar_Left(unsigned int speed, unsigned int mode);  //主车左转
void MasterCar_LeftMP(unsigned int speed, unsigned int mp);  //主车左转码盘

void MasterCar_Back(unsigned int speed);                        //主车后退
void MasterCar_Go(unsigned int speed);                          //主车前进（连续循迹用Go 转弯时用GoMP）
void MasterCar_GoMP(unsigned int speed, unsigned int mp);       //主车前进码盘值
void MasterCar_BackMP(unsigned int speed, unsigned int mp);     //主车后退码盘值
void MasterCar_SmartRun(unsigned int speed);                    //主车循迹运行
void MasterCar_SmartRun1(unsigned int speed);                   //主车循迹运行 调整白卡处于十字路口的车身
void MasterCar_SmartRun2(unsigned int speed);                   //主车循迹运行 寻任何姿势的白卡
void MasterCar_SmartRunMP(unsigned int speed, unsigned int MP); //主车循迹运行码盘值
void MasterCar_SmartRunMP2(unsigned int speed, unsigned int MP);//主车循迹运行码盘值暴力调车身
void MasterCar_BackEnter(unsigned int mp);                      //主车倒车入库
void MasterCar_TaskRunThread(void);                             //主车任务执行线程
void MasterCar_TaskReceiveThread(void);                         //主车任务接收线程
void Set_Flag_Track(unsigned int status);                       //主车循迹标志设置
void Set_Flag_Task(unsigned int task);                          //主车任务选择
unsigned char Get_Flag_Track(void);                             //主车循迹标志获取
unsigned char Get_Flag_Task(void);                              //主车任务标志获取
void SlaveCar_TaskRunThread(unsigned char *data);               //主车处理Zigbee接收副车控制指令


/*********************************************************************/
void task_first(void);	  //初始任务
void task_second(void);   //二次任务
void task_ETC(void);	  //等待ETC道闸后的任
void task_test(void);	  //测试任务
void task_wait(void);	  //等待副车启动指令
void task_waitWifi(void); //wifi启动指令
void task_RFID(void);	  //读RFID卡任务
/*********************************************************************/
#endif
