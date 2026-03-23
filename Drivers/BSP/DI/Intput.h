
#ifndef __BSP_DI_H
#define __BSP_DI_H	

#ifdef __cplusplus
extern "C" {
#endif

#include "./SYSTEM/sys/sys.h"

#define 			GPIO_PORT_CLOSE_SENSOR					    GPIOF       //		DI1
#define 			GPIO_PIN_CLOSE_SENSOR 					    GPIO_PIN_0 	//		阀门关位

#define 			GPIO_PORT_OPEN_SENSOR						GPIOF       //		DI2
#define 			GPIO_PIN_OPEN_SENSOR 						GPIO_PIN_1 	//		阀门开位

#define 			GPIO_PORT_ExCloseSwitch						GPIOF		//		DI3
#define 			GPIO_PIN_ExCloseSwitch 						GPIO_PIN_2  //		远程关阀

#define 			GPIO_PORT_DI4_SENSOR						GPIOF
#define 			GPIO_PIN_DI4_SENSOR 						GPIO_PIN_3  //DI4	数字输入

#define 			GPIO_PORT_DI5_SENSOR						GPIOF
#define 			GPIO_PIN_DI5_SENSOR 						GPIO_PIN_4  //DI5	数字输入

#define 			GPIO_PORT_DI6_SENSOR						GPIOF
#define 			GPIO_PIN_DI6_SENSOR 						GPIO_PIN_5  //DI6	数字输入

#define 			GPIO_PORT_DI7_SENSOR						GPIOF
#define 			GPIO_PIN_DI7_SENSOR 						GPIO_PIN_6  //DI7	数字输入
		
#define 			GPIO_PORT_DI8_SENSOR						GPIOF
#define 			GPIO_PIN_DI8_SENSOR 						GPIO_PIN_7  //DI8	数字输入

/******************************************************************************************/
/* 数字输入传感器操作函数 */



// 阀门开位 (DI2)
#define OpenSensor    HAL_GPIO_ReadPin(GPIO_PORT_OPEN_SENSOR, GPIO_PIN_OPEN_SENSOR)

// 阀门关位 (DI1)
#define CloseSensor   HAL_GPIO_ReadPin(GPIO_PORT_CLOSE_SENSOR, GPIO_PIN_CLOSE_SENSOR)
				
// 远程关阀 (DI3)
#define ExCloseSwitch   HAL_GPIO_ReadPin(GPIO_PORT_ExCloseSwitch, GPIO_PIN_ExCloseSwitch)

// 读取其他DI传感器状态
#define DI3_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI3_SENSOR, GPIO_PIN_DI3_SENSOR)
#define DI4_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI4_SENSOR, GPIO_PIN_DI4_SENSOR)
#define DI5_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI5_SENSOR, GPIO_PIN_DI5_SENSOR)
#define DI6_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI6_SENSOR, GPIO_PIN_DI6_SENSOR)
#define DI7_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI7_SENSOR, GPIO_PIN_DI7_SENSOR)
#define DI8_SENSOR()     HAL_GPIO_ReadPin(GPIO_PORT_DI8_SENSOR, GPIO_PIN_DI8_SENSOR)

/******************************************************************************************/
/* 函数声明 */
void DI_Init(void);

#ifdef __cplusplus
	}
#endif

#endif





