/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-10-15 09:42:21
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-11-22 20:15:05
 */
#ifndef __SYN7318_H
#define __SYN7318_H
#include "stm32f4xx.h"
#include "sys.h"

#define SYN7318RESET PBout(5)

void SYN7318_Init(void);
void SYN7318_Play(char *Pst);//语音合成播放函数
void SYN7318_Play_NotAscll(char *Pst);//语音合成播放函数 过滤非中文字符
void SYN7318_Test( void);					// 开启语音测试
void SYN_7318_One_test(uint8_t mode,uint8_t num);		// mode 模式测试 1 随机指令 0 指定指令
void Yu_Yin_Asr(void);						// 语音识别处理函数

uint8_t SYN7318_Extern(void);		// 语音识别
uint32_t SYN_Extern_wait(uint8_t count);//等待SYN7318语音识别完成并上传评分

#endif

