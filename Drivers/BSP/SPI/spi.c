#include "./BSP/SPI/spi.h"

// SPI句柄
SPI_HandleTypeDef hspi1;

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// 使能时钟
	SPIFLASH_SPI_CLK_ENABLE();
	SPIFLASH_SCK_GPIO_CLK_ENABLE();
	SPIFLASH_MISO_GPIO_CLK_ENABLE();
	SPIFLASH_MOSI_GPIO_CLK_ENABLE();

	// 配置SPI GPIO
	// SCK引脚配置
	GPIO_InitStruct.Pin = SPIFLASH_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = SPIFLASH_SCK_AF;
	HAL_GPIO_Init(SPIFLASH_SCK_GPIO_PORT, &GPIO_InitStruct);

	// MISO引脚配置
	GPIO_InitStruct.Pin = SPIFLASH_MISO_PIN;
	GPIO_InitStruct.Alternate = SPIFLASH_MISO_AF;
	HAL_GPIO_Init(SPIFLASH_MISO_GPIO_PORT, &GPIO_InitStruct);

	// MOSI引脚配置
	GPIO_InitStruct.Pin = SPIFLASH_MOSI_PIN;
	GPIO_InitStruct.Alternate = SPIFLASH_MOSI_AF;
	HAL_GPIO_Init(SPIFLASH_MOSI_GPIO_PORT, &GPIO_InitStruct);

	// SPI参数配置
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;

	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		// 初始化失败处理
		while(1);
	}

	// 启动传输
	SPI1_ReadWriteByte(0xFF);
}

// SPI1速度设置函数
// SPI速度=fAPB2/分频系数
void SPI1_SetSpeed(uint32_t SPI_BaudRatePrescaler)
{
	// 重新配置SPI速度参数
	hspi1.Init.BaudRatePrescaler = SPI_BaudRatePrescaler;
	HAL_SPI_Init(&hspi1);
}

// SPI1 读写一个字节
// TxData:要写入的字节
// 返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	uint8_t RxData = 0;
	
	// 使用HAL库函数SPI收发
	if (HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, HAL_MAX_DELAY) != HAL_OK)
	{
		// 传输失败
		return 0xFF;
	}
	
	return RxData;
}









