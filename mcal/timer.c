/**
 * @file    timer.c
 * @author  Sungpie
 * @date    2026-09-05
 * @version 1.0.0
 * @brief   STM32F4 하드웨어 타이머 레지스터 제어 구현부
 * 
 * @note    - TIM1은 APB2 버스(최대 96MHz)에 연결되어 있습니다.
 *          - TIM2~TIM5는 APB1 버스(최대 96MHz)에 연결되어 있습니다.
 *          - TIM1, TIM3, TIM4는 16비트 카운터이므로 ARR 설정 시 65535를 넘지 않아야 합니다.
 */

#include "stm32f4xx.h"
#include "timer.h"

/* ==============================================================================
 * 1. mcal_timer_repeat_init : 타이머 반복 모드 초기화
 * ============================================================================== */
bool mcal_timer_repeat_init(uint8_t timer_instance, uint32_t freq, uint32_t arr)
{
    // 1차 공통 매개변수 유효 범위 검사 (0 방어 및 최대 주파수 체크)
    if ((freq == 0) || (freq > 96000000U) || (arr == 0))
    {
        return false;
    }

    switch (timer_instance)
    {
        //------- Timer 1 (16-bit, APB2 Bus) ----------------------------
        case 1:
            if (arr > 0xFFFFU) return false; // 16비트 초과 방어

            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // (0x1UL << 0U) -> 클록 공급
            TIM1->CR1     = TIM_CR1_DIR; // (0x1UL << 4U) -> Downcounting, Repeat Mode
            TIM1->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM1->ARR     = arr - 1;
            return true;

        //------- Timer 2 (32-bit, APB1 Bus) ----------------------------
        case 2:
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
            TIM2->CR1     = TIM_CR1_DIR;
            TIM2->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM2->ARR     = arr - 1;
            return true;

        //------- Timer 3 (16-bit, APB1 Bus) ----------------------------
        case 3:
            if (arr > 0xFFFFU) return false;

            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
            TIM3->CR1     = TIM_CR1_DIR;
            TIM3->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM3->ARR     = arr - 1;
            return true;

        //------- Timer 4 (16-bit, APB1 Bus) ----------------------------
        case 4:
            if (arr > 0xFFFFU) return false;

            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
            TIM4->CR1     = TIM_CR1_DIR;
            TIM4->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM4->ARR     = arr - 1;
            return true;

        //------- Timer 5 (32-bit, APB1 Bus) ----------------------------
        case 5:
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
            TIM5->CR1     = TIM_CR1_DIR;
            TIM5->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM5->ARR     = arr - 1;
            return true;

        default:
            return false; // 지원하지 않는 타이머 번호
    }
}

/* ==============================================================================
 * 2. mcal_timer_oneshot_init : 타이머 단발(원샷) 모드 초기화
 * ============================================================================== */
bool mcal_timer_oneshot_init(uint8_t timer_instance, uint32_t freq, uint32_t arr)
{
    // 1차 공통 매개변수 유효 범위 검사
    if ((freq == 0) || (freq > 96000000U) || (arr == 0))
    {
        return false;
    }

    switch (timer_instance)
    {
        //------- Timer 1 (16-bit, APB2 Bus) ----------------------------
        case 1:
            if (arr > 0xFFFFU) return false;

            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
            TIM1->CR1     = (TIM_CR1_DIR | TIM_CR1_OPM); // Downcounting + One-Pulse Mode
            TIM1->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM1->ARR     = arr - 1;
            return true;

        //------- Timer 2 (32-bit, APB1 Bus) ----------------------------
        case 2:
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
            TIM2->CR1     = (TIM_CR1_DIR | TIM_CR1_OPM);
            TIM2->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM2->ARR     = arr - 1;
            return true;

        //------- Timer 3 (16-bit, APB1 Bus) ----------------------------
        case 3:
            if (arr > 0xFFFFU) return false;

            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
            TIM3->CR1     = (TIM_CR1_DIR | TIM_CR1_OPM);
            TIM3->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM3->ARR     = arr - 1;
            return true;

        //------- Timer 4 (16-bit, APB1 Bus) ----------------------------
        case 4:
            if (arr > 0xFFFFU) return false;

            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
            TIM4->CR1     = (TIM_CR1_DIR | TIM_CR1_OPM);
            TIM4->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM4->ARR     = arr - 1;
            return true;

        //------- Timer 5 (32-bit, APB1 Bus) ----------------------------
        case 5:
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
            TIM5->CR1     = (TIM_CR1_DIR | TIM_CR1_OPM);
            TIM5->PSC     = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM5->ARR     = arr - 1;
            return true;

        default:
            return false;
    }
}

