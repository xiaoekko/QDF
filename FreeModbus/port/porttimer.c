/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "os.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"


/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );
TIM_HandleTypeDef htim2;
/* ----------------------- Start implementation -----------------------------*/

BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		    TIM_MasterConfigTypeDef sMasterConfig = {0};

		    htim2.Instance = TIM2;
		    htim2.Init.Prescaler = (SystemCoreClock / (20000*2)) - 1; 	// 50us记一次数
		    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		    htim2.Init.Period = usTim1Timerout50us-1;					// usTim1Timerout50us * 50即为定时器溢出时间
		    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
		    {
		        return FALSE;
		    }
		    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
		    {
		        return FALSE;
		    }
		    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
		    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
		    {
		        return FALSE;
		    }

		    return TRUE;
}

inline void vMBPortTimersEnable( )
{
		__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);//避免程序一上电就进入定时器中断
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
		__HAL_TIM_SET_COUNTER(&htim2, 0);		// 清空计数器
	    __HAL_TIM_ENABLE(&htim2);				// 使能定时器
}

inline void vMBPortTimersDisable( )
{
    __HAL_TIM_DISABLE(&htim2);				// 禁能定时器
	__HAL_TIM_SET_COUNTER(&htim2,0);
	__HAL_TIM_DISABLE_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);
}

static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired( );
}



void TIM2_IRQHandler(void)
{
    OSIntEnter();
	HAL_TIM_IRQHandler(&htim2);
    OSIntExit();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		prvvTIMERExpiredISR( );
	}
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM2_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
}




