
#ifndef __BSP_DO_H
#define __BSP_DO_H	

#ifdef __cplusplus
extern "C" {
#endif

#include "./SYSTEM/sys/sys.h"

#define 			GPIO_PORT_DO3_RELAY					    GPIOC      
#define 			GPIO_PIN_DO3_RELAY 					    GPIO_PIN_6 	

#define 			GPIO_PORT_DO4_RELAY						GPIOG     
#define 			GPIO_PIN_DO4_RELAY 						GPIO_PIN_8 	


#define RELAY1(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIO_PORT_DO3_RELAY, GPIO_PIN_DO3_RELAY, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(GPIO_PORT_DO3_RELAY, GPIO_PIN_DO3_RELAY, GPIO_PIN_RESET); \
                  }while(0)       

#define RELAY2(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIO_PORT_DO4_RELAY, GPIO_PIN_DO4_RELAY, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(GPIO_PORT_DO4_RELAY, GPIO_PIN_DO4_RELAY, GPIO_PIN_RESET); \
                  }while(0) 
/******************************************************************************************/
/* º¯ÊýÉùÃ÷ */
void DO_Init(void);

#ifdef __cplusplus
	}
#endif

#endif





