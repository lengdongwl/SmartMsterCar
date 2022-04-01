/*
 * @Description: 
 * @Autor: 309 Mushroom
 * @Date: 2021-09-22 10:55:55
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-11-03 14:49:53
 */
#ifndef __BH1750_H
#define __BH1750_H	 
#include "sys.h"
#include "NEW_Task_board.h"
#include "TaskBoard.h"
//IO操作函数
#if versions==0
#define IIC_SCL    PBout(6) //SCL 
#define IIC_SDA    PBout(7) //SDA输出	 
#define READ_SDA   PBin(7)  //SDA输入 
#define ADDR 	     PGout(15) 
#endif
#if versions==1
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PGout(15) //SDA输出	 
#define READ_SDA   PGin(15)  //SDA输入 
#define ADDR 	     PBout(7) 
#endif
#if versions==2
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	     PGout(15) 
#endif
#if versions==3
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	     PGout(15) 
#endif
#if versions==4
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	     PGout(15) 
#endif
#if versions==5
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	   PGout(15) 
#endif
#if IOversion==0
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	   PGout(15) 
#endif
#if IOversion==1
#define IIC_SCL    PBout(7) //SCL 
#define IIC_SDA    PBout(6) //SDA输出	 
#define READ_SDA   PBin(6)  //SDA输入 
#define ADDR 	   PGout(15) 
#endif
#if IOversion==2
#define IIC_SCL    PBout(6) //SCL 
#define IIC_SDA    PBout(7) //SDA输出	 
#define READ_SDA   PBin(7)  //SDA输入 
#define ADDR 	   PGout(15) 
#endif


void BH1750_Configure(void);
uint16_t Get_Bh_Value(void);
void illumination_display(void);
uint16_t Get_Bh_Value_Buffer(void);
#endif
