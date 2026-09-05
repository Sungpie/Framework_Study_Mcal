/**
 * @file    timer.h
 * @author  Sungpie
 * @date    2026-09-05
 * @version 1.0.0
 * @brief   하드웨어 타이머 드라이버 인터페이스
 * 
 * @details TIM1부터 TIM5까지의 하드웨어 타이머를 추상화하여
 *          주기적 타이머(Repeat) 및 원샷(One-shot) 제어 기능 등을 제공합니다.
 * 
 * @copyright Copyright (c) 2026 Sungpie. All rights reserved.
 */

#ifndef TIMER_H   /* 1. TIMER_H가 정의되어 있지 않다면 진입 */
#define TIMER_H   /* 2. 진입하자마자 TIMER_H를 정의 */

#include "common.h"
#include "macro.h"

typedef enum {
    MCAL_TIMER_OK = 0,
    MCAL_TIMER_ERROR,
    MCAL_TIMER_BUSY,
    MCAL_TIMER_TIMEOUT    
} mcal_timer_status_t;

bool mcal_timer_repeat_init(uint8_t timer_instance, uint32_t freq, uint32_t arr);
bool mcal_timer_oneshot_init(uint8_t timer_instance, uint32_t freq, uint32_t arr);

bool mcal_timer_int_enable(uint8_t timer_instance, bool act);

bool mcal_timer_start(uint8_t timer_instance);
void mcal_timer_stop(uint8_t timer_instance);

#endif