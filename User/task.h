#ifndef __TASK_H
#define __TASK_H

#include "os.h"
#include "cpu.h"
#include "./SYSTEM/sys/sys.h"

/* 任务配置结构体 */
typedef struct {
    OS_TCB          *tcb;           /* 任务控制块指针 */
    CPU_STK         *stk;           /* 任务堆栈指针 */
    OS_TASK_PTR     task_func;      /* 任务函数指针 */
    CPU_CHAR        *name;          /* 任务名称指针 */
    OS_PRIO         prio;           /* 任务优先级 */
    CPU_STK_SIZE    stk_size;       /* 任务堆栈大小 */
} TaskConfig_t;

/* 任务优先级定义 */
typedef enum {
    TASK_PRIO_START     = 4,
    TASK_PRIO_MODBUS    = 5,
    TASK_PRIO_OTA       = 6,    // 添加OTA任务优先级
    TASK_PRIO_LED       = 7,
    TASK_PRIO_MANAGE    = 9,
    TASK_PRIO_KEY       = 10,
    TASK_PRIO_OLED      = 11,
    TASK_PRIO_PRESS     = 13
} TaskPriority_t;

/* 任务堆栈大小定义 */
typedef enum {
    TASK_STK_SIZE_SMALL     = 256,
    TASK_STK_SIZE_MEDIUM    = 512,
    TASK_STK_SIZE_LARGE     = 1024,
    TASK_STK_SIZE_XLARGE    = 2048
} TaskStackSize_t;

/* 任务ID枚举 */
typedef enum {
    TASK_ID_START = 0,
    TASK_ID_LED,
    TASK_ID_MANAGE,
    TASK_ID_KEY,
    TASK_ID_OLED,
    TASK_ID_MODBUS,
    TASK_ID_OTA,        // 添加OTA任务ID
    TASK_ID_PRESS,
    TASK_ID_MAX
} TaskID_t;

/* 任务函数声明 */
void qdf_task(void);
void start_task(void *p_arg);
void led_task(void *p_arg);
void manage_task(void *p_arg);
void key_task(void *p_arg);
void lcd_task(void *p_arg);
void modbus_task(void *p_arg);
void press_task(void *p_arg);
void ota_task(void *p_arg);  // 添加OTA任务函数声明

/* 工具函数声明 */
OS_ERR create_task(const TaskConfig_t *config);
void task_error_handler(OS_ERR err, const char *task_name);


extern 	uint8_t 	NetSetFlag;
extern	uint32_t 	CurrRS485PressValue;

extern	uint32_t	LowFreqCount;
extern	uint32_t	WakeupCount;
extern	uint8_t		LowFreqFlag;
extern	float		McuTemperature;

extern	uint16_t	MenuIDCount;

extern	uint8_t		ExCloseFlag;
extern  uint8_t     OverTempFlag;
extern  uint8_t     OverPressFlag;


















#endif

