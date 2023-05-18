/*
 * @Description: 
 * @Autor: 309
 * @Date: 2021-09-28 20:59:17
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2022-10-13 15:33:36
 */
#ifndef __TIMER_H__
#define __TIMER_H__


extern volatile uint32_t global_times;
extern volatile uint32_t delay_ms_const;

void Timer_Init(uint16_t arr,uint16_t psc);
uint32_t gt_get_sub(uint32_t c);
uint32_t gt_get(void);
void Tim2_Init(uint16_t arr,uint16_t psc);
void Tim5_Init(uint16_t arr,uint16_t psc);//默认不使能
void Tim4_Init(uint16_t arr,uint16_t psc);
void Tim8_Init(uint16_t arr,uint16_t psc);//默认不使能
#endif


