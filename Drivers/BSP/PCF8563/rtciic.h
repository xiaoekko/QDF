

#ifndef __BSP_RTCIIC_H__
#define __BSP_RTCIIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "./SYSTEM/delay/delay.h"
#include <stdint.h>
#include <stdbool.h>

/* IIC状态枚举 */
typedef enum {
    IIC_OK = 0,                 // 操作成功
    IIC_ERROR_TIMEOUT,          // 超时错误
    IIC_ERROR_NACK,             // 无应答错误
    IIC_ERROR_BUS_BUSY          // 总线忙错误
} IIC_Status_t;

/* 配置参数定义 */
#define IIC_USE_DELAY_CLK           1       // 是否使用延时时钟控制
#define IIC_TIMEOUT_COUNT           250     // 超时计数值
#define IIC_DELAY_TIME_US           2       // 延时时间(微秒)

/* GPIO引脚定义 */
#define PCF8563_SCL_GPIO_CLK        __HAL_RCC_GPIOG_CLK_ENABLE
#define PCF8563_SCL_PORT            GPIOG
#define PCF8563_SCL_PIN             GPIO_PIN_6

#define PCF8563_SDA_GPIO_CLK        __HAL_RCC_GPIOG_CLK_ENABLE
#define PCF8563_SDA_PORT            GPIOG
#define PCF8563_SDA_PIN             GPIO_PIN_5

/* GPIO操作宏 */
#define PCF_IIC_SCL_HIGH()          HAL_GPIO_WritePin(PCF8563_SCL_PORT, PCF8563_SCL_PIN, GPIO_PIN_SET)
#define PCF_IIC_SCL_LOW()           HAL_GPIO_WritePin(PCF8563_SCL_PORT, PCF8563_SCL_PIN, GPIO_PIN_RESET)
#define PCF_IIC_SDA_HIGH()          HAL_GPIO_WritePin(PCF8563_SDA_PORT, PCF8563_SDA_PIN, GPIO_PIN_SET)
#define PCF_IIC_SDA_LOW()           HAL_GPIO_WritePin(PCF8563_SDA_PORT, PCF8563_SDA_PIN, GPIO_PIN_RESET)
#define PCF_IIC_SDA_READ()          HAL_GPIO_ReadPin(PCF8563_SDA_PORT, PCF8563_SDA_PIN)

/* 延时控制宏 */
#if IIC_USE_DELAY_CLK == 1
    #define IIC_DELAY()             delay_us(IIC_DELAY_TIME_US)
#else
    #define IIC_DELAY()             __NOP()
#endif

/* 函数声明 */
void IIC_GPIO_Init(void);
void IIC_GPIO_DeInit(void);
void IIC_SDA_SetInput(void);
void IIC_SDA_SetOutput(void);

IIC_Status_t PCF_IIC_Start(void);
void PCF_IIC_Stop(void);
IIC_Status_t PCF_IIC_SendAck(bool ack);
IIC_Status_t PCF_IIC_WaitAck(void);
IIC_Status_t PCF_IIC_WriteByte(uint8_t data);
uint8_t PCF_IIC_ReadByte(bool send_ack);

/* 兼容性宏定义(保持向后兼容) */
#define PCF_IIC_Ack(a)              PCF_IIC_SendAck(!(a))
#define PCF_IIC_Wait_Ack()          PCF_IIC_WaitAck()
#define PCF_IIC_Write_Byte(data)    PCF_IIC_WriteByte(data)
#define PCF_IIC_Read_Byte(ack)      PCF_IIC_ReadByte(ack)

#ifdef __cplusplus
}
#endif

#endif /* __BSP_RTCIIC_H__ */





