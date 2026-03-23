#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "GUI.h"

#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/PRESS/press.h"
#include "./BSP/Brushless Motor/brushless motor.h"
#include "./BSP/PushMotor/pushmotor.h"
#include "./BSP/MAX31865/max31865.h"  
#include "./BSP/DI/Intput.h"
#include "./BSP/DO/Output.h"
#include "./BSP/PCF8563/pcf8563.h"
#include "./BSP/MB85RC16/mb85rc16.h"
#include "./BSP/W25QXX/w25qxx.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ADC/adc.h"

#include "keyapp.h"
#include "task.h"
#include "USART_.h"

#include "FreeModbusApp.h"
#include "mb.h"
#include "mbport.h"
#include "ota.h"

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟, 168Mhz */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2); // 配置优先级分组2
    delay_init(168);                    /* 延时初始化 */
    usart1_init(115200);                 /* 串口初始化为115200 */
    
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    GUI_Init();
	
	
    IRD_StartWork();                    /* 启动红外接收 */
    uart4_init(9600);                 		/* 压力传感器 */
    BM_GPIO_Init();                     /* 无刷电机初始化 */
    PushMotor_PWM_Init(200-1, 21-1);   /* 推拉电机初始化 */
    MAX31865_Init();					/*初始化MAX31865温度传感器*/
    DI_Init();							/*初始化DI输入*/
    DO_Init();							/*初始化DO输出*/
    PCF8563_Init();						/* 初始化PCF8563 RTC*/
    AT24CXX_Init();
    W25QXX_Init();
    ADC1_Init();
    SystemInitFunc();
	CheckSystemFirstRun();
    InitParametersFromEEPROM();
	OTA_Init();
	
	//SyncTimeToRTC();
	MotorStop();
	//KeepStop();
	
	
	
	
	
	
	
	
	
	

	qdf_task();                      /* 运行任务调度器emwin任务 */
    
    while(1);
	

}
