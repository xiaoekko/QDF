#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 引脚 定义 */

#define LED_GPIO_PORT                  GPIOA                                            //主控板LED
#define LED_GPIO_PIN                   GPIO_PIN_11
#define LED_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             

#define LEDG_GPIO_PORT                  GPIOF                                            //液晶板LED(绿色)
#define LEDG_GPIO_PIN                   GPIO_PIN_14
#define LEDG_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)             

#define LEDR_GPIO_PORT                  GPIOF                                            //液晶板LED(红色)                      
#define LEDR_GPIO_PIN                   GPIO_PIN_15
#define LEDR_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)  

#define DCDC_GPIO_PORT                  GPIOG                                            //DC-DC电源                      
#define DCDC_GPIO_PIN                   GPIO_PIN_0
#define DCDC_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

/******************************************************************************************/

/* LED端口定义 */
#define LED(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       

#define LEDG(x)   do{ x ? \
                      HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       

#define LEDR(x)   do{ x ? \
                      HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)

#define DCDC(x)   do{ x ? \
                      HAL_GPIO_WritePin(DCDC_GPIO_PORT, DCDC_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(DCDC_GPIO_PORT, DCDC_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0) 				  

/* LED取反定义 */
#define LED_TOGGLE()    do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN); }while(0)       /* LED = !LED */
#define LEDG_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDG_GPIO_PORT, LEDG_GPIO_PIN); }while(0)       /* LEDG = !LEDG */
#define LEDR_TOGGLE()    do{ HAL_GPIO_TogglePin(LEDR_GPIO_PORT, LEDR_GPIO_PIN); }while(0)       /* LEDR = !LEDR */

/******************************************************************************************/
/* 外部接口函数*/
void led_init(void);                                                                            /* 初始化 */

#endif
