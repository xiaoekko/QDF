#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"

// ADC和DMA句柄
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

uint16_t AD_Value[N][M]; // 存储多次ADC转换结果的数组，用于DMA传输

/*
 * 函数名称：ADC1_GPIO_Config
 * 函数功能：配置ADC相关的ADC GPIO引脚
 * 输入参数: 无
 * 返回值  ：无
 * 说明    ：无
 */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// 使能GPIO时钟
	BAT_ADC_GPIO_CLK_ENABLE();
	POWER_ADC_GPIO_CLK_ENABLE();
	BACK1_ADC_GPIO_CLK_ENABLE();
	BACK2_ADC_GPIO_CLK_ENABLE();

	// 配置所有ADC引脚为模拟输入模式
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	// 配置BAT ADC引脚
	GPIO_InitStruct.Pin = BAT_ADC_PIN;
	HAL_GPIO_Init(BAT_ADC_GPIO_PORT, &GPIO_InitStruct);

	// 配置POWER ADC引脚
	GPIO_InitStruct.Pin = POWER_ADC_PIN;
	HAL_GPIO_Init(POWER_ADC_GPIO_PORT, &GPIO_InitStruct);
	
	// 配置BACK1 ADC引脚
	GPIO_InitStruct.Pin = BACK1_ADC_PIN;
	HAL_GPIO_Init(BACK1_ADC_GPIO_PORT, &GPIO_InitStruct);
	
	// 配置BACK2 ADC引脚
	GPIO_InitStruct.Pin = BACK2_ADC_PIN;
	HAL_GPIO_Init(BACK2_ADC_GPIO_PORT, &GPIO_InitStruct);
}

/*
 * 函数名称：ADC1_DMA_Config
 * 函数功能：配置ADC相关的ADC DMA
 * 输入参数: 无
 * 返回值  ：无
 * 说明    ：无
 */
static void ADC1_DMA_Config(void)
{
	// 使能DMA时钟
	VOLTAGE_ADC_DMA_CLK_ENABLE();
	
	// 配置DMA
	hdma_adc1.Instance = DMA2_Stream0;
	hdma_adc1.Init.Channel = DMA_CHANNEL_0;
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;
	hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	
	if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
	{
		// DMA初始化失败处理
		while(1);
	}
	
	// 关联DMA句柄到ADC句柄
	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
}

/*
 * 函数名称：ADC1_Mode_Config
 * 函数功能：配置ADC1的工作模式
 * 输入参数: 无
 * 返回值  ：无
 * 说明    ：无
 */
static void ADC1_Mode_Config(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// 使能ADC时钟
	VOLTAGE_ADC_CLK_ENABLE();
	
	// 配置ADC
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = M;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		// ADC初始化失败处理
		while(1);
	}
	
	// 配置ADC通道
	// BAT电压检测通道
	sConfig.Channel = BAT_ADC_CHANNEL;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		while(1);
	}
	
	// BACK1电压检测通道
	sConfig.Channel = BACK1_ADC_CHANNEL;
	sConfig.Rank = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		while(1);
	}
	
	// BACK2电压检测通道
	sConfig.Channel = BACK2_ADC_CHANNEL;
	sConfig.Rank = 3;
	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		while(1);
	}
	
	// POWER电压检测通道
	sConfig.Channel = POWER_ADC_CHANNEL;
	sConfig.Rank = 4;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		while(1);
	}
	
	// 使能温度传感器和VREFINT
	ADC->CCR |= ADC_CCR_TSVREFE;
}

/*
 * 函数名称：ADC1_Init
 * 函数功能：初始化ADC1相关配置
 * 输入参数：无
 * 返回值  ：无
 * 说明    ：无
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_DMA_Config();
	ADC1_Mode_Config();
	
	// 启动ADC DMA转换
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)AD_Value, M*N) != HAL_OK)
	{
		while(1);
	}
}

/*
 * 函数名称：Get_Battery_Voltage
 * 函数功能：获取电池电压值
 * 输入参数：无
 * 返回值  ：电池电压(V)
 * 说明    ：无
 */
float Get_Battery_Voltage(void)
{
	uint16_t adc_value;
	float adc_voltage;
	float battery_voltage;
	
	// 从DMA缓冲区中读取BAT通道的ADC值，BAT通道配置为Rank 1，对应数组索引0
	// 取最近10次采样的平均值
	uint32_t sum = 0;
	uint8_t samples = 10;
	if (samples > N) samples = N;
	
	for (uint8_t i = 0; i < samples; i++)
	{
		sum += AD_Value[N-1-i][0];  // BAT通道在第0位，取最近samples次采样
	}
	adc_value = sum / samples;
	
	// 将ADC值转换为电压值，参考电压为3.3V，12位ADC分辨率
	adc_voltage = (float)adc_value * 3.3f / 4096.0f;
	
	// 根据分压电路计算实际电池电压
	// 理论分压比 = 18kΩ/(100kΩ+100kΩ+18kΩ) = 18/218 = 0.0826
	// 理论系数 = 12.11
	// 实际校准：系数 12.11 时显示 30.2V，实际 23.2V -> 修正为 12.11 * (23.2/30.2) ≈ 9.30
	battery_voltage = adc_voltage * 9.30f;
	
	return battery_voltage;
}

	 




