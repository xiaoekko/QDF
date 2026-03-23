
#include "./BSP/Brushless Motor/brushless motor.h"
#include "./BSP/LCD/lcd.h"

/**
 * @brief  初始化PE5引脚作为外部中断，用于检测脉冲反馈
 * @param  None
 * @retval None
 */
void PE5_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOE时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    // 配置PE5引脚为外部中断模式
    GPIO_InitStruct.Pin = PULSE_FEEDBACK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // 下降沿触发中断
    GPIO_InitStruct.Pull = GPIO_PULLUP;           // 上拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PULSE_FEEDBACK_PORT, &GPIO_InitStruct);
    
    // 配置NVIC中断
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief  初始化无刷电机控制相关GPIO
 * @param  None
 * @retval None
 */
void BM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    // 配置PC7 - 电机电源控制
    GPIO_InitStruct.Pin = BM_POWER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BM_POWER_PORT, &GPIO_InitStruct);
    
    // 配置PE4 - 电机使能
    GPIO_InitStruct.Pin = BM_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BM_EN_PORT, &GPIO_InitStruct);
    
    // 配置PE3 - 电机方向控制
    GPIO_InitStruct.Pin = BM_DIR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BM_DIR_PORT, &GPIO_InitStruct);
    
    // 初始化GPIO状态
    BM_POWER_OFF;
    BM_EN_OFF;
    BM_DIR_OFF;
	
	PE5_Init();
}

/**
 * @brief  启动无刷电机
 * @param  None
 * @retval None
 */

volatile uint32_t pulseCount = 0; // 脉冲计数器
volatile uint32_t lastPulseCount = 0; // 上次的脉冲计数
volatile uint32_t stallCheckTimer = 0; // 堵转检测计时器
volatile uint32_t motorStartDelay = 0; // 电机启动延时计数器
volatile uint8_t isMotorRunning = 0;   // 电机运行状态标志

volatile float currentRPM = 0.0f;      // 当前转速(RPM)
volatile uint32_t totalRevolutions = 0; // 总圈数
volatile uint32_t speedUpdateTimer = 0; // 转速更新计时器
volatile uint32_t lastSpeedPulseCount = 0; // 用于计算转速的上次脉冲数

volatile uint32_t pulseDiff = 0; // 脉冲计数器

void MotorStartup(void)
{
    if (isMotorRunning) return;

	// 重置运行相关的计数器
    lastPulseCount = pulseCount;  // 保存当前脉冲数
    stallCheckTimer = 0;
    motorStartDelay = 0;
	
    BM_POWER_ON;   // 开启电源
    
    //HAL_Delay(200); /* 延时200ms等待电源稳定 */

    BM_DIR_OFF;      // 设置方向(反转)
    BM_EN_ON;      // 使能电机
    
    //HAL_Delay(50);  /* 等待电机起转冲击 */
    lcd_recover();  /* 恢复LCD寄存器配置,防止白屏 */
	
	isMotorRunning = 1;    // 设置运行标志
}

/**
 * @brief  停止无刷电机
 * @param  None
 * @retval None
 */
void MotorStop(void)
{
    BM_POWER_OFF;   // 关闭电源
    BM_DIR_OFF;     // 设置方向
    BM_EN_OFF;      // 禁用电机
    isMotorRunning = 0;
}


/**
 * @brief  外部中断9-5处理函数
 * @param  None
 * @retval None
 */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PULSE_FEEDBACK_PIN);
}

/**
 * @brief  GPIO外部中断回调函数
 * @param  GPIO_Pin: 触发中断的GPIO引脚
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == PULSE_FEEDBACK_PIN)
    {
        pulseCount++; // 脉冲计数加1
        
        // if(pulseCount == 1000)
        // {
        //     BM_EN_OFF;  // 停止电机
        // }
    }
}

















