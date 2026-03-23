#include "task.h"
#include "os.h"
#include "cpu.h"
#include <stddef.h>

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

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
#include "types.h"
#include "UI.h"

#include "GUI.h"
#include "../Drivers/BSP/MB85RC16/AddrConst.h"
#include "USART_.h"

#include "FreeModbusApp.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "valve.h"
#include "ota.h"
#include "./BSP/PCF8563/rtciic.h"
#include "bsp_iwdg.h"

static enumHistoryResult convert_alarm_to_history_result(enumErrorAlarm alarm)
{
    switch(alarm) {
        case Opened:
            return HistoryResultOpened;
        case Closed:
        case RemoteClose:
        case OverPressClose:
        case UnderPressClose:
        case OverTempClose:
            return HistoryResultClosed;
        case OpenError:
            return HistoryResultOpenError;
        case CloseError:
            return HistoryResultCloseError;
        case SensorError:
            return HistoryResultSensorError;
        case MotorStallError:
            return HistoryResultMotorStallError;
        default:
            return HistoryResultUnknown;
    }
}

static enumHistoryActionCause normalize_history_action_cause(enumErrorAlarm alarm)
{
    switch(alarm) {
        case RemoteClose:
            return HistoryActionRemoteClose;
        case OverPressClose:
            return HistoryActionOverPressClose;
        case UnderPressClose:
            return HistoryActionUnderPressClose;
        case OverTempClose:
            return HistoryActionOverTempClose;
        default:
            return CurrentActionCause;
    }
}


/* 任务控制块定义 */
static OS_TCB StartTaskTCB;
static OS_TCB LED_TaskTCB;
static OS_TCB Manage_TaskTCB;
static OS_TCB Key_TaskTCB;
static OS_TCB OLED_TaskTCB;
static OS_TCB Modbus_TaskTCB;
static OS_TCB OTA_TaskTCB;      // OTA升级任务控制块
static OS_TCB Press_TaskTCB;


/* 任务堆栈定义 */
static CPU_STK START_TASK_STK[TASK_STK_SIZE_MEDIUM];
static CPU_STK LED_TASK_STK[TASK_STK_SIZE_SMALL];
static CPU_STK MANAGE_TASK_STK[TASK_STK_SIZE_SMALL];
static CPU_STK KEY_TASK_STK[TASK_STK_SIZE_LARGE];
static CPU_STK OLED_TASK_STK[TASK_STK_SIZE_XLARGE];
static CPU_STK MODBUS_TASK_STK[TASK_STK_SIZE_LARGE];
static CPU_STK OTA_TASK_STK[TASK_STK_SIZE_MEDIUM];    // OTA升级任务堆栈
static CPU_STK PRESS_TASK_STK[TASK_STK_SIZE_SMALL];


/* 任务配置表 */
static const TaskConfig_t task_configs[TASK_ID_MAX] = {
    [TASK_ID_START] = {
        .tcb = &StartTaskTCB,
        .stk = START_TASK_STK,
        .task_func = start_task,
        .name = "start task",
        .prio = TASK_PRIO_START,
        .stk_size = TASK_STK_SIZE_MEDIUM
    },
    [TASK_ID_LED] = {
        .tcb = &LED_TaskTCB,
        .stk = LED_TASK_STK,
        .task_func = led_task,
        .name = "LED task",
        .prio = TASK_PRIO_LED,
        .stk_size = TASK_STK_SIZE_SMALL
    },
    [TASK_ID_MANAGE] = {
        .tcb = &Manage_TaskTCB,
        .stk = MANAGE_TASK_STK,
        .task_func = manage_task,
        .name = "Manage task",
        .prio = TASK_PRIO_MANAGE,
        .stk_size = TASK_STK_SIZE_SMALL
    },
    [TASK_ID_KEY] = {
        .tcb = &Key_TaskTCB,
        .stk = KEY_TASK_STK,
        .task_func = key_task,
        .name = "Key task",
        .prio = TASK_PRIO_KEY,
        .stk_size = TASK_STK_SIZE_LARGE
    },
    [TASK_ID_OLED] = {
        .tcb = &OLED_TaskTCB,
        .stk = OLED_TASK_STK,
        .task_func = lcd_task,
        .name = "OLED task",
        .prio = TASK_PRIO_OLED,
        .stk_size = TASK_STK_SIZE_XLARGE
    },
    [TASK_ID_MODBUS] = {
        .tcb = &Modbus_TaskTCB,
        .stk = MODBUS_TASK_STK,
        .task_func = modbus_task,
        .name = "Modbus task",
        .prio = TASK_PRIO_MODBUS,
        .stk_size = TASK_STK_SIZE_LARGE
    },
    // OTA任务配置
    [TASK_ID_OTA] = {
        .tcb = &OTA_TaskTCB,
        .stk = OTA_TASK_STK,
        .task_func = ota_task,
        .name = "OTA task",
        .prio = TASK_PRIO_OTA,
        .stk_size = TASK_STK_SIZE_MEDIUM
    },
    [TASK_ID_PRESS] = {
        .tcb = &Press_TaskTCB,
        .stk = PRESS_TASK_STK,
        .task_func = press_task,
        .name = "Press task",
        .prio = TASK_PRIO_PRESS,
        .stk_size = TASK_STK_SIZE_SMALL
    }
};

