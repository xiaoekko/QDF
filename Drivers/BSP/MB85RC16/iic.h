

#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "./SYSTEM/sys/sys.h" 

// GPIO引脚定义
#define IIC_SCL_PORT        GPIOB
#define IIC_SCL_PIN         GPIO_PIN_8
#define IIC_SDA_PORT        GPIOB
#define IIC_SDA_PIN         GPIO_PIN_9

// SDA输入输出模式
#define SDA_IN()  do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = IIC_SDA_PIN; \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_PULLUP; \
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct); \
} while(0)

#define SDA_OUT() do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = IIC_SDA_PIN; \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_PULLUP; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct); \
} while(0)

// GPIO操作宏
#define IIC_SCL(x)    HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define IIC_SDA(x)    HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define read_SDA()    HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN)

// IIC所有操作函数
void IIC_Init(void);                // 初始化IIC的IO口
void IIC_Start(void);               // 发送IIC开始信号
void IIC_Stop(void);                // 发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);    // IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack); // IIC读取一个字节
uint8_t IIC_Wait_Ack(void);         // IIC等待ACK信号
void IIC_Ack(void);                 // IIC发送ACK信号
void IIC_NAck(void);                // IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr, uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif














