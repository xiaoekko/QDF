#include "max31865.h"
#include <math.h>


/**
 * @brief  软件SPI延时
 */
static void Soft_SPI_Delay(void)
{
    uint32_t i;
    for(i=0; i<10; i++) {
        __NOP();
    }
}

/**
 * @brief  软件SPI读写一个字节
 * @param  tx_data: 发送的数据
 * @retval 接收到的数据
 */
static uint8_t Soft_SPI_ReadWriteByte(uint8_t tx_data)
{
    uint8_t i, rx_data = 0;
    
    // MAX31865 使用 SPI Mode 1 (CPOL=0, CPHA=1) 或 Mode 3 (CPOL=1, CPHA=1)
    // 这里使用 Mode 1: CPOL=0 (空闲时时钟低电平), CPHA=1 (第二个边沿采样)
    
    for(i = 0; i < 8; i++)
    {
        MAX31865_SCK_LOW(); // CPOL=0, 空闲为低
        
        // 准备数据 (MSB First)
        if(tx_data & 0x80)
            MAX31865_MOSI_HIGH();
        else
            MAX31865_MOSI_LOW();
        tx_data <<= 1;
        
        Soft_SPI_Delay();
        MAX31865_SCK_HIGH(); // 第一个边沿 (Rising)，数据建立
        Soft_SPI_Delay();
        
        // 采样数据
        rx_data <<= 1;
        if(MAX31865_MISO_READ())
            rx_data |= 0x01;
            
        MAX31865_SCK_LOW(); // 第二个边沿 (Falling)，时钟回到空闲
        Soft_SPI_Delay();
    }
    
    return rx_data;
}

/**
 * @brief  MAX31865初始化
 * @param  None
 * @retval None
 */
void MAX31865_Init(void)
{
    MAX31865_GPIO_Init();
    
    // 配置MAX31865
    // BIAS ON, 自动模式, 3线制, 50HZ滤波
    MAX31865_SetConfig(MAX31865_CONFIG_BIAS | MAX31865_CONFIG_MODEAUTO | 
                      MAX31865_CONFIG_3WIRE | MAX31865_CONFIG_FILT50HZ);
    
    HAL_Delay(100); // 等待稳定
}

/**
 * @brief  MAX31865 GPIO引脚初始化
 * @param  None
 * @retval None
 */
void MAX31865_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    MAX31865_CS_GPIO_CLK_ENABLE();
    
    // 配置CS片选引脚 (PB6)
    GPIO_InitStruct.Pin = MAX31865_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // CS 默认上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX31865_CS_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置SCK时钟引脚 (PB3) - 软件SPI改为推挽输出
    GPIO_InitStruct.Pin = MAX31865_SCK_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX31865_SCK_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置MISO主机输入从机输出引脚 (PB4) - 软件SPI改为输入
    GPIO_InitStruct.Pin = MAX31865_MISO_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX31865_MISO_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置MOSI主机输出从机输入引脚 (PB5) - 软件SPI改为推挽输出
    GPIO_InitStruct.Pin = MAX31865_MOSI_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX31865_MOSI_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置DRDY数据就绪引脚 (PB7)
    GPIO_InitStruct.Pin = MAX31865_DRDY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MAX31865_DRDY_GPIO_PORT, &GPIO_InitStruct);
    
    // 初始化引脚状态
    MAX31865_CS_HIGH();
    MAX31865_SCK_LOW(); // Mode 0/1 idle low
    MAX31865_MOSI_LOW();
}

/**
 * @brief  MAX31865 SPI接口初始化
 * @param  None
 * @retval None
 */
/*
void MAX31865_SPI_Init(void)
{
    // ... (代码被注释掉)
}
*/

/**
 * @brief  读取MAX31865寄存器
 * @param  addr: 寄存器地址
 * @retval 寄存器值
 */
uint8_t MAX31865_ReadRegister(uint8_t addr)
{
    uint8_t tx_data = addr & 0x7F; // 读操作最高位为0
    uint8_t rx_data = 0;
    
    MAX31865_CS_LOW();
    Soft_SPI_ReadWriteByte(tx_data);      // 发送寄存器地址
    rx_data = Soft_SPI_ReadWriteByte(0xFF); // 发送Dummy byte接收数据
    MAX31865_CS_HIGH();
    
    return rx_data;
}

/**
 * @brief  写入MAX31865寄存器
 * @param  addr: 寄存器地址
 * @param  data: 要写入的数据
 * @retval None
 */