/**
 * @brief 任务错误处理函数
 * @param err 错误代码
 * @param task_name 任务名称
 */
void task_error_handler(OS_ERR err, const char *task_name) {
    if (err != OS_ERR_NONE) {
        // 发生严重错误，可在此处闪烁LED或打印日志

        while(1) {
            // 进入死循环，防止系统继续在错误状态下运行
        }
    }
}

/**
 * @brief 创建单个任务
 * @param config 任务配置结构体指针
 * @return OS_ERR 错误代码
 */
OS_ERR create_task(const TaskConfig_t *config) {
    OS_ERR err;
    
    if (config == NULL) {
        return OS_ERR_PTR_INVALID;
    }
    
    OSTaskCreate((OS_TCB *)config->tcb,
                 (CPU_CHAR *)config->name,
                 (OS_TASK_PTR)config->task_func,
                 (void *)0,
                 (OS_PRIO)config->prio,
                 (CPU_STK *)config->stk,
                 (CPU_STK_SIZE)config->stk_size / 10,
                 (CPU_STK_SIZE)config->stk_size,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    
    return err;
}

/**
 * @brief uC/OS-III 启动任务
 */
void qdf_task(void) {
    OS_ERR err;
    CPU_SR_ALLOC();
    
    OSInit(&err);
    task_error_handler(err, "OS Init");
    
    CPU_CRITICAL_ENTER();
    
    /* 创建启动任务 */
    err = create_task(&task_configs[TASK_ID_START]);
    task_error_handler(err, task_configs[TASK_ID_START].name);
    
    CPU_CRITICAL_EXIT();
    
    OSStart(&err);
    task_error_handler(err, "OS Start");
}

/**
 * @brief 启动任务函数
 * @param p_arg 参数指针
 */
void start_task(void *p_arg) {
    OS_ERR err;
    CPU_SR_ALLOC();
    
    (void)p_arg; /* 避免编译器警告 */
    
    CPU_Init();
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);
    task_error_handler(err, "Stat Task Init");
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN
    OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);
    task_error_handler(err, "Round Robin Config");
#endif
    
    CPU_CRITICAL_ENTER();
    
    /* 创建其他应用任务 */
    for (TaskID_t i = TASK_ID_LED; i < TASK_ID_MAX; i++) {
        err = create_task(&task_configs[i]);
        task_error_handler(err, task_configs[i].name);
    }
    
    CPU_CRITICAL_EXIT();
    
    /* 删除启动任务自身 */
    OSTaskDel((OS_TCB *)0, &err);
}



