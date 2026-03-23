#ifndef __OTA_H
#define __OTA_H

#include "sys.h"
#include "stm32f4xx_hal.h"
#include "../../User/USART_.h"  /* 包含统一的串口配置 */

/* Flash Memory Layout for STM32F407ZGT6 (1MB) */
// Bootloader: Sector 0-3 (64KB). 0x08000000 - 0x0800FFFF
// FLAG Area:  Sector 4 (64KB). 0x08010000 - 0x0801FFFF
// APP1 Area:  Sector 5-7 (3x128KB = 384KB). 0x08020000 - 0x0807FFFF (Current App)
// APP2 Area:  Sector 8-10 (3x128KB = 384KB). 0x08080000 - 0x080DFFFF (New App)

/* FLAG Area Definitions */
#define OTA_FLAG_ADDRESS          ((uint32_t)0x08010000) // 升级标志存放地址 (Sector 4)
#define OTA_FLAG_SECTOR           FLASH_SECTOR_4
#define OTA_UPGRADE_FLAG_VALUE    ((uint32_t)0xAAAAAAAA) // Bootloader检查此标志来决定是否升级

/* APP2 (New Firmware) Area Definitions */
#define APP2_START_ADDRESS        ((uint32_t)0x08080000) // APP2 起始地址
#define APP2_SECTOR_START         FLASH_SECTOR_8         // APP2 起始扇区
#define APP2_SECTOR_END           FLASH_SECTOR_10        // APP2 结束扇区 (Sectors 8, 9, 10)

/* OTA Communication Protocol */
#define OTA_START_CMD        "OTA_START:" // 开始命令格式: "OTA_START:<file_size>:<crc32>\r\n"
#define OTA_ACK_MSG          "OTA_ACK\r\n"
#define OTA_NACK_MSG         "OTA_NACK\r\n"
#define OTA_DONE_MSG         "OTA_DONE\r\n"

#define OTA_PACKET_SIZE      (1024)  // 1KB per packet

/* OTA State Machine */
typedef enum {
    OTA_STATE_IDLE,             // 0: 空闲，等待升级指令
    OTA_STATE_START,            // 1: 已收到开始指令，准备擦除
    OTA_STATE_ERASING,          // 2: 正在擦除APP2区域
    OTA_STATE_WAIT_FOR_DATA,    // 3: 擦除完成，等待固件数据
    OTA_STATE_RECEIVING,        // 4: 正在接收数据
    OTA_STATE_COMPLETE,         // 5: 升级包接收完成
    OTA_STATE_ERROR             // 6: 发生错误
} OTA_State;


/* Function Prototypes */
void OTA_Init(void);
void OTA_Task(void);
void OTA_Process_Data(void);
void OTA_Reboot(void);
void SysTick_Handler_Hook(void);

/* Extern Global Variables for USART3 Interrupt */
extern uint8_t  g_usart3_rx_buf[];
extern uint16_t g_usart3_rx_cnt;
extern uint32_t g_usart3_rx_tick;


#endif
