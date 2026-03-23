 #include "./BSP/LED/led.h"


void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    LED_GPIO_CLK_ENABLE();                                 
    LEDG_GPIO_CLK_ENABLE();                                
    LEDR_GPIO_CLK_ENABLE();                                
	DCDC_GPIO_CLK_ENABLE();
	
    gpio_init_struct.Pin = LED_GPIO_PIN;                   
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;           
    gpio_init_struct.Pull = GPIO_PULLUP;                   
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;         
    HAL_GPIO_Init(LED_GPIO_PORT, &gpio_init_struct);       

    gpio_init_struct.Pin = LEDG_GPIO_PIN;                  
    HAL_GPIO_Init(LEDG_GPIO_PORT, &gpio_init_struct);      
    
    gpio_init_struct.Pin = LEDR_GPIO_PIN;                  
    HAL_GPIO_Init(LEDR_GPIO_PORT, &gpio_init_struct);      
    
	gpio_init_struct.Pin = DCDC_GPIO_PIN;            
	HAL_GPIO_Init(DCDC_GPIO_PORT, &gpio_init_struct);
	
    LED(1);                                                
    LEDG(0);                                               
    LEDR(0);                                               
	DCDC(0);
}
