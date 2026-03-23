#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/*****************************************************************************
**
**		配置宏         SPI1 
**
*****************************************************************************/
// SPI句柄声明
extern SPI_HandleTypeDef hspi1;

// SPI相关GPIO引脚宏
#define SPIFLASH_SPIx                           SPI1
#define SPIFLASH_SPI_CLK_ENABLE()              __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIFLASH_SPI_CLK_DISABLE()             __HAL_RCC_SPI1_CLK_DISABLE()

#define SPIFLASH_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIFLASH_SCK_GPIO_PORT                 GPIOA  
#define SPIFLASH_SCK_PIN                       GPIO_PIN_5     
#define SPIFLASH_SCK_AF                        GPIO_AF5_SPI1
							
#define SPIFLASH_MISO_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIFLASH_MISO_GPIO_PORT                GPIOA 
#define SPIFLASH_MISO_PIN                      GPIO_PIN_6 
#define SPIFLASH_MISO_AF                       GPIO_AF5_SPI1
								
#define SPIFLASH_MOSI_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIFLASH_MOSI_GPIO_PORT                GPIOA  
#define SPIFLASH_MOSI_PIN                      GPIO_PIN_7 
#define SPIFLASH_MOSI_AF                       GPIO_AF5_SPI1

/*****************************************************************************
**
**		结构体声明
**
*****************************************************************************/

/*****************************************************************************
**
**		函数声明
**
*****************************************************************************/
void 	SPI1_Init(void);			 // 初始化SPI1接口
void 	SPI1_SetSpeed(uint32_t SpeedSet); // 设置SPI1速度   
uint8_t 	SPI1_ReadWriteByte(uint8_t TxData);//SPI1读写一个字节

#ifdef __cplusplus
	}
#endif

#endif

/***********************************************************************************************
*************************************  文件结束  ***********************************************
***********************************************************************************************/


