#include "USART_.h"
#include "os.h"

#include "stm32f4xx_hal.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/PRESS/press.h"


/* ========== 全局变量定义 ========== */
UART_HandleTypeDef huart1;                      /* USART1句柄 */
UART_HandleTypeDef huart4;                      /* UART4句柄 */
UART_HandleTypeDef huart2;                      /* USART2句柄 */
UART_HandleTypeDef huart3;                      /* USART3句柄 (OTA) */

/* OTA USART3相关变量 */
#define OTA_PACKET_SIZE 1024
uint8_t  g_usart3_rx_buf[OTA_PACKET_SIZE + 16]; /* 串口3接收缓冲区 */
uint16_t g_usart3_rx_cnt = 0;                   /* 串口3接收计数 */
volatile OS_TICK g_usart3_last_rx_tick = 0;     /* 用于记录最后接收时间的tick */



/**
 * @brief       调试-USART1初始化函数
 */
void usart1_init(uint32_t baudrate)
{
    huart1.Instance = USART1;                           /* 设置为USART1 */
    huart1.Init.BaudRate = baudrate;                    /* 设置波特率 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B;        /* 数据位8位，无奇偶校验位 */
    huart1.Init.StopBits = UART_STOPBITS_1;             /* 停止位1位 */
    huart1.Init.Parity = UART_PARITY_NONE;              /* 无奇偶校验位 */
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;        /* 无硬件流控 */
    huart1.Init.Mode = UART_MODE_TX_RX;                 /* 收发模式 */
    HAL_UART_Init(&huart1);                             /* 调用HAL_UART_Init()函数初始化UART1 */
    
    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
}



/**
 * @brief       压力传感器-UART4初始化函数
 */
void uart4_init(uint32_t baudrate)
{
    huart4.Instance = UART4;                            /* 设置为UART4 */
    huart4.Init.BaudRate = baudrate;        /* 设置波特率为9600 */
    huart4.Init.WordLength = UART_WORDLENGTH_8B;        /* 数据位8位 */
    huart4.Init.StopBits = UART_STOPBITS_1;             /* 停止位1位 */
    huart4.Init.Parity = UART_PARITY_NONE;              /* 无奇偶校验 */
    huart4.Init.Mode = UART_MODE_TX_RX;                 /* 收发模式 */
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;        /* 无硬件流控 */
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;    /* 16倍过采样 */
    
    if (HAL_UART_Init(&huart4) != HAL_OK) {             /* 初始化UART4 */
        error_handler();                                 /* 初始化失败处理 */
    }  
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);       /* 使能UART4空闲中断 */
    
    press_reset_data();                                 /* 重置压力传感器数据 */
}






/**
 * @brief       UART的MSP初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              当我们调用HAL_UART_Init()时会自动调用此函数
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(huart->Instance == USART_UX)                             /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_UX_CLK_ENABLE();                                  /* USART1 时钟使能 */
        USART_TX_GPIO_CLK_ENABLE();                             /* 发送引脚时钟使能 */
        USART_RX_GPIO_CLK_ENABLE();                             /* 接收引脚时钟使能 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* TX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;          /* 复用为USART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);   /* 初始化发送引脚 */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* RX引脚 */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;          /* 复用为USART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 初始化接收引脚 */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 抢占优先级3，子优先级3 */
