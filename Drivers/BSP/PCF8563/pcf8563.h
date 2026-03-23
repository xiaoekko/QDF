
#ifndef __BSP_PCF8563_H__ 
#define __BSP_PCF8563_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../User/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* PCF8563设备地址定义 */
#define PCF8563_DEVICE_ADDRESS           0xA2    // PCF8563设备地址(写)
#define PCF8563_WRITE_ADDRESS            0xA2    // 写地址
#define PCF8563_READ_ADDRESS             0xA3    // 读地址

/* 寄存器地址定义 */
#define PCF8563_REG_CONTROL_STATUS_1     0x00    // 控制/状态寄存器1
#define PCF8563_REG_CONTROL_STATUS_2     0x01    // 控制/状态寄存器2
#define PCF8563_REG_SECONDS              0x02    // 秒寄存器
#define PCF8563_REG_MINUTES              0x03    // 分钟寄存器
#define PCF8563_REG_HOURS                0x04    // 小时寄存器
#define PCF8563_REG_DAYS                 0x05    // 日寄存器
#define PCF8563_REG_WEEKDAYS             0x06    // 星期寄存器
#define PCF8563_REG_MONTHS               0x07    // 月寄存器
#define PCF8563_REG_YEARS                0x08    // 年寄存器
#define PCF8563_REG_MINUTE_ALARM         0x09    // 分钟闹钟寄存器
#define PCF8563_REG_HOUR_ALARM           0x0A    // 小时闹钟寄存器
#define PCF8563_REG_DAY_ALARM            0x0B    // 日闹钟寄存器
#define PCF8563_REG_WEEKDAY_ALARM        0x0C    // 星期闹钟寄存器
#define PCF8563_REG_CLKOUT               0x0D    // CLKOUT控制寄存器
#define PCF8563_REG_TIMER_CONTROL        0x0E    // 定时器控制寄存器
#define PCF8563_REG_TIMER_VALUE          0x0F    // 定时器值寄存器

/* 控制位定义 */
#define PCF8563_STOP_BIT                 0x08    // 停止位
#define PCF8563_TESTC_BIT                0x08    // 测试位
#define PCF8563_VL_BIT                   0x80    // 电压低位

/* 数据掩码定义 */
#define PCF8563_SECONDS_MASK             0x7F    // 秒掩码
#define PCF8563_MINUTES_MASK             0x7F    // 分钟掩码
#define PCF8563_HOURS_MASK               0x3F    // 小时掩码
#define PCF8563_DAYS_MASK                0x3F    // 日掩码
#define PCF8563_WEEKDAYS_MASK            0x07    // 星期掩码
#define PCF8563_MONTHS_MASK              0x1F    // 月掩码
#define PCF8563_YEARS_MASK               0xFF    // 年掩码

/* 函数声明 */
PCF8563_Status_t PCF8563_Init(void);
PCF8563_Status_t PCF8563_DeInit(void);
PCF8563_Status_t PCF8563_Check(void);
PCF8563_Status_t PCF8563_Start(void);
PCF8563_Status_t PCF8563_Stop(void);
PCF8563_Status_t PCF8563_SetTime(const PCF8563_Time_t* time);
PCF8563_Status_t PCF8563_GetTime(PCF8563_Time_t* time);
PCF8563_Status_t PCF8563_SetDate(const PCF8563_Date_t* date);
PCF8563_Status_t PCF8563_GetDate(PCF8563_Date_t* date);
PCF8563_Status_t PCF8563_SetDateTime(const PCF8563_DateTime_t* datetime);
PCF8563_Status_t PCF8563_GetDateTime(PCF8563_DateTime_t* datetime);
bool PCF8563_IsVoltageLow(void);
void SyncTimeToRTC(void);
#ifdef __cplusplus
}
#endif

#endif /* __BSP_PCF8563_H__ */



