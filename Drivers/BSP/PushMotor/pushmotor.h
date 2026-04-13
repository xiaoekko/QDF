
#ifndef __PUSHMOTOR_H__
#define __PUSHMOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "./SYSTEM/sys/sys.h"        

// 推杆电机PWM定时器相关定义
#define PUSHMOTOR_TIM                           htim3
#define PUSHMOTOR_TIM_HANDLE                    TIM3
#define PUSHMOTOR_TIM_CLK_ENABLE()             __HAL_RCC_TIM3_CLK_ENABLE()
#define PUSHMOTOR_TIM_CLK_DISABLE()            __HAL_RCC_TIM3_CLK_DISABLE()

#define PUSHMOTOR_PWM_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define PUSHMOTOR_PWM_GPIO_PORT                GPIOC
#define PUSHMOTOR_PWM_PIN                      GPIO_PIN_8
#define PUSHMOTOR_PWM_AF                       GPIO_AF2_TIM3

#define PUSHMOTOR_TIM_IRQn                     TIM3_IRQn
#define PUSHMOTOR_TIM_IRQHandler               TIM3_IRQHandler

// 推杆电机方向控制引脚定义
#define PUSHMOTOR_CTRL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define PUSHMOTOR_CTRL_PORT                    GPIOC
#define PUSHMOTOR_CTRL_PIN                     GPIO_PIN_9
#define PUSHMOTOR_CTRL_TOGGLE()                HAL_GPIO_TogglePin(PUSHMOTOR_CTRL_PORT, PUSHMOTOR_CTRL_PIN)
#define PUSHMOTOR_CTRL_ON()                    HAL_GPIO_WritePin(PUSHMOTOR_CTRL_PORT, PUSHMOTOR_CTRL_PIN, GPIO_PIN_SET)
#define PUSHMOTOR_CTRL_OFF()                   HAL_GPIO_WritePin(PUSHMOTOR_CTRL_PORT, PUSHMOTOR_CTRL_PIN, GPIO_PIN_RESET)

// 推杆电机电源控制
#define PUSHRODMOTOR_POWER_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define PUSHRODMOTOR_POWER_PORT                GPIOA
#define PUSHRODMOTOR_POWER_PIN                 GPIO_PIN_12
#define PUSHRODMOTOR_POWER_TOGGLE()            HAL_GPIO_TogglePin(PUSHRODMOTOR_POWER_PORT, PUSHRODMOTOR_POWER_PIN)
#define PUSHRODMOTOR_POWER_ON()                HAL_GPIO_WritePin(PUSHRODMOTOR_POWER_PORT, PUSHRODMOTOR_POWER_PIN, GPIO_PIN_SET)
#define PUSHRODMOTOR_POWER_OFF()               HAL_GPIO_WritePin(PUSHRODMOTOR_POWER_PORT, PUSHRODMOTOR_POWER_PIN, GPIO_PIN_RESET)

// 电机运行宏
#define ForwardRun()    do { \
    PUSHRODMOTOR_POWER_ON(); \
    PUSHMOTOR_CTRL_OFF(); \
    HAL_TIM_PWM_Start(&PUSHMOTOR_TIM, TIM_CHANNEL_3); \
    __HAL_TIM_SET_COMPARE(&PUSHMOTOR_TIM, TIM_CHANNEL_3, 120); \
} while(0)

#define ReverseRun()    do { \
    PUSHRODMOTOR_POWER_ON(); \
    PUSHMOTOR_CTRL_ON(); \
    HAL_TIM_PWM_Start(&PUSHMOTOR_TIM, TIM_CHANNEL_3); \
    __HAL_TIM_SET_COMPARE(&PUSHMOTOR_TIM, TIM_CHANNEL_3, 120); \
} while(0)

#define KeepStop()      do { \
    __HAL_TIM_SET_COMPARE(&PUSHMOTOR_TIM, TIM_CHANNEL_3, 0); \
    HAL_TIM_PWM_Stop(&PUSHMOTOR_TIM, TIM_CHANNEL_3); \
    PUSHRODMOTOR_POWER_OFF(); \
    PUSHMOTOR_CTRL_OFF(); \
} while(0)

     

extern uint32_t PushMotroCount;
extern TIM_HandleTypeDef PUSHMOTOR_TIM;

void PushMotor_PWM_Init(uint32_t arr, uint32_t psc);

#ifdef __cplusplus
}
#endif

#endif













