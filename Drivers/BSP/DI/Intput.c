#include "./BSP/DI/Intput.h"

void DI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //  πƒ‹GPIOF ±÷”
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    // ≈‰÷√DI1 /∑ß√≈πÿŒª (PF0)
    GPIO_InitStructure.Pin = GPIO_PIN_CLOSE_SENSOR;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_PORT_CLOSE_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI2 /∑ß√≈ø™Œª (PF1)
    GPIO_InitStructure.Pin = GPIO_PIN_OPEN_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_OPEN_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI3 /‘∂≥Ãπÿ∑ß (PF2)
    GPIO_InitStructure.Pin = GPIO_PIN_ExCloseSwitch;
    HAL_GPIO_Init(GPIO_PORT_ExCloseSwitch, &GPIO_InitStructure);
	
    // ≈‰÷√DI4 (PF3)
    GPIO_InitStructure.Pin = GPIO_PIN_DI4_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_DI4_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI5 (PF4)
    GPIO_InitStructure.Pin = GPIO_PIN_DI5_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_DI5_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI6 (PF5)
    GPIO_InitStructure.Pin = GPIO_PIN_DI6_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_DI6_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI7 (PF6)
    GPIO_InitStructure.Pin = GPIO_PIN_DI7_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_DI7_SENSOR, &GPIO_InitStructure);
    
    // ≈‰÷√DI8 (PF7)
    GPIO_InitStructure.Pin = GPIO_PIN_DI8_SENSOR;
    HAL_GPIO_Init(GPIO_PORT_DI8_SENSOR, &GPIO_InitStructure);
}







