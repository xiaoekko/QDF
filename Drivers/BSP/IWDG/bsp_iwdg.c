#include "bsp_iwdg.h"

// IWDG句柄定义
IWDG_HandleTypeDef hiwdg;

/**
 * @brief  初始化独立看门狗
 * @param  prer: 分频数:0~7(只有低3位有效!)
 * @param  rlr: 自动重装载值,0~0XFFF
 * @note   分频因子=4*2^prer.但最大值只能是256!
 *         rlr:重装载寄存器值:低11位有效
 *         时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms)
 * @retval None
 */
void IWDG_Init(uint8_t prer, uint16_t rlr)
{
    // 配置IWDG句柄
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = prer;    // 设置分频系数
    hiwdg.Init.Reload = rlr;        // 设置重装载值
    
    // 初始化IWDG
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        // 初始化错误处理
    }
}

/**
 * @brief  喂独立看门狗
 * @param  None
 * @retval None
 */
void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);  // 刷新看门狗
}