void MAX31865_WriteRegister(uint8_t addr, uint8_t data)
{
    uint8_t reg_addr = addr | 0x80; // 写操作最高位为1
    
    MAX31865_CS_LOW();
    Soft_SPI_ReadWriteByte(reg_addr);
    Soft_SPI_ReadWriteByte(data);
    MAX31865_CS_HIGH();
}

/**
 * @brief  读取RTD值
 * @param  None
 * @retval RTD原始值
 */
uint16_t MAX31865_ReadRTD(void)
{
    uint8_t msb = MAX31865_ReadRegister(MAX31865_RTD_MSB_REG);
    uint8_t lsb = MAX31865_ReadRegister(MAX31865_RTD_LSB_REG);
    
    uint16_t rtd = ((uint16_t)msb << 8) | lsb;
    rtd >>= 1; // 去掉故障位
    
    return rtd;
}

/**
 * @brief  计算温度值
 * @param  rtd_value: RTD原始值
 * @param  rtd_nominal: RTD标称阻值
 * @param  ref_resistor: 参考电阻值
 * @retval 温度值
 */
float MAX31865_CalculateTemperature(uint16_t rtd_value, float rtd_nominal, float ref_resistor)
{
    float rtd_resistance;
    float temperature;
    
    // 计算RTD实际电阻值
    rtd_resistance = (rtd_value * ref_resistor) / 32768.0f;
    
    float Z1, Z2, Z3, Z4;
    float temp;
    
    Z1 = -RTD_A;
    Z2 = RTD_A * RTD_A - (4 * RTD_B);
    Z3 = (4 * RTD_B) / rtd_nominal;
    Z4 = 2 * RTD_B;
    
    temp = Z2 + (Z3 * rtd_resistance);
    temp = (sqrtf(temp) + Z1) / Z4;
    
    if (temp >= 0)
    {
        temperature = temp;
    }
    else
    {
        // 负温度修正计算
        float rpoly = rtd_resistance;
        rpoly *= rpoly;
        rpoly *= rpoly;
        rpoly *= RTD_C;
        
        temperature = temp - rpoly;
    }
    
    return temperature;
}

/**
 * @brief  获取温度值
 * @param  None
 * @retval 温度值(摄氏度)
 */
float MAX31865_GetTemperature(void)
{
    uint16_t rtd_value = MAX31865_ReadRTD();
    return MAX31865_CalculateTemperature(rtd_value, 100.0f, 430.0f);
}

/**
 * @brief  清除故障状态
 * @param  None
 * @retval None
 */
void MAX31865_ClearFault(void)
{
    uint8_t config = MAX31865_ReadRegister(MAX31865_CONFIG_REG);
    config |= MAX31865_CONFIG_FAULTSTAT;
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
}

/**
 * @brief  读取故障状态
 * @param  None
 * @retval 故障状态寄存器值
 */
uint8_t MAX31865_ReadFault(void)
{
    return MAX31865_ReadRegister(MAX31865_FAULT_STATUS_REG);
}

/**
 * @brief  设置配置寄存器
 * @param  config: 配置值
 * @retval None
 */
void MAX31865_SetConfig(uint8_t config)
{
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
}

/**
 * @brief  使能/禁用偏置电压
 * @param  enable: 1-使能, 0-禁用
 * @retval None
 */
void MAX31865_EnableBias(uint8_t enable)
{
    uint8_t config = MAX31865_ReadRegister(MAX31865_CONFIG_REG);
    if (enable)
    {
        config |= MAX31865_CONFIG_BIAS;
    }
    else
    {
        config &= ~MAX31865_CONFIG_BIAS;
    }
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
}

/**
 * @brief  设置线制模式
 * @param  wires: 3-三线制, 4-四线制
 * @retval None
 */
void MAX31865_SetWires(uint8_t wires)
{
    uint8_t config = MAX31865_ReadRegister(MAX31865_CONFIG_REG);
    if (wires == 3)
    {
        config |= MAX31865_CONFIG_3WIRE;
    }
    else
    {
        config &= ~MAX31865_CONFIG_3WIRE;
    }
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
}

/**
 * @brief  设置滤波器频率
 * @param  filter: 50-50Hz, 60-60Hz
 * @retval None
 */
void MAX31865_SetFilter(uint8_t filter)
{
    uint8_t config = MAX31865_ReadRegister(MAX31865_CONFIG_REG);
    if (filter == 50)
    {
        config |= MAX31865_CONFIG_FILT50HZ;
    }
    else
    {
        config &= ~MAX31865_CONFIG_FILT50HZ;
    }
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
}


