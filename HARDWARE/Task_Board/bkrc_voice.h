/*
 * @Description: 
 * @Autor: 309 Mushroom
 * @Date: 2021-03-29 17:16:23
 * @LastEditors: 309 Mushroom
 * @LastEditTime: 2021-12-20 20:28:03
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VOICE_H
#define __VOICE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported constants --------------------------------------------------------*/
extern uint8_t uart6_data;			// USART6 接收数据缓存
extern uint8_t uart6_flag;			// USART6 接收数据时序
extern uint8_t UART6_RxData[8];		// USART6 接收数据缓存
extern uint8_t voice_falg;			// 语音模块返回状态

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USART6_Config_Lib(void);
void SendData_USART6(uint8_t *Data, uint16_t length);
uint8_t Voice_Drive(void);
/*void YY_Play_Zigbee(char *p);		// 语音播报标志物播报指定文本信息
void YY_Comm_Zigbee(uint8_t Primary, uint8_t Secondary); // 播报语音控制命令
*/
uint8_t BKRC_Voice_Extern(uint8_t yy_mode,uint8_t n)	;	// 语音识别
void BKRC_Voice_Init(void);// 语音识别初始化
void XiaoChuang_PlaySIZE(int number);//播报数值大小个十百0-999
void XiaoChuang_PlayNUMBER(int number);//播报数值0-999

#endif /* __VOICE_H */

/************************ (C) COPYRIGHT WEN ***** END OF FILE *****************/
