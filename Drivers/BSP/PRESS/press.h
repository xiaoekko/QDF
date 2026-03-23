
#ifndef __BSP_PRESS_H__
#define __BSP_PRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "./SYSTEM/sys/sys.h"


/* 485方向控制引脚操作宏 */
#define PRESS_RS485_DE(x)   do{ x ? \
                      HAL_GPIO_WritePin(PRESS_RS485_DE_GPIO_PORT, PRESS_RS485_DE_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(PRESS_RS485_DE_GPIO_PORT, PRESS_RS485_DE_PIN, GPIO_PIN_RESET); \
                  }while(0)

/* 数据缓冲区大小定义 */
#define TX_DATA_SIZE                           8    
#define RX_BUF_MAX_SIZE                        64

/* 压力传感器相关常量 */
#define RS485_GATHER_TIMES                     10   // 采集次数
#define RS485_ABORT_TIMES                      3    // 放弃次数
#define MODBUS_FRAME_MIN_SIZE                  9    // Modbus帧最小长度
#define PRESSURE_MAX_VALUE                     99999.0f

/* 压力传感器数据结构 */
typedef struct {
    uint32_t encode_value;          // 编码值
    uint32_t tmp_adc_press;         // 临时ADC压力值
    float tmp_adc_value;            // 临时ADC值
    float tmp_press;                // 临时压力值
    float up_pressure;              // 上传压力值
    
    uint32_t sum_rx_buf[64];        // 接收数据缓冲区
    uint8_t sum_count;              // 计数器
    
    uint16_t temp_crc16;            // 临时CRC
    uint16_t temp_crc_check;        // 临时CRC校验
    
    volatile uint16_t rx_received_len;  // 接收数据长度
    volatile uint8_t uart_ready;        // 串口就绪标志
    volatile uint8_t data_valid;        // 压力数据有效标志
} press_data_t;

/* 通信数据包 - 优化版 */
typedef struct {
    uint8_t device_id;              // 设备ID
    uint8_t packet_cmd;             // 数据包命令
    uint16_t length;                // 数据长度
    uint8_t buffer[RX_BUF_MAX_SIZE]; // 数据缓冲区
    uint16_t crc_check;             // CRC校验值
    uint8_t error;                  // 错误标志
    uint8_t counter;                // 计数器
} comm_packet_t;

/* 全局变量声明 */
extern UART_HandleTypeDef huart4;
extern press_data_t g_press_data;
extern uint8_t tx_buffer[TX_DATA_SIZE];
extern uint8_t rx_buffer[RX_BUF_MAX_SIZE];
extern comm_packet_t g_rx_packet, g_tx_packet;

/* 函数声明 */

void press_rs485_send_data(uint8_t *buf, uint8_t len);
void press_data_process(uint8_t *data, uint16_t size);
float press_float32_convert(uint32_t num_float32);
uint16_t press_crc16_calculate(uint8_t *data, uint8_t len);
uint32_t press_filter_data(uint32_t *buffer, uint8_t length);
void press_reset_data(void);
void error_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_PRESS_H__ */



