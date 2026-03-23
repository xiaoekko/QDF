#include "./BSP/DO/Output.h"

void DO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    
    // ≈‰÷√DO3 ºÃµÁ∆˜1
    GPIO_InitStructure.Pin = GPIO_PIN_DO3_RELAY;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_PORT_DO3_RELAY, &GPIO_InitStructure);
    
    // ≈‰÷√DO4 ºÃµÁ∆˜2
    GPIO_InitStructure.Pin = GPIO_PIN_DO4_RELAY;
    HAL_GPIO_Init(GPIO_PORT_DO4_RELAY, &GPIO_InitStructure);
    

}