void Low_Power(void)       // 进入低功耗模式
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	
    lcd_display_off();          // 关闭LCD显示
	
    LCD_BL(0);                  // 关闭LCD背光
	
    DCDC(0);                    // 关闭DC-DC电源

    // GPIO配置
    GPIO_InitStructure.Pin      =       LED_GPIO_PIN;         // 主控板LED
	GPIO_InitStructure.Mode 	= 		GPIO_MODE_INPUT;  		
	GPIO_InitStructure.Pull 	= 		GPIO_PULLUP;	
	GPIO_InitStructure.Speed 	= 		GPIO_SPEED_FREQ_LOW;	
	HAL_GPIO_Init(LED_GPIO_PORT,&GPIO_InitStructure);


    GPIO_InitStructure.Pin      =       DCDC_GPIO_PIN;            // DC-DC电源
    GPIO_InitStructure.Pull     =       GPIO_PULLUP;             // 上拉
	HAL_GPIO_Init(DCDC_GPIO_PORT,&GPIO_InitStructure);

    GPIO_InitStructure.Pin      =       BM_POWER_PIN;            // 无刷电机电源
    GPIO_InitStructure.Pull     =       GPIO_PULLDOWN;           // 下拉
	HAL_GPIO_Init(BM_POWER_PORT,&GPIO_InitStructure);

    GPIO_InitStructure.Pin      =       BM_EN_PIN;               // 无刷电机使能
    GPIO_InitStructure.Pull     =       GPIO_PULLDOWN;           // 下拉
	HAL_GPIO_Init(BM_EN_PORT,&GPIO_InitStructure);
	
    GPIO_InitStructure.Pin      =       BM_DIR_PIN;              // 无刷电机方向
    GPIO_InitStructure.Pull     =       GPIO_PULLDOWN;           // 下拉
	HAL_GPIO_Init(BM_DIR_PORT,&GPIO_InitStructure);
	
 
	GPIO_InitStructure.Pin 		= 		IIC_SCL_PIN;
    GPIO_InitStructure.Pull     =       GPIO_PULLUP;             // 上拉
    HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);            // 模拟I2C GPIO

	GPIO_InitStructure.Pin 		= 		IIC_SDA_PIN;
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);            // 模拟I2C GPIO

	GPIO_InitStructure.Pin 		=  		PCF8563_SCL_PIN;		
    GPIO_InitStructure.Pull     =       GPIO_PULLUP;             // 上拉
    HAL_GPIO_Init(PCF8563_SCL_PORT, &GPIO_InitStructure);        // 模拟I2C GPIO

	GPIO_InitStructure.Pin 		=  		PCF8563_SDA_PIN;		
    HAL_GPIO_Init(PCF8563_SDA_PORT, &GPIO_InitStructure);        // 模拟I2C GPIO


    // 关闭USART
//	__HAL_UART_DISABLE(&huart1); 
//  USART_UX_CLK_DISABLE();//关闭USART时钟

//	__HAL_UART_DISABLE(&huart4); 
//  PRESS_RS485_CLK_DISABLE();//关闭USART时钟

    // 关闭DMA
	__HAL_DMA_DISABLE(&hdma_adc1);
	__HAL_RCC_DMA1_CLK_DISABLE(); 

    // 关闭ADC
	__HAL_ADC_DISABLE(&hadc1);
	__HAL_RCC_ADC1_CLK_DISABLE();
	
    // 关闭定时器
	__HAL_TIM_DISABLE(&htim3);	
	__HAL_RCC_TIM3_CLK_DISABLE(); 

//	__HAL_TIM_DISABLE(&htim1); 
//	__HAL_RCC_TIM1_CLK_DISABLE();



//      // 复位RCC配置
//		HAL_RCC_DeInit();
//		
//      // 配置HSE和PLL
//		RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//		
//      // 选择HSE作为PLL源
//		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//		RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//		
//      // PLL参数配置
//		//RCC_OscInitStruct.PLL.PLLM = PLL_M_Temp;
//		RCC_OscInitStruct.PLL.PLLN = 64;
//      RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8; // 主PLL分频系数 PLL_P_Temp = 8
//		RCC_OscInitStruct.PLL.PLLQ = 8;
//		
//      // 初始化振荡器
//		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//		{
//		}
//		
//      // 选择PLL作为系统时钟
//		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//		
//		
//      // 配置时钟树
//		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
//		{
//		}
//		
//      // 更新系统核心时钟变量
//		SystemCoreClockUpdate();
//		
//      // 延时初始化
//		delay_init(8);
	

//		usart1_init(115200);

//		uart4_init(9600);

//		MAX31865_Init();

//		IRD_StartWork();
		
}


