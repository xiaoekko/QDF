#ifndef __VERSION_CONF_H
#define __VERSION_CONF_H

/* ==========================================
 * 版本信息配置
 * ========================================== */

// 硬件版本
#define HARDWARE_VERSION_STR    " V2.0.0"

// 软件版本
#define SOFTWARE_VERSION_STR    " V2.2.1"

// 构建日期
#define BUILD_DATE_STR          "2026-03-24"


/* ==========================================
 * 设备型号配置
 * 取消注释下方对应的型号定义
 * ========================================== */

#define DEVICE_MODEL_DN50
//#define DEVICE_MODEL_DN80
//#define DEVICE_MODEL_DN100


#if defined(DEVICE_MODEL_DN50)
    #define DEVICE_MODEL_STR    "DN50"
    #define DEVICE_MODEL_X      250
    #define DEVICE_MODEL_Y      120
#elif defined(DEVICE_MODEL_DN80)
    #define DEVICE_MODEL_STR    "DN80"
    #define DEVICE_MODEL_X      200
    #define DEVICE_MODEL_Y      125
#elif defined(DEVICE_MODEL_DN100)
    #define DEVICE_MODEL_STR    "DN100"
    #define DEVICE_MODEL_X      200
    #define DEVICE_MODEL_Y      125
#else
    /* 默认配置 */
    #define DEVICE_MODEL_STR    "DN50"
    #define DEVICE_MODEL_X      250
    #define DEVICE_MODEL_Y      120
#endif

#endif
