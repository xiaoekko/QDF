#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "os.h"
#include "USART_.h"
#include "stm32f4xx_hal.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

	
/* ----------------------- Start implementation -----------------------------*/


void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if (xRxEnable)						
				{
					__HAL_UART_ENABLE_IT(&huart2,USART_IT_RXNE);	
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				}
			else
				{
					__HAL_UART_DISABLE_IT(&huart2,USART_IT_RXNE);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
				}
	if (xTxEnable)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
					__HAL_UART_ENABLE_IT(&huart2,USART_IT_TC);
				}
			else
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
					__HAL_UART_DISABLE_IT(&huart2,USART_IT_TC);
				}
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
			huart2.Instance = USART2;
		    huart2.Init.BaudRate = ulBaudRate;
		    huart2.Init.StopBits = UART_STOPBITS_1;
		    huart2.Init.Mode = UART_MODE_TX_RX;
		    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
			huart2.Init.WordLength = UART_WORDLENGTH_8B;
		    

		    return HAL_UART_Init(&huart2) == HAL_OK ? TRUE : FALSE;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);	
	USART2->DR = ucByte;
	return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);	
	*pucByte = (USART2->DR & (uint16_t)0x00FF); 
	return TRUE;
}


static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}


static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}



void USART2_IRQHandler(void)
{
    OSIntEnter();

    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET))
    {
        prvvUARTRxISR();
    }
    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET))
    {
        prvvUARTTxReadyISR();
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);
    }

    OSIntExit();
}