void Wake_Up(void)      // 唤醒处理函数
{	
	SystemInit();
	
	HAL_Init();                         
    sys_stm32_clock_init(336, 8, 2, 7); 
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2); 
    delay_init(168);                   
    usart1_init(115200);               
    
    led_init();                        
    lcd_init();                        
	lcd_display_on();
    GUI_Init();
	
    IRD_StartWork();                    
    uart4_init(9600);                 	
    BM_GPIO_Init();                     
    PushMotor_PWM_Init(200-1, 21-1);   	
    MAX31865_Init();					
    DI_Init();							
    DO_Init();							
    PCF8563_Init();						
    AT24CXX_Init();
    W25QXX_Init();
    ADC1_Init();
    SystemInitFunc();
	CheckSystemFirstRun();
    InitParametersFromEEPROM();
	OTA_Init();
	MotorStop();
		
	g_page_reset_flag = 1;

}

uint32_t	WakeupCount=0;
uint32_t	LowFreqCount=0;
uint8_t		LowFreqFlag=0;
uint8_t		WakeupFlag=0;
uint8_t		ExCloseFlag=0;
uint8_t		OverTempFlag=0;
uint8_t		OverPressFlag=0;

/* LED任务 */

void led_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    
    DCDC(0);      // 关闭DC-DC电源
    ReverseRun();     // 反转运行
    OSTimeDlyHMSM(0,0,3,500,OS_OPT_TIME_HMSM_STRICT,&err);  // 延时3秒500毫秒
    KeepStop();      // 保持停止
    
    while(1)
    {
        // 检查是否需要唤醒
        uint8_t wakeup_needed = 0;
        
        // 红外按键检测
        uint8_t ir_key = IR_GetKey();
        if(ir_key == IR_KEY_STOP) {
            if(LowFreqFlag == 1) {  // 如果处于低功耗模式
                wakeup_needed = 1;
            }
        }
        
        // 温度检测
        if(temperature > AlarmTemperatureValue) {
            if(LowFreqFlag == 1) {
                wakeup_needed = 1;
                OverTempFlag = 1;
            }
        }
        
        // 外部关阀开关检测
        if(ExCloseSwitch == 1) {
            if(LowFreqFlag == 1) {
                wakeup_needed = 1;
                ExCloseFlag = 1;
            }
        }
        
        // 过压检测
        if(OverpressureCheckFunc() == 1) {
            if(LowFreqFlag == 1) {
                wakeup_needed = 1;
                OverPressFlag = 1;
            }
        }
        
        // 如果需要唤醒，置位唤醒标志
        if(wakeup_needed) {
            WakeupFlag = 1;
        }
        
        // 进入低功耗模式判断
        if((LowFreqCount > DeepSleepCount) && (DeepSleepCount > 10) && (LowFreqFlag == 0)) {
            LowFreqCount = 0;
            LowFreqFlag = 1;
            WakeupFlag = 0;
            OSTaskSuspend((OS_TCB *)&OLED_TaskTCB, &err);
            OSTaskSuspend((OS_TCB *)&Manage_TaskTCB, &err);
            Low_Power();
        }
        
        // 执行唤醒操作
        if(WakeupFlag == 1) {
            WakeupFlag = 0;
            LowFreqCount = 0;
            LowFreqFlag = 0;
            Wake_Up();
            OSTaskResume((OS_TCB *)&OLED_TaskTCB, &err);
            OSTaskResume((OS_TCB *)&Manage_TaskTCB, &err);
        }
        
        // 低功耗计时逻辑
        if(LowFreqFlag == 0) {
            static uint8_t timer_1s = 0;
            timer_1s++;
            if(timer_1s >= 2) {
                 timer_1s = 0;
                 LowFreqCount++;
            }
        }
        
        // 读取温度传感器
        if(MAX31865_DRDY_READ() == 0) {
            temperature = MAX31865_GetTemperature();
        }
        
        if(temperature > 125.0f) {
            temperature = 125.0f;
            MAX31865_SetConfig(MAX31865_CONFIG_BIAS | MAX31865_CONFIG_MODEAUTO | 
                               MAX31865_CONFIG_3WIRE | MAX31865_CONFIG_FILT50HZ);
            MAX31865_ClearFault();
        }
        else if(temperature < -45.0f) {
            temperature = -45.0f;
            MAX31865_SetConfig(MAX31865_CONFIG_BIAS | MAX31865_CONFIG_MODEAUTO | 
                               MAX31865_CONFIG_3WIRE | MAX31865_CONFIG_FILT50HZ);
            MAX31865_ClearFault();
        }
		
        // LED控制逻辑
        static uint8_t led_toggle = 0;
        led_toggle = !led_toggle;
        
        if(ErrorAlarm == Opening) {
            // 开阀中：LEDG闪烁，LEDR熄灭
            LEDR(0);
            LEDG(led_toggle);
        }
        else if(ErrorAlarm == CloseError || ErrorAlarm == OpenError || 
                ErrorAlarm == SensorError || ErrorAlarm == OverPressClose || 
                ErrorAlarm == UnderPressClose || ErrorAlarm == OverTempClose || 
                ErrorAlarm == MotorStallError) {
            // 故障状态：LEDR闪烁，LEDG熄灭
            LEDG(0);
            LEDR(led_toggle);
        }
        else {
            // 其他状态：LED保持熄灭
            LEDG(0);
            LEDR(0);
        }

        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}




