#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "stm32f4xx_hal.h"
#include "os.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ========== USART1 配置定义 ========== */
#define USART_TX_GPIO_PORT              GPIOA
#define USART_TX_GPIO_PIN               GPIO_PIN_9
#define USART_TX_GPIO_AF                GPIO_AF7_USART1
#define USART_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 发送引脚时钟使能 */


#define USART_RX_GPIO_PORT              GPIOA
#define USART_RX_GPIO_PIN               GPIO_PIN_10
#define USART_RX_GPIO_AF                GPIO_AF7_USART1
#define USART_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 接收引脚时钟使能 */

#define USART_UX                        USART1
#define USART_UX_IRQn                   USART1_IRQn
#define USART_UX_IRQHandler             USART1_IRQHandler
#define USART_UX_CLK_ENABLE()           do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 时钟使能 */
#define USART_UX_CLK_DISABLE()          do{ __HAL_RCC_USART1_CLK_DISABLE(); }while(0)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ========== UART4 配置定义 ========== */
#define PRESS_RS485_USARTx                     UART4                    
#define PRESS_RS485_CLK_ENABLE()               __HAL_RCC_UART4_CLK_ENABLE() 
#define PRESS_RS485_CLK_DISABLE()              __HAL_RCC_UART4_CLK_DISABLE()

//#define PRESS_RS485_BAUDRATE                   9600                     	

/* RS485发送引脚配置 */
#define PRESS_RS485_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE() 
#define PRESS_RS485_TX_GPIO_PORT               GPIOC                   		
#define PRESS_RS485_TX_PIN                     GPIO_PIN_10             		
#define PRESS_RS485_TX_AF                      GPIO_AF8_UART4          		

/* RS485接收引脚配置 */
#define PRESS_RS485_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE() 
#define PRESS_RS485_RX_GPIO_PORT               GPIOC                    	
#define PRESS_RS485_RX_PIN                     GPIO_PIN_11              	
#define PRESS_RS485_RX_AF                      GPIO_AF8_UART4          		

/* RS485方向控制引脚配置 */
#define PRESS_RS485_DE_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE() 
#define PRESS_RS485_DE_GPIO_PORT               GPIOA                    	
#define PRESS_RS485_DE_PIN                     GPIO_PIN_15              	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ========== USART3 配置定义 (OTA) ========== */
#define OTA_USART3_CLK_ENABLE()                __HAL_RCC_USART3_CLK_ENABLE()
#define OTA_USART3_CLK_DISABLE()               __HAL_RCC_USART3_CLK_DISABLE()

#define OTA_USART3_BAUDRATE                    115200

/* USART3发送引脚配置 */
#define OTA_USART3_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define OTA_USART3_TX_GPIO_PORT                GPIOB
#define OTA_USART3_TX_PIN                      GPIO_PIN_10
#define OTA_USART3_TX_AF                       GPIO_AF7_USART3

/* USART3接收引脚配置 */
#define OTA_USART3_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define OTA_USART3_RX_GPIO_PORT                GPIOB
#define OTA_USART3_RX_PIN                      GPIO_PIN_11
#define OTA_USART3_RX_AF                       GPIO_AF7_USART3

#define OTA_USART3_IRQn                        USART3_IRQn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/* ========== 外部变量声明 ========== */

extern UART_HandleTypeDef huart1;       /* UART1句柄 */
extern UART_HandleTypeDef huart4;       /* UART4句柄 */
extern UART_HandleTypeDef huart2;       /* UART2句柄 */
extern UART_HandleTypeDef huart3;       /* UART3句柄 (OTA) */

/* FreeModbus */
extern void prvvUARTRxISR(void);
extern void prvvUARTTxReadyISR(void);


/* ========== 函数声明 ========== */
void uart4_init(uint32_t baudrate);                    		/* UART4初始化函数 */
void usart1_init(uint32_t baudrate);            /* USART1初始化函数 */
void usart2_init(uint32_t baudrate);
void usart3_init(void);                         /* USART3初始化函数 (OTA) */


extern volatile OS_TICK g_usart3_last_rx_tick;





