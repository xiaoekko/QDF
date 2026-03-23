#ifndef _BSP_IR_DECODE_H
#define _BSP_IR_DECODE_H

#include "stm32f4xx_hal.h"

/* 红外遥控识别码相关定义 */
#define IR_KEY_STRAT    0x00

#define IR_REPEAT_SEND_EN		1	/* 连发功能使能 */
#define IR_REPEAT_FILTER		10	/* 连发滤波次数，108ms内连发10次以上认为有效 */

/* 红外接收头GPIO配置 */
#define IRD_GPIO_PORT                                   GPIOA
#define IRD_GPIO_PIN                                    GPIO_PIN_8
#define IRD_GPIO_CLK_ENABLE()                           __HAL_RCC_GPIOA_CLK_ENABLE()

/* 红外遥控器按键码值定义 */
#define IR_KEY_UP       (69 + IR_KEY_STRAT)     /* 上键 */
#define IR_KEY_BACK     (71 + IR_KEY_STRAT)     /* 返回键 */
#define IR_KEY_DOWN     (7 + IR_KEY_STRAT)      /* 下键 */
#define IR_KEY_ENTER    (9 + IR_KEY_STRAT)      /* 确认键 */
#define IR_KEY_OPEN     (12 + IR_KEY_STRAT)     /* 开启键 */
#define IR_KEY_CLOSE    (94 + IR_KEY_STRAT)     /* 关闭键 */
#define IR_KEY_STOP     (82 + IR_KEY_STRAT)     /* 停止键 */

/*    
    红外遥控器按键值定义
    根据实际遥控器修改
    69  :   up
    71  :   back
    7   :   down
    9   :   enter
    12  :   open
    94  :   close
    82  :   stop
*/

/*
		 key = IR_GetKey();
        if(key != 0)  // 按键按下
        {
            if(key == IR_KEY_OPEN)  // 如果是开启键
            {
                            // 执行相应操作
            }   
        }

*/


typedef struct
{
	uint16_t LastCapture;
	uint8_t Status;
	uint8_t RxBuf[4];
	uint8_t RepeatCount;
}IRD_T;

void IRD_StartWork(void);
void IRD_StopWork(void);
uint8_t IR_GetKey(void);

extern IRD_T g_tIR;
extern TIM_HandleTypeDef htim1;

#endif