void OpenCloseFunc(void)                // 开到位/关到位状态检测
{
        if((OpenSensor == GPIO_PIN_RESET)&&(CloseSensor == GPIO_PIN_SET))           // 开到位
			{
				uCoilsStatusLO.Bit2=1;	
				uCoilsStatusLO.Bit3=0;	
			}
        else if((OpenSensor == GPIO_PIN_SET)&&(CloseSensor == GPIO_PIN_RESET))      // 关到位
			{
				uCoilsStatusLO.Bit2=0;	
				uCoilsStatusLO.Bit3=1;	
			}
}

void manage_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    
    static enumErrorAlarm last_state = (enumErrorAlarm)-1;  // 记录上一次状态
    static enumModelID last_model = (enumModelID)-1;        // 记录上一次模式
    static uint32_t last_periodic_tag = 0xFFFFFFFFUL;       // 记录上一次5分钟定时写入标记
    static uint8_t startup_history_pending = 1U;       // 上电后首条历史待写入
    static uint8_t history_baseline_initialized = 0U;   // 历史记录比较基准已初始化
    static uint32_t startup_history_tick = 0U;          // 上电后首条历史延时起点
    
    while (1) {
        uint8_t need_history_write = 0;
        uint32_t now_tick = HAL_GetTick();
        uint32_t periodic_tag = (((uint32_t)current_date.year * 12UL + current_date.month) * 31UL + current_date.day);
        periodic_tag = (periodic_tag * 24UL) + current_time.hour;
        periodic_tag = (periodic_tag * 12UL) + (current_time.minute / 5U);

        if (history_baseline_initialized == 0U) {
            history_baseline_initialized = 1U;
            startup_history_tick = now_tick;
            last_state = ErrorAlarm;
            last_model = ModelID;
            last_periodic_tag = periodic_tag;
        }

        if (startup_history_pending == 1U) {
            if ((now_tick - startup_history_tick) >= 3000U) {
                startup_history_pending = 0U;
                last_state = ErrorAlarm;
                last_model = ModelID;
                last_periodic_tag = periodic_tag;
                Flash_History_Write(ModelID,
                                    HistoryActionNone,
                                    convert_alarm_to_history_result(ErrorAlarm),
                                    g_press_data.up_pressure,
                                    temperature);
            }
        }
        else {
            // 状态或模式变化时记录一次历史数据，不记录开阀中/关阀中瞬态
            if ((last_state != ErrorAlarm || last_model != ModelID) && 
                ErrorAlarm != Opening && ErrorAlarm != Closing) {
                last_state = ErrorAlarm;
                last_model = ModelID;
                need_history_write = 1;
            }
            // 状态变化到瞬态时只更新比较基准，避免切回稳定状态后漏记
            else if (last_state != ErrorAlarm || last_model != ModelID) {
                last_state = ErrorAlarm;
                last_model = ModelID;
            }

            // 每5分钟整点补充记录一次当前状态，避免长时间无变化时没有历史数据
            if ((current_time.minute % 5U == 0U) && (current_time.second < 2U) &&
                (last_periodic_tag != periodic_tag)) {
                last_periodic_tag = periodic_tag;
                need_history_write = 1;
            }

            if (need_history_write) {
                Flash_History_Write(ModelID,
                                    normalize_history_action_cause(ErrorAlarm),
                                    convert_alarm_to_history_result(ErrorAlarm),
                                    g_press_data.up_pressure,
                                    temperature);
            }
        }
        
        
        OpenCloseFunc();
        
        SetupState(ErrorAlarm);
        
        HandModelFunc();
        
        OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}