#endif
    }
    else if(huart->Instance == UART4)                           /* 如果是UART4，进行UART4 MSP初始化 */
    {
        __HAL_RCC_UART4_CLK_ENABLE();                           /* 使能UART4时钟 */
        __HAL_RCC_GPIOC_CLK_ENABLE();                           /* 使能GPIOC时钟 */
        __HAL_RCC_GPIOA_CLK_ENABLE();                           /* 使能GPIOA时钟 */
        
        /* UART4 GPIO配置
         * PC10 ------> UART4_TX
         * PC11 ------> UART4_RX
         */
        gpio_init_struct.Pin = GPIO_PIN_10|GPIO_PIN_11;         /* TX和RX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 超高速 */
        gpio_init_struct.Alternate = GPIO_AF8_UART4;           /* 复用为UART4 */
        HAL_GPIO_Init(GPIOC, &gpio_init_struct);               /* 初始化UART4引脚 */
        
        /* RS485方向控制引脚配置 */
        gpio_init_struct.Pin = GPIO_PIN_15;                     /* DE引脚 */
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL;                    /* 无上下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;           /* 低速 */
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);               /* 初始化DE引脚 */

        /* UART4中断配置 */
        HAL_NVIC_SetPriority(UART4_IRQn, 2, 3);                /* 抢占优先级2，子优先级3 */
        HAL_NVIC_EnableIRQ(UART4_IRQn);                        /* 使能UART4中断 */
    }
	else if(huart->Instance == USART2)                 
    {
        __HAL_RCC_USART2_CLK_ENABLE();                                                   
        __HAL_RCC_GPIOA_CLK_ENABLE();                          
        
        /* USART2 GPIO配置
         * PA2 ------> USART2_TX
         * PA3 ------> USART2_RX
         */
        gpio_init_struct.Pin = GPIO_PIN_2|GPIO_PIN_3;        
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;               
        gpio_init_struct.Pull = GPIO_PULLUP;                   
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;    
        gpio_init_struct.Alternate = GPIO_AF7_USART2;           
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);               
        
        /* 方向控制引脚配置 */
        gpio_init_struct.Pin = GPIO_PIN_1;                    
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;           
        gpio_init_struct.Pull = GPIO_PULLUP;                   
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;          
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);               

        /* USART2中断配置 */
        HAL_NVIC_SetPriority(USART2_IRQn, 2, 3);                
        HAL_NVIC_EnableIRQ(USART2_IRQn);  

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    else if(huart->Instance == USART3)                          /* 如果是USART3，进行USART3 MSP初始化 */
    {
        OTA_USART3_CLK_ENABLE();                                /* 使能USART3时钟 */
        OTA_USART3_TX_GPIO_CLK_ENABLE();                        /* 使能GPIOB时钟 */
        OTA_USART3_RX_GPIO_CLK_ENABLE();                        /* 使能GPIOB时钟 */
        
        /* USART3 GPIO配置
         * PB10 ------> USART3_TX
         * PB11 ------> USART3_RX
         */
        gpio_init_struct.Pin = OTA_USART3_TX_PIN | OTA_USART3_RX_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init_struct.Alternate = OTA_USART3_TX_AF;
        HAL_GPIO_Init(OTA_USART3_TX_GPIO_PORT, &gpio_init_struct);
        
        /* USART3中断配置 */
        HAL_NVIC_SetPriority(OTA_USART3_IRQn, 2, 2);
        HAL_NVIC_EnableIRQ(OTA_USART3_IRQn);
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief       UART MSP反初始化函数
 * @param       uartHandle: UART句柄
 * @retval      无
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    if (uartHandle->Instance == UART4) {
        __HAL_RCC_UART4_CLK_DISABLE();                         /* 失能UART4时钟 */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11);     /* 反初始化TX/RX引脚 */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);                   /* 反初始化DE引脚 */
        HAL_NVIC_DisableIRQ(UART4_IRQn);                       /* 失能UART4中断 */
    }
    else if (uartHandle->Instance == USART3) {
        OTA_USART3_CLK_DISABLE();                               /* 失能USART3时钟 */
        HAL_GPIO_DeInit(OTA_USART3_TX_GPIO_PORT, OTA_USART3_TX_PIN | OTA_USART3_RX_PIN);
        HAL_NVIC_DisableIRQ(OTA_USART3_IRQn);                   /* 失能USART3中断 */
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief       UART发送完成回调函数
 * @param       huart: UART句柄指针
 * @retval      无
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4) {
        g_press_data.uart_ready = 1;                           /* 设置UART就绪标志 */
        PRESS_RS485_DE(0);                                      /* 切换到接收模式 */
        /* 重新启动接收中断 */
        HAL_UARTEx_ReceiveToIdle_IT(&huart4, rx_buffer, RX_BUF_MAX_SIZE);
    }
}


/**
 * @brief       UART接收完成回调函数
 * @param       huart: UART句柄类型指针
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART_UX)                             /* 如果是串口1 */
    {
        if((g_usart_rx_sta & 0x8000) == 0)                      /* 接收未完成 */
        {
            if(g_usart_rx_sta & 0x4000)                         /* 接收到了0x0d */
            {
                if(g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;                         /* 接收错误,重新开始 */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;                   /* 接收完成了 */
                }
            }
            else                                                /* 还没收到0X0D */
            {
                if(g_rx_buffer[0] == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;                   /* 标记接收到0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0] ;
                    g_usart_rx_sta++;
                    if(g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;                     /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        
        HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief       UART接收事件回调函数
 * @param       huart: UART句柄指针
 * @param       size: 接收到的数据长度
 * @retval      无
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance == UART4) {
        g_press_data.rx_received_len = size;                    /* 记录接收数据长度 */
        
        /* 处理接收到的数据 */
        press_data_process(rx_buffer, size);
        
        /* 重新启动接收中断 */
        HAL_UARTEx_ReceiveToIdle_IT(&huart4, rx_buffer, RX_BUF_MAX_SIZE);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief       USART1中断服务函数
 * @param       无
 * @retval      无
 */
void USART1_IRQHandler(void)
{ 
#if SYS_SUPPORT_OS       
    OSIntEnter();    
#endif

    HAL_UART_IRQHandler(&huart1);                               

#if SYS_SUPPORT_OS                 
    OSIntExit();
#endif
}




/**
 * @brief       UART4中断服务函数
 * @param       无
 * @retval      无
 */
void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart4);                               
}


/**
 * @brief       OTA-USART3初始化函数
 */
void usart3_init(void)
{
    /* 配置USART3参数 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = OTA_USART3_BAUDRATE;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        /* 初始化错误处理 */
        while(1);
    }
    
    /* 使能接收中断 */
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    
    /* 初始化接收变量 */
    g_usart3_rx_cnt = 0;
    g_usart3_last_rx_tick = 0;
}


/**
 * @brief       USART3中断服务函数
 * @param       无
 * @retval      无
 */
void USART3_IRQHandler(void)
{
    OS_ERR err;
#if SYS_SUPPORT_OS       
    OSIntEnter();    
#endif

    if((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET))
    {
        /* 检查缓冲区是否溢出 */
        if (g_usart3_rx_cnt < sizeof(g_usart3_rx_buf))
        {
            /* 读取数据到全局缓冲区 */
            g_usart3_rx_buf[g_usart3_rx_cnt++] = (uint8_t)(huart3.Instance->DR & 0x00FF);
        }
        else
        {
            /* 缓冲区溢出，为清除RXNE标志而空读 */
            volatile uint8_t dummy_read = (uint8_t)(huart3.Instance->DR & 0x00FF);
        }
        /* 更新最后接收时间 */
        g_usart3_last_rx_tick = OSTimeGet(&err);
    }

#if SYS_SUPPORT_OS                 
    OSIntExit();
#endif
}










