#ifndef __MAX31865_H
#define __MAX31865_H
  
#include "stm32f4xx_hal.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

// MAX31865 GPIO引脚定义 - 所有引脚都在GPIOB上，方便管理
#define MAX31865_CS_GPIO_PORT           GPIOB
#define MAX31865_CS_GPIO_PIN            GPIO_PIN_6
#define MAX31865_CS_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define MAX31865_SCK_GPIO_PORT          GPIOB
#define MAX31865_SCK_GPIO_PIN           GPIO_PIN_3
#define MAX31865_SCK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define MAX31865_MISO_GPIO_PORT         GPIOB
#define MAX31865_MISO_GPIO_PIN          GPIO_PIN_4
#define MAX31865_MISO_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define MAX31865_MOSI_GPIO_PORT         GPIOB
#define MAX31865_MOSI_GPIO_PIN          GPIO_PIN_5
#define MAX31865_MOSI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define MAX31865_DRDY_GPIO_PORT         GPIOB
#define MAX31865_DRDY_GPIO_PIN          GPIO_PIN_7
#define MAX31865_DRDY_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

// GPIO操作宏定义
#define MAX31865_CS_LOW()               HAL_GPIO_WritePin(MAX31865_CS_GPIO_PORT, MAX31865_CS_GPIO_PIN, GPIO_PIN_RESET)
#define MAX31865_CS_HIGH()              HAL_GPIO_WritePin(MAX31865_CS_GPIO_PORT, MAX31865_CS_GPIO_PIN, GPIO_PIN_SET)
#define MAX31865_DRDY_READ()            HAL_GPIO_ReadPin(MAX31865_DRDY_GPIO_PORT, MAX31865_DRDY_GPIO_PIN)

// 软件SPI GPIO操作宏
#define MAX31865_SCK_LOW()              HAL_GPIO_WritePin(MAX31865_SCK_GPIO_PORT, MAX31865_SCK_GPIO_PIN, GPIO_PIN_RESET)
#define MAX31865_SCK_HIGH()             HAL_GPIO_WritePin(MAX31865_SCK_GPIO_PORT, MAX31865_SCK_GPIO_PIN, GPIO_PIN_SET)
#define MAX31865_MOSI_LOW()             HAL_GPIO_WritePin(MAX31865_MOSI_GPIO_PORT, MAX31865_MOSI_GPIO_PIN, GPIO_PIN_RESET)
#define MAX31865_MOSI_HIGH()            HAL_GPIO_WritePin(MAX31865_MOSI_GPIO_PORT, MAX31865_MOSI_GPIO_PIN, GPIO_PIN_SET)
#define MAX31865_MISO_READ()            HAL_GPIO_ReadPin(MAX31865_MISO_GPIO_PORT, MAX31865_MISO_GPIO_PIN)

// MAX31865寄存器地址
#define MAX31865_CONFIG_REG             0x00
#define MAX31865_RTD_MSB_REG            0x01
#define MAX31865_RTD_LSB_REG            0x02
#define MAX31865_HFAULT_MSB_REG         0x03
#define MAX31865_HFAULT_LSB_REG         0x04
#define MAX31865_LFAULT_MSB_REG         0x05
#define MAX31865_LFAULT_LSB_REG         0x06
#define MAX31865_FAULT_STATUS_REG       0x07

// 配置寄存器位定义
#define MAX31865_CONFIG_BIAS            0x80
#define MAX31865_CONFIG_MODEAUTO        0x40
#define MAX31865_CONFIG_MODEOFF         0x00
#define MAX31865_CONFIG_1SHOT           0x20
#define MAX31865_CONFIG_3WIRE           0x10
#define MAX31865_CONFIG_24WIRE          0x00
#define MAX31865_CONFIG_FAULTSTAT       0x02
#define MAX31865_CONFIG_FILT50HZ        0x01
#define MAX31865_CONFIG_FILT60HZ        0x00

// RTD计算常数
#define RTD_A                           3.9083e-3
#define RTD_B                           -5.775e-7
#define RTD_C                           -4.183e-12

// 函数声明
void MAX31865_Init(void);
void MAX31865_GPIO_Init(void);
// void MAX31865_SPI_Init(void);
uint8_t MAX31865_ReadRegister(uint8_t addr);
void MAX31865_WriteRegister(uint8_t addr, uint8_t data);
uint16_t MAX31865_ReadRTD(void);
float MAX31865_CalculateTemperature(uint16_t rtd_value, float rtd_nominal, float ref_resistor);
float MAX31865_GetTemperature(void);
void MAX31865_ClearFault(void);
uint8_t MAX31865_ReadFault(void);
void MAX31865_SetConfig(uint8_t config);
void MAX31865_EnableBias(uint8_t enable);
void MAX31865_SetWires(uint8_t wires);
void MAX31865_SetFilter(uint8_t filter);

#endif /* __MAX31865_H */

