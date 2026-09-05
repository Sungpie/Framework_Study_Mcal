/**
 * @file    pwm.h
 * @author  Sungpie
 * @date    2026-09-05
 * @version 1.0.0
 * @brief   MCAL 하드웨어 PWM 드라이버 인터페이스
 * 
 * @details TIM1부터 TIM5까지의 범용 및 고급 타이머를 활용하여
 *          PWM 파형을 생성, 주파수/주기 변경 및 채널별 독립 구동 인터페이스를 제공합니다.
 * 
 * @copyright Copyright (c) 2026 Sungpie. All rights reserved.
 */

#include "common.h"
#include "macro.h"

#ifndef PWM_H
#define PWM_H

bool mcal_pwm_init(uint8_t pwm_instance, uint32_t freq, uint32_t arr); //일단 성공 실패만

bool mcal_pwm_freq_change(uint8_t pwm_instance, uint32_t freq); //arr까지 변경하면 사용 시 매개변수가 헷갈릴 수 있어보여서
bool mcal_pwm_arr_change(uint8_t pwm_instance, uint32_t arr);

bool mcal_pwm_start(uint8_t pwm_instance, uint8_t cch, uint8_t duty);
void mcal_pwm_stop(uint8_t pwm_instance, uint8_t cch);

#endif