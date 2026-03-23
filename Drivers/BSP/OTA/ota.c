#include "ota.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "USART_.h"
#include "./BSP/LCD/lcd.h"


/* 移植				注释#define VECT_TAB_OFFSET  0x00  
					IROM1:0x8020000
					User 1# : fromelf --bin -o "$L@L.bin" "#L"
					,VECT_TAB_OFFSET=0x20000
*/

/* 私有常量 */
#define OTA_RX_TIMEOUT_TICKS   20 // 判定接收超时的Tick数 (20 ticks)

/* 全局变量 */
static OTA_State ota_state = OTA_STATE_IDLE; // OTA状态机
static uint32_t ota_fwr_size = 0;           // 固件总大小
static uint32_t ota_recv_size = 0;          // 已接收大小
static uint32_t ota_write_addr = APP2_START_ADDRESS; // Flash写入地址

/* CRC 相关变量 */
static uint32_t ota_expected_crc32 = 0;        // 期望 CRC32
static uint32_t ota_calc_crc32     = 0xFFFFFFFF; // 当前累计 CRC32
static uint8_t  ota_crc_verified   = 0;        // CRC 是否已验证


/* 内部函数声明 */
static void OTA_Usart_SendString(char *str);
static uint8_t OTA_EraseApp2Sectors(void);
static uint8_t OTA_WriteToFlash(uint8_t* data, uint16_t len);
static uint8_t OTA_SetUpgradeFlag(void);
/* 删除这里的 static uint32_t millis(void); 声明 */