void key_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    
	uint8_t KeyCount=0;
	
    while (1) {
        // 按键扫描任务
		{
				KeyCount++;
				if(KeyCount>2)
				{
					KeyCount=0;
					KeyRespFunc();
				}
			}
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


/*===============================LCD=====================================================================================*/ 

void lcd_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    CPU_SR_ALLOC();
    
    uint8_t PrevMenuID,NextMenuID;
    PrevMenuID=NextMenuID=0;
    
    MenuID=0;
    
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    	
    while (1) {
        MenuIDCount++;
        if(MenuIDCount>50) {        // 自动切回主界面
            MenuIDCount=0;
            MenuID=0;
        }
        
        NextMenuID=MenuID;
        if(PrevMenuID!=NextMenuID) {
            CPU_CRITICAL_ENTER();
            PrevMenuID=NextMenuID;
            
            CPU_CRITICAL_EXIT();
            
            // 椤甸潰閲嶇疆鏍囧織
            g_page_reset_flag = 1;
        }

        if(MenuID==0) {
            FirstInterfaceFunc();
        }
        else if(MenuID==1) {
            SecondInterfaceFunc();
        }
        else if(MenuID==2) {
            ThirdInterfaceFunc();
        }
        else if(MenuID==3) {
            ForthInterfaceFunc();
        }
        
        OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}



/*============================ModBus========================================================================================*/

void modbus_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    static uint32_t sensor_update_counter = 0;
    
    RegHoldingOKFlag = FALSE;

    eMBInit(MB_RTU, DeviceID, 0x02, 9600, MB_PAR_NONE);
    eMBEnable();
    
    while(1) {
        // 轮询 Modbus 协议栈
        eMBErrorCode eStatus = eMBPoll();
		
        /* 处理 Modbus 远程控制指令（Coil 0: 开阀，Coil 1: 关阀） */
        /* 使用位掩码直接检测，并在处理后自动清除（点动模式），防止被后续逻辑覆盖 */
		if(ucRegCoilsBuf[0] & 0x01) 
		{
            IR_KEY_OPEN_FLAG = 1;        // 触发开阀信号
            ucRegCoilsBuf[0] &= ~0x01;   // 清除指令位
		}
		
		if(ucRegCoilsBuf[0] & 0x02) 
		{
            IR_KEY_CLOSE_FLAG = 1;       // 触发关阀信号
            ucRegCoilsBuf[0] &= ~0x02;   // 清除指令位
		}

        // 每10次循环（约1秒）更新一次保持寄存器
        if (++sensor_update_counter >= 10) {
            sensor_update_counter = 0;
            // 更新保持寄存器数据
            usRegHoldingBuf[0] = (uint16_t)(current_time.second & 0x3F);              // 秒
            usRegHoldingBuf[1] = (uint16_t)(current_time.minute & 0x3F);              // 分
            usRegHoldingBuf[2] = (uint16_t)(current_time.hour & 0x1F);                // 时
            usRegHoldingBuf[3] = (uint16_t)(current_date.day & 0x1F);                 // 日
            usRegHoldingBuf[4] = (uint16_t)(current_date.month & 0x0F);               // 月
            usRegHoldingBuf[5] = (uint16_t)(current_date.year & 0x3F);                // 年
            usRegHoldingBuf[6] = (uint16_t)(DeviceID & 0xFF);                         // 设备ID
            usRegHoldingBuf[7] = (uint16_t)(battery_voltage * 100);                   // 电池电压 * 100
            usRegHoldingBuf[8] = (uint16_t)(g_press_data.up_pressure * 1000);         // 当前压力 * 1000
            usRegHoldingBuf[9] = (uint16_t)(MaxPressure * 1000);                      // 最大压力 * 1000
            usRegHoldingBuf[10] = (uint16_t)(MinPressure * 1000);                     // 最小压力 * 1000
            usRegHoldingBuf[11] = (uint16_t)(temperature * 100);                      // 温度 * 100
            usRegHoldingBuf[12] = (uint16_t)(AlarmTemperatureValue * 100);            // 报警温度 * 100
			
			ucRegCoilsBuf[0]=uCoilsStatusLO.Byte;
			ucRegCoilsBuf[1]=uCoilsStatusHI.Byte;
        }
        
        // 更新线圈状态
        uCoilsStatusLO.Byte = ucRegCoilsBuf[0];	
        uCoilsStatusHI.Byte = ucRegCoilsBuf[1];
		
        // 更新过载和过温标志
        OverLoadFlag = uCoilsStatusLO.Bit4;
        OverTemperatureFlag = uCoilsStatusLO.Bit5;
        
        // 更新离散输入寄存器
        uInputStatusLO.Bit0 = OpenSensor;
        uInputStatusLO.Bit1 = CloseSensor;
        ucRegDiscreteBuf[0] = uInputStatusLO.Byte;
        
        // 如果上位机写入了保持寄存器
        if(RegHoldingOKFlag == TRUE) {
            RegHoldingOKFlag = FALSE;
            
            // 解析写入的数据
            current_time.second 		= 	usRegHoldingBuf[0] & 0x3F;
            current_time.minute 		= 	usRegHoldingBuf[1] & 0x3F;
            current_time.hour 			= 	usRegHoldingBuf[2] & 0x1F;            
            current_date.day 			= 	usRegHoldingBuf[3] & 0x1F;
            current_date.month			= 	usRegHoldingBuf[4] & 0x0F;
            current_date.year 			= 	usRegHoldingBuf[5] & 0x3F;           
            DeviceID 					= 	usRegHoldingBuf[6] & 0xFF;            
            battery_voltage 			= 	usRegHoldingBuf[7] / 100.0; 
            g_press_data.up_pressure 	= 	usRegHoldingBuf[8] / 1000.0;
            MaxPressure 				= 	usRegHoldingBuf[9] / 1000.0;
            MinPressure 				= 	usRegHoldingBuf[10] / 1000.0;
            temperature 				= 	usRegHoldingBuf[11] / 100.0;
            AlarmTemperatureValue 		= 	usRegHoldingBuf[12] / 100.0;
                   
            SyncTimeToRTC();// 同步时间到RTC
            
            // 检查设备ID范围
            if(DeviceID > MAX_DEV_ID) DeviceID = MAX_DEV_ID;
            else if(DeviceID < 1) DeviceID = 1;
            
            if(MaxPressure > AUTO_HIGH_MAX) MaxPressure = AUTO_HIGH_MAX;
            else if(MaxPressure < AUTO_HIGH_MIN) MaxPressure = AUTO_HIGH_MIN;
            
            if(MinPressure > AUTO_LOW_MAX) MinPressure = AUTO_LOW_MAX;
            else if(MinPressure < AUTO_LOW_MIN) MinPressure = AUTO_LOW_MIN;
            
            SetupModel(ModelID);
            
            // 保存到EEPROM
            AT24CXX_WriteUchar(ADR_MODELID, ModelID);
            AT24CXX_WriteUchar(ADR_DEVICEID, DeviceID);
            AT24CXX_WriteFloat(ADR_MAXPRESSURE, MaxPressure);
            AT24CXX_WriteFloat(ADR_MINPRESSURE, MinPressure);
        }
        
        OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}



/*============================压力传感器任务=======================================================================================*/

void press_task(void *p_arg) {		
    (void)p_arg;
    OS_ERR err;
   
    // 开启串口接收中断
    PRESS_RS485_DE(0);
    HAL_UARTEx_ReceiveToIdle_IT(&huart4, rx_buffer, RX_BUF_MAX_SIZE);

    while (1) {
            // 处理串口发送
             if (g_press_data.uart_ready) {
                 g_press_data.uart_ready = 0;
                 PRESS_RS485_DE(1);     // 切换到发送模式
                 HAL_UART_Transmit_IT(&huart4, tx_buffer, TX_DATA_SIZE);
         }

        // 任务延时
        OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


/*============================OTA升级任务========================================================================================*/

void ota_task(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    
    while (1) {

        OTA_Task();
        
        // 任务延时
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}























































