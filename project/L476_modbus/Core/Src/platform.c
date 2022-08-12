#include "platform.h"

// ============================================================================
//
void platform_set_timer(TIM_HandleTypeDef* htim, uint32_t value)
{
    __HAL_TIM_SET_AUTORELOAD(htim, value);
}

// ============================================================================
//
void platform_start_timer(TIM_HandleTypeDef* htim)
{
    __HAL_TIM_SET_COUNTER(htim,0);
    HAL_TIM_Base_Start_IT(htim);
}

// ============================================================================
//
void platform_stop_timer(TIM_HandleTypeDef* htim)
{
    HAL_TIM_Base_Stop_IT(htim);
}