/* === 软件 CRC32 计算 (多项式 0xEDB88320) === */
static uint32_t CRC32_Update(uint32_t crc, const uint8_t *buf, uint32_t len) {
    while (len--) {
        crc ^= *buf++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 1U)
                crc = (crc >> 1) ^ 0xEDB88320U;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/**
 * @brief  HAL库: 初始化用于OTA的USART3
 * @note   TX: PB10, RX: PB11, Baud: 115200
 */
void OTA_Init(void) {
	
	usart3_init();
    
    ota_state = OTA_STATE_IDLE;
	
}

/**
 * @brief  处理接收到的数据/命令
 */
void OTA_Process_Data(void) {
    // 确保字符串结束
    g_usart3_rx_buf[g_usart3_rx_cnt] = '\0';

    if (ota_state == OTA_STATE_IDLE) {
        /* 仅在空闲状态下接收开始命令，格式: OTA_START:<size>:<crc32> */
        if (strncmp((char*)g_usart3_rx_buf, OTA_START_CMD, strlen(OTA_START_CMD)) == 0) {
            char *p = (char*)g_usart3_rx_buf + strlen(OTA_START_CMD);
            ota_fwr_size = strtoul(p, &p, 10);   // 文件大小

            if (*p == ':') {
                p++;
                ota_expected_crc32 = strtoul(p, NULL, 16); // 期望 CRC32
            }

            if (ota_fwr_size > 0 && ota_fwr_size <= (384 * 1024)) {
                ota_calc_crc32   = 0xFFFFFFFF;
                ota_crc_verified = 0;
                ota_state        = OTA_STATE_START;
                
                // 添加：通过串口3回复已收到OTA_START命令
                OTA_Usart_SendString("OTA_START_RECEIVED\r\n");
            } else {
                OTA_Usart_SendString(OTA_NACK_MSG);
            }
        }
    } else if (ota_state == OTA_STATE_WAIT_FOR_DATA || ota_state == OTA_STATE_RECEIVING) {
        /* 接收固件数据 */
        ota_state = OTA_STATE_RECEIVING;
        if (OTA_WriteToFlash(g_usart3_rx_buf, g_usart3_rx_cnt)) {
            ota_calc_crc32 = CRC32_Update(ota_calc_crc32, g_usart3_rx_buf, g_usart3_rx_cnt);
            ota_recv_size  += g_usart3_rx_cnt;
            ota_write_addr += g_usart3_rx_cnt;

            if (ota_recv_size >= ota_fwr_size) {
                ota_calc_crc32 = ~ota_calc_crc32; // 反转得到最终 CRC32
                ota_state = OTA_STATE_COMPLETE;
                OTA_Usart_SendString(OTA_DONE_MSG);
            } else {
                OTA_Usart_SendString(OTA_ACK_MSG);
            }
        } else {
            ota_state = OTA_STATE_ERROR;
            OTA_Usart_SendString(OTA_NACK_MSG);
        }
    }
    
    // 清空本次接收计数
    g_usart3_rx_cnt = 0;
}

/**
 * @brief  OTA主任务状态机
 */
void OTA_Task(void) {
    static OS_TICK last_display_time = 0;
    char lcd_buf[50];
    OS_ERR err;
    OS_TICK now_ticks;

    // 1. 数据接收处理
    if (g_usart3_rx_cnt > 0) {
        // 检查是否接收到完整命令（以\r\n结尾）
        if (g_usart3_rx_cnt >= 2 && 
            g_usart3_rx_buf[g_usart3_rx_cnt-2] == '\r' && 
            g_usart3_rx_buf[g_usart3_rx_cnt-1] == '\n') {
            // 收到命令，立即处理
            OTA_Process_Data();
        }
        // 检查数据接收是否超时 (仅在等待或接收数据状态)
        else if (ota_state == OTA_STATE_WAIT_FOR_DATA || ota_state == OTA_STATE_RECEIVING)
        {
            now_ticks = OSTimeGet(&err);
            if ((now_ticks - g_usart3_last_rx_tick) > OTA_RX_TIMEOUT_TICKS)
            {
                // 数据帧接收超时，处理数据
                OTA_Process_Data();
            }
        }
    }

    // 2. OTA状态机处理
    switch (ota_state) {
        case OTA_STATE_START:
            lcd_clear(WHITE);

            lcd_show_string(10, 10, 200, 16, 16, "OTA Started.",BLUE);
            sprintf(lcd_buf, "Fwr Size: %d KB", ota_fwr_size / 1024);
            lcd_show_string(10, 30, 230, 16, 16, lcd_buf,BLUE);
            ota_state = OTA_STATE_ERASING;
            break;

        case OTA_STATE_ERASING:

            lcd_show_string(10, 50, 200, 16, 16, "Erasing APP2 Area...",BLUE);
            if (OTA_EraseApp2Sectors()) {
                lcd_show_string(10, 70, 200, 16, 16, "Erase OK. Ready for data.",BLUE);
                OTA_Usart_SendString(OTA_ACK_MSG);
                ota_state = OTA_STATE_WAIT_FOR_DATA;
                ota_recv_size = 0;
                ota_write_addr = APP2_START_ADDRESS;
            } else {
                lcd_show_string(10, 70, 200, 16, 16, "Erase Failed!",BLUE);
                ota_state = OTA_STATE_ERROR;
            }
            break;
        
        case OTA_STATE_RECEIVING:
            now_ticks = OSTimeGet(&err);
            if(now_ticks - last_display_time > 500) { // 每500ms更新一次进度
                last_display_time = now_ticks;

                lcd_show_string(10, 90, 200, 16, 16, "Receiving data...",BLUE);
                uint8_t progress = 0;
                if (ota_fwr_size > 0) {
                    progress = (uint8_t)(((long long)ota_recv_size * 100) / ota_fwr_size);
                }
                if (progress > 100) {
                    progress = 100;
                }
                sprintf(lcd_buf, "Progress: %d%%", progress);
                lcd_show_string(10, 110, 200, 16, 16, lcd_buf,BLUE);

                /* 添加诊断信息: 显示接收字节数 */
                sprintf(lcd_buf, "Recv: %lu / %lu", ota_recv_size, ota_fwr_size);
                lcd_show_string(10, 130, 230, 16, 16, lcd_buf, BLUE);
            }
            break;

        case OTA_STATE_COMPLETE:
            if (!ota_crc_verified) {
                lcd_clear(WHITE);
                lcd_show_string(10, 10, 200, 16, 16, "Verifying CRC...",BLUE);

                /* 添加诊断信息: 显示CRC校验码 */
                sprintf(lcd_buf, "Exp CRC: %08lX", ota_expected_crc32);
                lcd_show_string(10, 30, 230, 16, 16, lcd_buf, BLUE);
                sprintf(lcd_buf, "Got CRC: %08lX", ota_calc_crc32);
                lcd_show_string(10, 50, 230, 16, 16, lcd_buf, BLUE);

                if (ota_calc_crc32 == ota_expected_crc32) {
                    lcd_show_string(10, 70, 200, 16, 16, "CRC OK!",GREEN);
                    ota_crc_verified = 1;
                    OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err); // 延时1秒，方便查看
                } else {
                    lcd_show_string(10, 70, 200, 16, 16, "CRC ERROR!",RED);
                    ota_state = OTA_STATE_ERROR;
                    OTA_Usart_SendString(OTA_NACK_MSG);
                    break;
                }
            }

            lcd_show_string(10, 90, 200, 16, 16, "Setting flag...",GREEN);
            if (OTA_SetUpgradeFlag()) {
                lcd_show_string(10, 110, 200, 16, 16, "Flag OK. Rebooting...",GREEN);
                OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);
                OTA_Reboot();
            } else {
                lcd_show_string(10, 110, 200, 16, 16, "Flag Failed!",RED);
                ota_state = OTA_STATE_ERROR;
            }
            break;
        
        case OTA_STATE_ERROR:
             lcd_show_string(10, 180, 200, 16, 16, "OTA Failed! Halted.",RED);
             while(1); // 停机

        case OTA_STATE_IDLE:
        case OTA_STATE_WAIT_FOR_DATA:
        default:
            // 在这些状态下不执行特殊操作，等待事件触发
            break;
    }
}

