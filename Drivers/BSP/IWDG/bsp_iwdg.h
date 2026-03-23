
#ifndef _BSP_IWDG_H__
#define _BSP_IWDG_H__

#include "sys.h"

// IWDG句柄声明
extern IWDG_HandleTypeDef hiwdg;

// 函数声明
void IWDG_Init(uint8_t prer, uint16_t rlr);  // IWDG初始化
void IWDG_Feed(void);                        // 喂狗函数

#endif
