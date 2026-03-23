#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__	

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/*****************************************************************************
**
**		变量声明
**
*****************************************************************************/
// ADC和DMA句柄声明
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

// BAT ADC引脚和通道定义
#define BAT_ADC_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define BAT_ADC_GPIO_PORT                  GPIOB
#define BAT_ADC_PIN                        GPIO_PIN_1
#define BAT_ADC_CHANNEL                    ADC_CHANNEL_9

// POWER ADC引脚和通道定义
#define POWER_ADC_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define POWER_ADC_GPIO_PORT                GPIOB
#define POWER_ADC_PIN                      GPIO_PIN_0
#define POWER_ADC_CHANNEL                  ADC_CHANNEL_8

// BACK1 ADC引脚和通道定义
#define BACK1_ADC_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BACK1_ADC_GPIO_PORT                GPIOC
#define BACK1_ADC_PIN                      GPIO_PIN_1
#define BACK1_ADC_CHANNEL                  ADC_CHANNEL_11

// BACK2 ADC引脚和通道定义
#define BACK2_ADC_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BACK2_ADC_GPIO_PORT                GPIOC
#define BACK2_ADC_PIN                      GPIO_PIN_2
#define BACK2_ADC_CHANNEL                  ADC_CHANNEL_12

// ADC和DMA相关定义
#define VOLTAGE_ADC                        ADC1
#define VOLTAGE_ADC_CLK_ENABLE()           __HAL_RCC_ADC1_CLK_ENABLE()
#define VOLTAGE_ADC_DMA_CLK_ENABLE()       __HAL_RCC_DMA2_CLK_ENABLE()

// DMA缓冲区大小定义
#define M                       4       // 4个通道
#define N                       30      // 每个通道30个采样值

/*****************************************************************************
**
**		函数声明
**
*****************************************************************************/
extern uint16_t AD_Value[N][M];         // 存储多次ADC转换结果的数组，用于DMA传输

void ADC1_Init(void);                   // ADC1初始化
float Get_Battery_Voltage(void);        // 获取电池电压


#ifdef __cplusplus
	}
#endif

#endif











