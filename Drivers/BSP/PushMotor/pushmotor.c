#include "./BSP/PushMotor/pushmotor.h"

uint32_t PushMotroCount;
TIM_HandleTypeDef PUSHMOTOR_TIM;

// 新增状态变量
volatile uint8_t pushMotorState = 0;   // 0:停止, 1:正转, 2:反转
volatile uint16_t pushMotorSpeed = 0;  // 当前PWM值

void PushMotor_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    PUSHMOTOR_CTRL_GPIO_CLK_ENABLE();
    PUSHRODMOTOR_POWER_GPIO_CLK_ENABLE();

    // 配置推杆电机方向控制引脚
    GPIO_InitStruct.Pin = PUSHMOTOR_CTRL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PUSHMOTOR_CTRL_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(PUSHMOTOR_CTRL_PORT, PUSHMOTOR_CTRL_PIN, GPIO_PIN_RESET);

    // 配置推杆电机电源控制引脚
    GPIO_InitStruct.Pin = PUSHRODMOTOR_POWER_PIN;
    HAL_GPIO_Init(PUSHRODMOTOR_POWER_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(PUSHRODMOTOR_POWER_PORT, PUSHRODMOTOR_POWER_PIN, GPIO_PIN_RESET);
}

void PushMotor_PWM_Init(uint32_t arr, uint32_t psc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    // 使能GPIO时钟和定时器时钟
    PUSHMOTOR_PWM_GPIO_CLK_ENABLE();
    PUSHMOTOR_TIM_CLK_ENABLE();

    // 配置GPIO引脚为复用推挽输出
    GPIO_InitStruct.Pin = PUSHMOTOR_PWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = PUSHMOTOR_PWM_AF;
    HAL_GPIO_Init(PUSHMOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);

    // 配置定时器基本参数
    PUSHMOTOR_TIM.Instance = PUSHMOTOR_TIM_HANDLE;
    PUSHMOTOR_TIM.Init.Prescaler = psc;
    PUSHMOTOR_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    PUSHMOTOR_TIM.Init.Period = arr;
    PUSHMOTOR_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    PUSHMOTOR_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&PUSHMOTOR_TIM);

    // 主机配置
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&PUSHMOTOR_TIM, &sMasterConfig);

    // 配置PWM通道
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = arr;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&PUSHMOTOR_TIM, &sConfigOC, TIM_CHANNEL_3);

    // 初始化GPIO
    PushMotor_Gpio_Init();
}

