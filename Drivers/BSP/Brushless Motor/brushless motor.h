
#ifndef __BSP_BRUSHLESS_MOTOR_H__
#define __BSP_BRUSHLESS_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "./SYSTEM/sys/sys.h"
#include "stm32f4xx_hal.h"


///========================================================================================================================================
//电机电源控制 - PC7
#define 			BM_POWER_PORT               		GPIOC
#define     		BM_POWER_PIN                		GPIO_PIN_7
#define 			BM_POWER_ON		   				HAL_GPIO_WritePin(BM_POWER_PORT, BM_POWER_PIN, GPIO_PIN_SET)
#define 			BM_POWER_OFF			   			HAL_GPIO_WritePin(BM_POWER_PORT, BM_POWER_PIN, GPIO_PIN_RESET)
#define				BM_POWER_TOGGLE		 			HAL_GPIO_TogglePin(BM_POWER_PORT, BM_POWER_PIN)

//电机使能 - PE4
#define 			BM_EN_PORT               		GPIOE
#define     		BM_EN_PIN                		GPIO_PIN_4
#define 			BM_EN_ON		   					HAL_GPIO_WritePin(BM_EN_PORT, BM_EN_PIN, GPIO_PIN_SET)
#define 			BM_EN_OFF			   			HAL_GPIO_WritePin(BM_EN_PORT, BM_EN_PIN, GPIO_PIN_RESET)
#define				BM_EN_TOGGLE		 				HAL_GPIO_TogglePin(BM_EN_PORT, BM_EN_PIN)

//电机方向控制 - PE3
#define 			BM_DIR_PORT               		GPIOE
#define     		BM_DIR_PIN                		GPIO_PIN_3
#define 			BM_DIR_ON		   				HAL_GPIO_WritePin(BM_DIR_PORT, BM_DIR_PIN, GPIO_PIN_SET)
#define 			BM_DIR_OFF			   			HAL_GPIO_WritePin(BM_DIR_PORT, BM_DIR_PIN, GPIO_PIN_RESET)
#define				BM_DIR_TOGGLE		 			HAL_GPIO_TogglePin(BM_DIR_PORT, BM_DIR_PIN)

// 脉冲反馈引脚 - PE5
#define 			PULSE_FEEDBACK_PORT               		GPIOE
#define     		PULSE_FEEDBACK_PIN                		GPIO_PIN_5


#define  			ErrorStop()         					do { BM_POWER_ON; BM_DIR_OFF; BM_EN_OFF; } while (0)
#define  			ErrorRun()         						do { BM_POWER_ON; BM_DIR_OFF; BM_EN_ON; } while (0)

// 函数声明
void    PE5_Init(void);
void 	BM_GPIO_Init(void);
void    MotorStartup(void);
void 	MotorStop(void);

// 速度测量相关定义
#define PULSES_PER_REVOLUTION 12      // 每转脉冲数
#define SPEED_UPDATE_PERIOD   100     // 速度更新周期(ms)

// 全局变量声明
extern volatile uint32_t pulseCount;

#ifdef __cplusplus
	}
#endif

#endif