/**
 * @brief  软复位
 */
void OTA_Reboot(void) {
    __disable_irq();
    NVIC_SystemReset();
}

/*******************************************************************************
*                        内部(static)函数实现
*******************************************************************************/

/**
 * @brief  通过USART3发送字符串 (HAL库版本)
 */
static void OTA_Usart_SendString(char *str) {
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/**
 * @brief  擦除APP2区域的Flash扇区 (HAL库版本)
 * @retval 1: 成功, 0: 失败
 */
static uint8_t OTA_EraseApp2Sectors(void) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    
    HAL_FLASH_Unlock();
    
    /* 配置擦除参数 */
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_8;  // 起始扇区
    EraseInitStruct.NbSectors = 3;            // 擦除3个扇区 (8, 9, 10)
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }
    
    HAL_FLASH_Lock();
    return 1;
}

/**
 * @brief  将数据写入Flash (HAL库版本)
 * @retval 1: 成功, 0: 失败
 */
static uint8_t OTA_WriteToFlash(uint8_t* data, uint16_t len) {
    HAL_FLASH_Unlock();
    
    for (uint16_t i = 0; i < len; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ota_write_addr + i, data[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return 0; // 写入失败
        }
    }
    
    HAL_FLASH_Lock();
    return 1;
}

/**
 * @brief  设置升级标志 (HAL库版本)
 * @retval 1: 成功, 0: 失败
 */
static uint8_t OTA_SetUpgradeFlag(void) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;
    
    HAL_FLASH_Unlock();
    
    // 1. 擦除FLAG扇区
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = OTA_FLAG_SECTOR;
    EraseInitStruct.NbSectors = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }

    // 2. 写入升级标志
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, OTA_FLAG_ADDRESS, OTA_UPGRADE_FLAG_VALUE) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }

    HAL_FLASH_Lock();
    return 1;
}

/* 将下面的 millis() 相关函数实现整个删除 */
/**
 * @brief  获取系统毫秒级时间 (简易实现)
 */
/*
#include "stm32f4xx_it.h" 
volatile uint32_t sys_tick_counter = 0;

void SysTick_Handler_Hook(void) { // 在stm32f4xx_it.c的SysTick_Handler中调用
    sys_tick_counter++;
}

uint32_t millis(void) {
    return sys_tick_counter;
}
*/



