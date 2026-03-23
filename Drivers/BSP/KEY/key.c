#include 		"./BSP/KEY/key.h"

static uint8_t s_IR_KeyCode = 0;    // 红外按键值

IRD_T g_tIR;
TIM_HandleTypeDef htim1;


/*
*********************************************************************************************************
*	函 数 名: IR_GetKey
*	功能说明: 读取红外遥控器按键值
*	形    参: 无
*	返 回 值: 按键值，如果没有按键按下，则返回0
*********************************************************************************************************
*/
uint8_t IR_GetKey(void)
{
	uint8_t ret;
	
	ret = s_IR_KeyCode;
	s_IR_KeyCode = 0;
	
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: IRD_StartWork
*	功能说明: 启动TIM定时器，开始红外解码
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void IRD_StartWork(void)
{
    TIM_IC_InitTypeDef sConfigIC = {0};
    uint32_t tim_clk;

    /* 获取TIM1时钟频率，APB2分频系数=1,则TIM1时钟=PCLK2，否则TIM1时钟=PCLK2*2 */
    tim_clk = HAL_RCC_GetPCLK2Freq();
    if((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1) 
    {
        tim_clk *= 2;
    }

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = (tim_clk / 100000) - 1; // 10us
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
    {
        // Initialization Error
    }

    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        // Configuration Error
    }

    if (HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        // Start Error
    }

	g_tIR.LastCapture = 0;	
	g_tIR.Status = 0;
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* tim_icHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(tim_icHandle->Instance==TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
        IRD_GPIO_CLK_ENABLE();

        GPIO_InitStruct.Pin = IRD_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(IRD_GPIO_PORT, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    }
}

/*
*********************************************************************************************************
*	函 数 名: IRD_StopWork
*	功能说明: 停止红外解码
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void IRD_StopWork(void)
{
	HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_1);
}

/*
*********************************************************************************************************
*	函 数 名: IRD_DecodeNec
*	功能说明: 解码NEC格式的红外遥控信号
*	形    参: _width 脉冲宽度，单位 10us
*	返 回 值: 无
*********************************************************************************************************
*/
void IRD_DecodeNec(uint16_t _width)
{
	static uint16_t s_LowWidth;
	static uint8_t s_Byte;
	static uint8_t s_Bit;
	uint16_t TotalWitdh;
	
	/* NEC 编码 包含5个部分:
		1、引导码  9ms低 + 4.5ms高
		2、地址码8位数据  0=1.125ms  1=2.25ms    bit0先传
		3、地址反码8位数据  0=1.125ms  1=2.25ms
		4、控制码8位数据      0=1.125ms  1=2.25ms
		5、控制反码8位数据  0=1.125ms  1=2.25ms
	*/

loop1:	
	switch (g_tIR.Status)
	{
		case 0:			/* 929 等待引导码低电平  7ms - 11ms */
			if ((_width > 700) && (_width < 1100))
			{
				g_tIR.Status = 1;
				s_Byte = 0;
				s_Bit = 0;
			}
			break;

		case 1:			/* 413 等待引导码高电平  3ms - 6ms */
			if ((_width > 313) && (_width < 600))	/* 正常的引导码 4.5ms */
			{
				g_tIR.Status = 2;
			}
			else if ((_width > 150) && (_width < 250))	/* 2.25ms */
			{
				#ifdef IR_REPEAT_SEND_EN				
					if (g_tIR.RepeatCount >= IR_REPEAT_FILTER)
					{
						s_IR_KeyCode = g_tIR.RxBuf[2] + IR_KEY_STRAT;	/* 连发按键有效 */
					}
					else
					{
						g_tIR.RepeatCount++;
					}
				#endif
				g_tIR.Status = 0;	/* 回到状态0 */
			}
			else
			{
				/* 错误的码 */
				g_tIR.Status = 0;	/* 回到状态0 */
			}
			break;
		
		case 2:			/* 接收数据位 0.56ms */
			if ((_width > 10) && (_width < 100))
			{		
				g_tIR.Status = 3;
				s_LowWidth = _width;	/* 保存低电平宽度 */
			}
			else	/* 错误的码 */
			{
				/* 错误的码 */
				g_tIR.Status = 0;	/* 回到状态0 */	
				goto loop1;		/* 重新判断当前的码 */
			}
			break;

		case 3:			/* 85+25, 64+157 接收数据位高电平，总共32bit */																																																																							
			TotalWitdh = s_LowWidth + _width;
			/* 0??1.125ms??1??2.25ms */				
			s_Byte >>= 1;
			if ((TotalWitdh > 92) && (TotalWitdh < 132))
			{
				;					/* bit = 0 */
			}
			else if ((TotalWitdh > 205) && (TotalWitdh < 245))
			{
				s_Byte += 0x80;		/* bit = 1 */
			}	
			else
			{
				/* 错误的码 */
				g_tIR.Status = 0;	/* 回到状态0 */	
				goto loop1;		/* 重新判断当前的码 */
			}
			
			s_Bit++;
			if (s_Bit == 8)	
			{
				g_tIR.RxBuf[0] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 16)	
			{
				g_tIR.RxBuf[1] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 24)	
			{
				g_tIR.RxBuf[2] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 32)	
			{
				g_tIR.RxBuf[3] = s_Byte;
								
				if (g_tIR.RxBuf[2] + g_tIR.RxBuf[3] == 255)	
				{
					s_IR_KeyCode = g_tIR.RxBuf[2] + IR_KEY_STRAT;	
					g_tIR.RepeatCount = 0;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																								
				}
				
				g_tIR.Status = 0;	
				break;
			}
			g_tIR.Status = 2;	
			break;							
	}
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    uint16_t NowCapture;
    uint16_t Width;

    if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        NowCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        if (NowCapture >= g_tIR.LastCapture)
        {
            Width = NowCapture - g_tIR.LastCapture;
        }
        else
        {
            Width = ((0xFFFF - g_tIR.LastCapture) + NowCapture);
        }

        if ((g_tIR.Status == 0) && (g_tIR.LastCapture == 0))
        {
            g_tIR.LastCapture = NowCapture;
            return;
        }

        g_tIR.LastCapture = NowCapture;
        IRD_DecodeNec(Width);
    }
}