/* ==============================================================================
 * 3. mcal_timer_int_enable : 타이머 인터럽트 경로 켜기/끄기
 * ============================================================================== */
bool mcal_timer_int_enable(uint8_t timer_instance, bool act)
{
    switch (timer_instance)
    {
        //------- Timer 1 -------------------------------
        case 1:
            if (act)
            {
                TIM1->SR &= ~TIM_SR_UIF;                 // (0x1UL << 0U) -> 잔여 UIF 깃발 클리어
                NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn); // 25 -> 내부 클리어
                TIM1->DIER |= TIM_DIER_UIE;               // (0x1UL << 0U) -> 인터럽트 허용
                NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);       // 핸들러 허용
            }
            else
            {
                TIM1->DIER &= ~TIM_DIER_UIE;              // (0x1UL << 0U) -> 인터럽트 금지
                NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);      // 핸들러 금지
                NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn); // 잔여 펜딩 클리어
            }
            return true;

        //------- Timer 2 -------------------------------
        case 2:
            if (act)
            {
                TIM2->SR &= ~TIM_SR_UIF;
                NVIC_ClearPendingIRQ(TIM2_IRQn);
                TIM2->DIER |= TIM_DIER_UIE;
                NVIC_EnableIRQ(TIM2_IRQn);
            }
            else
            {
                TIM2->DIER &= ~TIM_DIER_UIE;
                NVIC_DisableIRQ(TIM2_IRQn);
                NVIC_ClearPendingIRQ(TIM2_IRQn);
            }
            return true;

        //------- Timer 3 -------------------------------
        case 3:
            if (act)
            {
                TIM3->SR &= ~TIM_SR_UIF;
                NVIC_ClearPendingIRQ(TIM3_IRQn);
                TIM3->DIER |= TIM_DIER_UIE;
                NVIC_EnableIRQ(TIM3_IRQn);
            }
            else
            {
                TIM3->DIER &= ~TIM_DIER_UIE;
                NVIC_DisableIRQ(TIM3_IRQn);
                NVIC_ClearPendingIRQ(TIM3_IRQn);
            }
            return true;

        //------- Timer 4 -------------------------------
        case 4:
            if (act)
            {
                TIM4->SR &= ~TIM_SR_UIF;
                NVIC_ClearPendingIRQ(TIM4_IRQn);
                TIM4->DIER |= TIM_DIER_UIE;
                NVIC_EnableIRQ(TIM4_IRQn);
            }
            else
            {
                TIM4->DIER &= ~TIM_DIER_UIE;
                NVIC_DisableIRQ(TIM4_IRQn);
                NVIC_ClearPendingIRQ(TIM4_IRQn);
            }
            return true;

        //------- Timer 5 -------------------------------
        case 5:
            if (act)
            {
                TIM5->SR &= ~TIM_SR_UIF;
                NVIC_ClearPendingIRQ(TIM5_IRQn);
                TIM5->DIER |= TIM_DIER_UIE;
                NVIC_EnableIRQ(TIM5_IRQn);
            }
            else
            {
                TIM5->DIER &= ~TIM_DIER_UIE;
                NVIC_DisableIRQ(TIM5_IRQn);
                NVIC_ClearPendingIRQ(TIM5_IRQn);
            }
            return true;

        default:
            return false;
    }
}

/* ==============================================================================
 * 4. mcal_timer_start : 타이머 카운터 출발 및 인터럽트 활성화
 * ============================================================================== */
