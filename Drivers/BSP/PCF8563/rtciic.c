#include "./BSP/PCF8563/rtciic.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief  初始化IIC GPIO
 * @retval 无
 */
void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* 设置初始电平 */
    PCF_IIC_SCL_HIGH();
    PCF_IIC_SDA_HIGH();

    /* 配置SCL引脚 */
    GPIO_InitStruct.Pin = PCF8563_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PCF8563_SCL_PORT, &GPIO_InitStruct);

    /* 配置SDA引脚 */
    GPIO_InitStruct.Pin = PCF8563_SDA_PIN;
    HAL_GPIO_Init(PCF8563_SDA_PORT, &GPIO_InitStruct);

    /* 设置初始电平 */
    PCF_IIC_SCL_HIGH();
    PCF_IIC_SDA_HIGH();
}

/**
 * @brief  反初始化IIC GPIO
 * @retval 无
 */
void IIC_GPIO_DeInit(void)
{
    HAL_GPIO_DeInit(PCF8563_SCL_PORT, PCF8563_SCL_PIN);
    HAL_GPIO_DeInit(PCF8563_SDA_PORT, PCF8563_SDA_PIN);
}

/**
 * @brief  设置SDA为输入模式
 * @retval 无
 */
void IIC_SDA_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = PCF8563_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PCF8563_SDA_PORT, &GPIO_InitStruct);
}

/**
 * @brief  设置SDA为输出模式
 * @retval 无
 */
void IIC_SDA_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = PCF8563_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PCF8563_SDA_PORT, &GPIO_InitStruct);
}

/**
 * @brief  IIC起始信号
 * @retval IIC状态
 */
IIC_Status_t PCF_IIC_Start(void)
{
    IIC_SDA_SetOutput();
    
    /* 确保总线空闲 */
    PCF_IIC_SDA_HIGH();
    PCF_IIC_SCL_HIGH();
    IIC_DELAY();
    
    /* 检查总线是否空闲 */
    if (PCF_IIC_SDA_READ() == GPIO_PIN_RESET) {
        return IIC_ERROR_BUS_BUSY;
    }
    
    /* 产生START信号 */
    PCF_IIC_SDA_LOW();
    IIC_DELAY();
    PCF_IIC_SCL_LOW();
    IIC_DELAY();
    
    return IIC_OK;
}

/**
 * @brief  IIC停止信号
 * @retval 无
 */
void PCF_IIC_Stop(void)
{
    IIC_SDA_SetOutput();
    
    PCF_IIC_SDA_LOW();
    IIC_DELAY();
    PCF_IIC_SCL_HIGH();
    IIC_DELAY();
    PCF_IIC_SDA_HIGH();
    IIC_DELAY();
}

/**
 * @brief  发送应答信号
 * @param  ack: true发送ACK, false发送NACK
 * @retval IIC状态
 */
IIC_Status_t PCF_IIC_SendAck(bool ack)
{
    IIC_SDA_SetOutput();
    
    if (ack) {
        PCF_IIC_SDA_LOW();   // ACK
    } else {
        PCF_IIC_SDA_HIGH();  // NACK
    }
    
    IIC_DELAY();
    PCF_IIC_SCL_HIGH();
    IIC_DELAY();
    PCF_IIC_SCL_LOW();
    IIC_DELAY();
    
    return IIC_OK;
}

/**
 * @brief  等待应答信号
 * @retval IIC状态
 */
IIC_Status_t PCF_IIC_WaitAck(void)
{
    uint16_t timeout = 0;
    
    IIC_SDA_SetInput();
    PCF_IIC_SDA_HIGH();
    IIC_DELAY();
    PCF_IIC_SCL_HIGH();
    IIC_DELAY();
    
    while (PCF_IIC_SDA_READ() == GPIO_PIN_SET) {
        timeout++;
        if (timeout > IIC_TIMEOUT_COUNT) {
            PCF_IIC_Stop();
            return IIC_ERROR_TIMEOUT;
        }
        IIC_DELAY();
    }
    
    PCF_IIC_SCL_LOW();
    IIC_DELAY();
    
    return IIC_OK;
}

/**
 * @brief  发送一个字节
 * @param  data: 要发送的数据
 * @retval IIC状态
 */
IIC_Status_t PCF_IIC_WriteByte(uint8_t data)
{
    uint8_t i;
    
    IIC_SDA_SetOutput();
    
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            PCF_IIC_SDA_HIGH();
        } else {
            PCF_IIC_SDA_LOW();
        }
        
        IIC_DELAY();
        PCF_IIC_SCL_HIGH();
        IIC_DELAY();
        PCF_IIC_SCL_LOW();
        
        data <<= 1;
    }
    
    return PCF_IIC_WaitAck();
}

/**
 * @brief  接收一个字节
 * @param  send_ack: true发送ACK, false发送NACK
 * @retval 接收到的数据
 */
uint8_t PCF_IIC_ReadByte(bool send_ack)
{
    uint8_t i;
    uint8_t data = 0;
    
    IIC_SDA_SetInput();
    
    for (i = 0; i < 8; i++) {
        data <<= 1;
        
        IIC_DELAY();
        PCF_IIC_SCL_HIGH();
        IIC_DELAY();
        
        if (PCF_IIC_SDA_READ() == GPIO_PIN_SET) {
            data |= 0x01;
        }
        
        PCF_IIC_SCL_LOW();
        IIC_DELAY();
    }
    
    PCF_IIC_SendAck(send_ack);
    
    return data;
}








