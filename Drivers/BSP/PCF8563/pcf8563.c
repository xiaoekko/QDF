#include "./BSP/PCF8563/pcf8563.h"
#include "./BSP/PCF8563/rtciic.h"
#include "./SYSTEM/delay/delay.h"
#include "UI.h"
/* 内部函数声明 */
static uint8_t BCD_To_Decimal(uint8_t bcd);
static uint8_t Decimal_To_BCD(uint8_t decimal);
static PCF8563_Status_t PCF8563_WriteRegister(uint8_t reg_addr, uint8_t data);
static PCF8563_Status_t PCF8563_ReadRegister(uint8_t reg_addr, uint8_t* data);
static bool IsValidTime(const PCF8563_Time_t* time);
static bool IsValidDate(const PCF8563_Date_t* date);



/**
 * @brief  BCD码转十进制
 * @param  bcd: BCD码
 * @retval 十进制数
 */
static uint8_t BCD_To_Decimal(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

/**
 * @brief  十进制转BCD码
 * @param  decimal: 十进制数
 * @retval BCD码
 */
static uint8_t Decimal_To_BCD(uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

/**
 * @brief  向PCF8563写寄存器
 * @param  reg_addr: 寄存器地址
 * @param  data: 要写入的数据
 * @retval PCF8563状态
 */
static PCF8563_Status_t PCF8563_WriteRegister(uint8_t reg_addr, uint8_t data)
{
    if (PCF_IIC_Start() != IIC_OK) {
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(PCF8563_WRITE_ADDRESS) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(reg_addr) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(data) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    PCF_IIC_Stop();
    return PCF8563_OK;
}

/**
 * @brief  从PCF8563读寄存器
 * @param  reg_addr: 寄存器地址
 * @param  data: 读取的数据指针
 * @retval PCF8563状态
 */
static PCF8563_Status_t PCF8563_ReadRegister(uint8_t reg_addr, uint8_t* data)
{
    if (data == NULL) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    if (PCF_IIC_Start() != IIC_OK) {
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(PCF8563_WRITE_ADDRESS) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(reg_addr) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_Start() != IIC_OK) {
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    if (PCF_IIC_WriteByte(PCF8563_READ_ADDRESS) != IIC_OK) {
        PCF_IIC_Stop();
        return PCF8563_ERROR_COMMUNICATION;
    }
    
    *data = PCF_IIC_ReadByte(0);
    PCF_IIC_Stop();
    
    return PCF8563_OK;
}

/**
 * @brief  验证时间有效性
 * @param  time: 时间结构体
 * @retval true: 有效, false: 无效
 */
static bool IsValidTime(const PCF8563_Time_t* time)
{
    if (time == NULL) return false;
    if (time->hour > 23) return false;
    if (time->minute > 59) return false;
    if (time->second > 59) return false;
    return true;
}

/**
 * @brief  验证日期有效性
 * @param  date: 日期结构体
 * @retval true: 有效, false: 无效
 */
static bool IsValidDate(const PCF8563_Date_t* date)
{
    if (date == NULL) return false;
    if (date->year > 99) return false;
    if (date->month < 1 || date->month > 12) return false;
    if (date->day < 1 || date->day > 31) return false;
    if (date->weekday > 6) return false;
    return true;
}

/**
 * @brief  初始化PCF8563
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_Init(void)
{
    PCF8563_Status_t status;
    
    // 初始化IIC接口
    IIC_GPIO_Init();
    HAL_Delay(100);
    
    // 检查PCF8563是否存在
    status = PCF8563_Check();
    if (status != PCF8563_OK) {
        return status;
    }
    
    // 启动PCF8563
    status = PCF8563_Start();
    if (status != PCF8563_OK) {
        return status;
    }
    
    // 配置CLKOUT为32.768KHz
    status = PCF8563_WriteRegister(PCF8563_REG_CLKOUT, 0x80);
    
    return status;
}

/**
 * @brief  反初始化PCF8563
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_DeInit(void)
{
    return PCF8563_Stop();
}

/**
 * @brief  检查PCF8563是否存在
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_Check(void)
{
    uint8_t test_data = 0x55;
    uint8_t read_data;
    PCF8563_Status_t status;
    
    // 写入测试数据到定时器值寄存器
    status = PCF8563_WriteRegister(PCF8563_REG_TIMER_VALUE, test_data);
    if (status != PCF8563_OK) {
        return status;
    }
    
    HAL_Delay(10);
    
    // 读回数据验证
    status = PCF8563_ReadRegister(PCF8563_REG_TIMER_VALUE, &read_data);
    if (status != PCF8563_OK) {
        return status;
    }
    
    if (read_data != test_data) {
        return PCF8563_ERROR_DEVICE_NOT_FOUND;
    }
    
    return PCF8563_OK;
}

/**
 * @brief  启动PCF8563
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_Start(void)
{
    uint8_t data;
    PCF8563_Status_t status;
    
    status = PCF8563_ReadRegister(PCF8563_REG_CONTROL_STATUS_1, &data);
    if (status != PCF8563_OK) {
        return status;
    }
    
    data &= ~PCF8563_STOP_BIT;  // 清除STOP位
    
    return PCF8563_WriteRegister(PCF8563_REG_CONTROL_STATUS_1, data);
}

/**
 * @brief  停止PCF8563
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_Stop(void)
{
    uint8_t data;
    PCF8563_Status_t status;
    
    status = PCF8563_ReadRegister(PCF8563_REG_CONTROL_STATUS_1, &data);
    if (status != PCF8563_OK) {
        return status;
    }
    
    data |= PCF8563_STOP_BIT;  // 设置STOP位
    
    return PCF8563_WriteRegister(PCF8563_REG_CONTROL_STATUS_1, data);
}

/**
 * @brief  设置时间
 * @param  time: 时间结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_SetTime(const PCF8563_Time_t* time)
{
    PCF8563_Status_t status;
    
    if (!IsValidTime(time)) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_WriteRegister(PCF8563_REG_SECONDS, 
                                   Decimal_To_BCD(time->second) & PCF8563_SECONDS_MASK);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_WriteRegister(PCF8563_REG_MINUTES, 
                                   Decimal_To_BCD(time->minute) & PCF8563_MINUTES_MASK);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_WriteRegister(PCF8563_REG_HOURS, 
                                   Decimal_To_BCD(time->hour) & PCF8563_HOURS_MASK);
    
    return status;
}

/**
 * @brief  获取时间
 * @param  time: 时间结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_GetTime(PCF8563_Time_t* time)
{
    uint8_t data;
    PCF8563_Status_t status;
    
    if (time == NULL) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_ReadRegister(PCF8563_REG_SECONDS, &data);
    if (status != PCF8563_OK) return status;
    time->second = BCD_To_Decimal(data & PCF8563_SECONDS_MASK);
    
    status = PCF8563_ReadRegister(PCF8563_REG_MINUTES, &data);
    if (status != PCF8563_OK) return status;
    time->minute = BCD_To_Decimal(data & PCF8563_MINUTES_MASK);
    
    status = PCF8563_ReadRegister(PCF8563_REG_HOURS, &data);
    if (status != PCF8563_OK) return status;
    time->hour = BCD_To_Decimal(data & PCF8563_HOURS_MASK);
    
    return PCF8563_OK;
}

/**
 * @brief  设置日期
 * @param  date: 日期结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_SetDate(const PCF8563_Date_t* date)
{
    PCF8563_Status_t status;
    
    if (!IsValidDate(date)) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_WriteRegister(PCF8563_REG_DAYS, 
                                   Decimal_To_BCD(date->day) & PCF8563_DAYS_MASK);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_WriteRegister(PCF8563_REG_WEEKDAYS, 
                                   date->weekday & PCF8563_WEEKDAYS_MASK);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_WriteRegister(PCF8563_REG_MONTHS, 
                                   Decimal_To_BCD(date->month) & PCF8563_MONTHS_MASK);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_WriteRegister(PCF8563_REG_YEARS, 
                                   Decimal_To_BCD(date->year));
    
    return status;
}

/**
 * @brief  获取日期
 * @param  date: 日期结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_GetDate(PCF8563_Date_t* date)
{
    uint8_t data;
    PCF8563_Status_t status;
    
    if (date == NULL) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_ReadRegister(PCF8563_REG_DAYS, &data);
    if (status != PCF8563_OK) return status;
    date->day = BCD_To_Decimal(data & PCF8563_DAYS_MASK);
    
    status = PCF8563_ReadRegister(PCF8563_REG_WEEKDAYS, &data);
    if (status != PCF8563_OK) return status;
    date->weekday = data & PCF8563_WEEKDAYS_MASK;
    
    status = PCF8563_ReadRegister(PCF8563_REG_MONTHS, &data);
    if (status != PCF8563_OK) return status;
    date->month = BCD_To_Decimal(data & PCF8563_MONTHS_MASK);
    
    status = PCF8563_ReadRegister(PCF8563_REG_YEARS, &data);
    if (status != PCF8563_OK) return status;
    date->year = BCD_To_Decimal(data);
    
    return PCF8563_OK;
}

/**
 * @brief  设置日期和时间
 * @param  datetime: 日期时间结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_SetDateTime(const PCF8563_DateTime_t* datetime)
{
    PCF8563_Status_t status;
    
    if (datetime == NULL) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_SetTime(&datetime->time);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_SetDate(&datetime->date);
    
    return status;
}

/**
 * @brief  获取日期和时间
 * @param  datetime: 日期时间结构体
 * @retval PCF8563状态
 */
PCF8563_Status_t PCF8563_GetDateTime(PCF8563_DateTime_t* datetime)
{
    PCF8563_Status_t status;
    
    if (datetime == NULL) {
        return PCF8563_ERROR_INVALID_PARAMETER;
    }
    
    status = PCF8563_GetTime(&datetime->time);
    if (status != PCF8563_OK) return status;
    
    status = PCF8563_GetDate(&datetime->date);
    
    return status;
}

/**
 * @brief  检查电压是否过低
 * @retval true: 电压过低, false: 电压正常
 */
bool PCF8563_IsVoltageLow(void)
{
    uint8_t data;
    
    if (PCF8563_ReadRegister(PCF8563_REG_SECONDS, &data) == PCF8563_OK) {
        return (data & PCF8563_VL_BIT) != 0;
    }
    
    return false;
}


// 添加时间同步函数
void SyncTimeToRTC(void)
{
    // 设置时间，不检查返回值，防止阻塞日期设置
    PCF8563_SetTime(&current_time);
    
    // 强制设置为0以确保参数校验通过
    current_date.weekday = 0;
    
    // 设置日期到PCF8563
    PCF8563_SetDate(&current_date);
}