bool mcal_timer_start(uint8_t timer_instance)
{
    switch (timer_instance)
    {
        //------- Timer 1 -------------------------------
        case 1:
            TIM1->EGR |= TIM_EGR_UG;                 // (0x1UL << 0U) -> 세팅값 즉시 장전
            TIM1->SR  &= ~TIM_SR_UIF;                // (0x1UL << 0U) -> UG로 켜진 플래그 청소
            NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn); // 25 -> 펜딩 클리어
            NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);       // NVIC 활성화
            TIM1->DIER |= TIM_DIER_UIE;               // (0x1UL << 0U) -> 인터럽트 허용
            TIM1->CR1  |= TIM_CR1_CEN;                // (0x1UL << 0U) -> 카운트 시작
            return true;

        //------- Timer 2 -------------------------------
        case 2:
            TIM2->EGR |= TIM_EGR_UG;
            TIM2->SR  &= ~TIM_SR_UIF;
            NVIC_ClearPendingIRQ(TIM2_IRQn);
            NVIC_EnableIRQ(TIM2_IRQn);
            TIM2->DIER |= TIM_DIER_UIE;
            TIM2->CR1  |= TIM_CR1_CEN;
            return true;

        //------- Timer 3 -------------------------------
        case 3:
            TIM3->EGR |= TIM_EGR_UG;
            TIM3->SR  &= ~TIM_SR_UIF;
            NVIC_ClearPendingIRQ(TIM3_IRQn);
            NVIC_EnableIRQ(TIM3_IRQn);
            TIM3->DIER |= TIM_DIER_UIE;
            TIM3->CR1  |= TIM_CR1_CEN;
            return true;

        //------- Timer 4 -------------------------------
        case 4:
            TIM4->EGR |= TIM_EGR_UG;
            TIM4->SR  &= ~TIM_SR_UIF;
            NVIC_ClearPendingIRQ(TIM4_IRQn);
            NVIC_EnableIRQ(TIM4_IRQn);
            TIM4->DIER |= TIM_DIER_UIE;
            TIM4->CR1  |= TIM_CR1_CEN;
            return true;

        //------- Timer 5 -------------------------------
        case 5:
            TIM5->EGR |= TIM_EGR_UG;
            TIM5->SR  &= ~TIM_SR_UIF;
            NVIC_ClearPendingIRQ(TIM5_IRQn);
            NVIC_EnableIRQ(TIM5_IRQn);
            TIM5->DIER |= TIM_DIER_UIE;
            TIM5->CR1  |= TIM_CR1_CEN;
            return true;

        default:
            return false;
    }
}

/* ==============================================================================
 * 5. mcal_timer_stop : 타이머 카운터 및 인터럽트 정지
 * ============================================================================== */
void mcal_timer_stop(uint8_t timer_instance)
{
    switch (timer_instance)
    {
        //------- Timer 1 -------------------------------
        case 1:
            TIM1->CR1  &= ~TIM_CR1_CEN;          // (0x1UL << 0U) -> 초시계 정지
            TIM1->DIER &= ~TIM_DIER_UIE;         // (0x1UL << 0U) -> 인터럽트 금지
            NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn); // NVIC 비활성화
            break;

        //------- Timer 2 -------------------------------
        case 2:
            TIM2->CR1  &= ~TIM_CR1_CEN;
            TIM2->DIER &= ~TIM_DIER_UIE;
            NVIC_DisableIRQ(TIM2_IRQn);
            break;

        //------- Timer 3 -------------------------------
        case 3:
            TIM3->CR1  &= ~TIM_CR1_CEN;
            TIM3->DIER &= ~TIM_DIER_UIE;
            NVIC_DisableIRQ(TIM3_IRQn);
            break;

        //------- Timer 4 -------------------------------
        case 4:
            TIM4->CR1  &= ~TIM_CR1_CEN;
            TIM4->DIER &= ~TIM_DIER_UIE;
            NVIC_DisableIRQ(TIM4_IRQn);
            break;

        //------- Timer 5 -------------------------------
        case 5:
            TIM5->CR1  &= ~TIM_CR1_CEN;
            TIM5->DIER &= ~TIM_DIER_UIE;
            NVIC_DisableIRQ(TIM5_IRQn);
            break;

        default:
            break; // 잘못된 번호는 아무 동작 없이 리턴
    }
}