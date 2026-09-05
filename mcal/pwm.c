/**
 * @file    pwm.c
 * @author  Sungpie
 * @date    2026-09-05
 * @version 1.0.0
 * @brief   STM32F4 하드웨어 타이머 기반 PWM 레지스터 제어 구현부
 * 
 * @details switch-case 점프 테이블 구조를 통해 TIM1~TIM5 하드웨어를 분기 제어하며,
 *          각 타이머의 GPIO Alternate Function, Downcounting 모드, Output Compare 모드를 구성합니다.
 * 
 * @note    [하드웨어 버스 및 핀 매핑 정보]
 *          - TIM1: APB2 버스 (AF1) -> CH1: PA8,  CH2: PA9,  CH3: PA10 (고급 제어 타이머, BDTR MOE 필수)
 *          - TIM2: APB1 버스 (AF1) -> CH1: PA15, CH2: PB3,  CH3: PB10 (32비트 카운터)
 *          - TIM3: APB1 버스 (AF2) -> CH1: PC6,  CH2: PC7,  CH3: PC8  (16비트 카운터)
 *          - TIM4: APB1 버스 (AF2) -> CH1: PB6,  CH2: PB7,  CH3: PB8  (16비트 카운터)
 *          - TIM5: APB1 버스 (AF2) -> CH1: PA0,  CH2: PA1             (32비트 카운터, PA2는 ST-Link 공유로 제외)
 */

#include "stm32f4xx.h"
#include "pwm.h"
#include "macro.h"

bool mcal_pwm_init(uint8_t pwm_instance, uint32_t freq, uint32_t arr)
{
    // 입력 매개변수 공통 유효성 검사 (0 방어 및 최대 주파수 체크)
    if ((freq == 0) || (freq > 96000000U) || (arr == 0))
    {
        return false;
    }

    switch (pwm_instance)
    {
        //------- Timer 1 PWM (16-bit, APB2 Bus) Channel 1 (PA8), Channel 2 (PA9), Channel 3 (PA10) -----------
        case 1:
            if (arr > 0xFFFFU) return false; // 16비트 타이머 카운터 한계(65,535) 방어

            // 1. TIM1(APB2 Bit 0) 및 GPIOA(AHB1 Bit 0) 클록 인가
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  // (0x1UL << 0U)
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // (0x1UL << 0U)

            // 2. GPIO 모드 설정 (PA8, PA9, PA10을 AF 모드 = 2로 설정)
            GPIOA->MODER &= ~((3U << 16) | (3U << 18) | (3U << 20));
            GPIOA->MODER |=  ((2U << 16) | (2U << 18) | (2U << 20));

            // 3. Alternate Function 매핑 (TIM1은 AF1 = 1)
            GPIOA->AFR[1] &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 8));
            GPIOA->AFR[1] |=  ((1U << 0) | (1U << 4) | (1U << 8));

            // 4. Downcounting + ARR Preload 활성화
            TIM1->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE; // (0x1UL << 4U) | (0x1UL << 7U)

            // 5. Output Compare 모드 설정 (PWM 모드 1 & Preload 활성화)
            TIM1->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
                          // (0x4UL << 4U) | (0x2UL << 4U) | (0x1UL << 3U) |
                          // (0x4UL << 12U) | (0x2UL << 12U) | (0x1UL << 11U)

            TIM1->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);
                          // (0x4UL << 4U) | (0x2UL << 4U) | (0x1UL << 3U)

            // 6. 극성 초기화 (Active High)
            TIM1->CCER = 0;

            // 7. [고급 타이머 필수] 메인 출력 활성화 (MOE = Main Output Enable)
            TIM1->BDTR |= TIM_BDTR_MOE; // (0x1UL << 15U)

            // 8. PSC 계산 및 유효 범위(16비트) 검증
            {
                double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;
                if (calculated_psc < 0.0 || calculated_psc > 65535.0)
                {
                    return false;
                }
                TIM1->PSC = (uint32_t)calculated_psc;
            }
            TIM1->ARR = arr - 1;

            // 9. Manual Update(UG 발생)로 레지스터 값 즉시 하드웨어에 로드
            TIM1->EGR |= TIM_EGR_UG;
            return true;

        //------- Timer 2 PWM (32-bit, APB1 Bus) Channel 1 (PA15), Channel 2 (PB3), Channel 3 (PB10) -----------
        case 2:
            if (arr > 0xFFFFU) return false;

            // 1. TIM2 및 GPIOA, GPIOB 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

            // 2. GPIO 모드 설정 (AF 모드 = 2)
            GPIOA->MODER &= ~(3U << 30);
            GPIOA->MODER |=  (2U << 30);

            GPIOB->MODER &= ~((3U << 6) | (3U << 20));
            GPIOB->MODER |=  ((2U << 6) | (2U << 20));

            // 3. Alternate Function 매핑 (AF1)
            GPIOA->AFR[1] &= ~(0xFU << 28);
            GPIOA->AFR[1] |=  (1U << 28);

            GPIOB->AFR[0] &= ~(0xFU << 12);
            GPIOB->AFR[0] |=  (1U << 12);

            GPIOB->AFR[1] &= ~(0xFU << 8);
            GPIOB->AFR[1] |=  (1U << 8);

            // 4. Downcounting + ARR Preload 활성화
            TIM2->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정
            TIM2->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM2->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            // 6. 극성 초기화
            TIM2->CCER = 0;

            // 7. PSC 계산 및 검증
            {
                double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;
                if (calculated_psc < 0.0 || calculated_psc > 65535.0)
                {
                    return false;
                }
                TIM2->PSC = (uint32_t)calculated_psc;
            }
            TIM2->ARR = arr - 1;

            TIM2->EGR |= TIM_EGR_UG;
            return true;

        //------- Timer 3 PWM (16-bit, APB1 Bus) Channel 1, 2, 3 (PC6, PC7, PC8) -----------
        case 3:
            if (arr > 0xFFFFU) return false;

            // 1. TIM3 및 GPIOC 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

            // 2. PC6, PC7, PC8 대체 기능 설정
            GPIOC->MODER &= ~((3U << 12) | (3U << 14) | (3U << 16));
            GPIOC->MODER |=  ((2U << 12) | (2U << 14) | (2U << 16));

            // 3. AF2(TIM3) 매핑
            GPIOC->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));
            GPIOC->AFR[0] |=  ((2U << 24) | (2U << 28));

            GPIOC->AFR[1] &= ~(0xFU << 0);
            GPIOC->AFR[1] |=  (2U << 0);

            // 4. Downcounting + ARR Preload 활성화
            TIM3->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정
            TIM3->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM3->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            TIM3->CCER = 0;

            // 6. PSC 계산 및 검증
            {
                double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;
                if (calculated_psc < 0.0 || calculated_psc > 65535.0)
                {
                    return false;
                }
                TIM3->PSC = (uint32_t)calculated_psc;
            }
            TIM3->ARR = arr - 1;

            TIM3->EGR |= TIM_EGR_UG;
            return true;

        //------- Timer 4 PWM (16-bit, APB1 Bus) Channel 1 (PB6), Channel 2 (PB7), Channel 3 (PB8) -----------
        case 4:
            if (arr > 0xFFFFU) return false;

            // 1. TIM4 및 GPIOB 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

            // 2. PB6, PB7, PB8 대체 기능 설정
            GPIOB->MODER &= ~((3U << 12) | (3U << 14) | (3U << 16));
            GPIOB->MODER |=  ((2U << 12) | (2U << 14) | (2U << 16));

            // 3. AF2(TIM4) 매핑
            GPIOB->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));
            GPIOB->AFR[0] |=  ((2U << 24) | (2U << 28));

            GPIOB->AFR[1] &= ~(0xFU << 0);
            GPIOB->AFR[1] |=  (2U << 0);

            // 4. Downcounting + ARR Preload 활성화
            TIM4->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정
            TIM4->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM4->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            TIM4->CCER = 0;

            // 6. PSC 계산 및 검증
            {
                double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;
                if (calculated_psc < 0.0 || calculated_psc > 65535.0)
                {
                    return false;
                }
                TIM4->PSC = (uint32_t)calculated_psc;
            }
            TIM4->ARR = arr - 1;

            TIM4->EGR |= TIM_EGR_UG;
            return true;

        //------- Timer 5 PWM (32-bit, APB1 Bus) Channel 1, 2 -----------
        case 5:
            // 1. TIM5 및 GPIOA 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

            // 2. PA0, PA1, PA2 대체 기능 설정
            GPIOA->MODER &= ~((3U << 0) | (3U << 2) | (3U << 4));
            GPIOA->MODER |=  ((2U << 0) | (2U << 2) | (2U << 4));

            // 3. AF2(TIM5) 매핑
            GPIOA->AFR[0] &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 8));
            GPIOA->AFR[0] |=  ((2U << 0) | (2U << 4) | (2U << 8));

            // 4. Downcounting + ARR Preload 활성화
            TIM5->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정
            TIM5->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM5->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            TIM5->CCER = 0;

            // 6. PSC 계산 및 검증
            {
                double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;
                if (calculated_psc < 0.0 || calculated_psc > 65535.0)
                {
                    return false;
                }
                TIM5->PSC = (uint32_t)calculated_psc;
            }
            TIM5->ARR = arr - 1;

            TIM5->EGR |= TIM_EGR_UG;
            return true;

        default:
            return false;
    }
}

bool mcal_pwm_freq_change(uint8_t pwm_instance, uint32_t freq)
{
    if (freq == 0)
    {
        return false;
    }

    switch (pwm_instance)
    {
        case 1:
            TIM1->PSC = (unsigned int)(96000000 / (double)freq + 0.5) - 1;
            return true;

        case 2:
            TIM2->PSC = (unsigned int)(96000000 / (double)freq + 0.5) - 1;
            return true;

        case 3:
            TIM3->PSC = (unsigned int)(96000000 / (double)freq + 0.5) - 1;
            return true;

        case 4:
            TIM4->PSC = (unsigned int)(96000000 / (double)freq + 0.5) - 1;
            return true;

        case 5:
            TIM5->PSC = (unsigned int)(96000000 / (double)freq + 0.5) - 1;
            return true;

        default:
            return false;
    }
}

bool mcal_pwm_arr_change(uint8_t pwm_instance, uint32_t arr)
{
    if (arr == 0)
    {
        return false;
    }

    switch (pwm_instance)
    {
        case 1:
            TIM1->ARR = arr - 1;
            return true;

        case 2:
            TIM2->ARR = arr - 1;
            return true;

        case 3:
            TIM3->ARR = arr - 1;
            return true;

        case 4:
            TIM4->ARR = arr - 1;
            return true;

        case 5:
            TIM5->ARR = arr - 1;
            return true;

        default:
            return false;
    }
}

bool mcal_pwm_start(uint8_t pwm_instance, uint8_t cch, uint8_t duty)
{
    switch (pwm_instance)
    {
        //------- Timer 1 PWM ----------------------------------------------------
        case 1:
            if (cch == 1)
            {
                TIM1->CCER |= TIM_CCER_CC1E; // (0x1UL << 0U)
                TIM1->CCR1 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
            }
            else if (cch == 2)
            {
                TIM1->CCER |= TIM_CCER_CC2E; // (0x1UL << 4U)
                TIM1->CCR2 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
            }
            else if (cch == 3)
            {
                TIM1->CCER |= TIM_CCER_CC3E; // (0x1UL << 8U)
                TIM1->CCR3 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
            }
            else
            {
                return false;
            }
            TIM1->CR1 |= TIM_CR1_CEN; // 0x1UL << 0U;
            return true;

        //------- Timer 2 PWM ----------------------------------------------------
        case 2:
            if (cch == 1)
            {
                TIM2->CCER |= TIM_CCER_CC1E;
                TIM2->CCR1 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
            }
            else if (cch == 2)
            {
                TIM2->CCER |= TIM_CCER_CC2E;
                TIM2->CCR2 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
            }
            else if (cch == 3)
            {
                TIM2->CCER |= TIM_CCER_CC3E;
                TIM2->CCR3 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
            }
            else
            {
                return false;
            }
            TIM2->CR1 |= TIM_CR1_CEN;
            return true;

        //------- Timer 3 PWM ----------------------------------------------------
        case 3:
            if (cch == 1)
            {
                TIM3->CCER |= TIM_CCER_CC1E;
                TIM3->CCR1 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
            }
            else if (cch == 2)
            {
                TIM3->CCER |= TIM_CCER_CC2E;
                TIM3->CCR2 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
            }
            else if (cch == 3)
            {
                TIM3->CCER |= TIM_CCER_CC3E;
                TIM3->CCR3 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
            }
            else
            {
                return false;
            }
            TIM3->CR1 |= TIM_CR1_CEN;
            return true;

        //------- Timer 4 PWM ----------------------------------------------------
        case 4:
            if (cch == 1)
            {
                TIM4->CCER |= TIM_CCER_CC1E;
                TIM4->CCR1 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
            }
            else if (cch == 2)
            {
                TIM4->CCER |= TIM_CCER_CC2E;
                TIM4->CCR2 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
            }
            else if (cch == 3)
            {
                TIM4->CCER |= TIM_CCER_CC3E;
                TIM4->CCR3 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
            }
            else
            {
                return false;
            }
            TIM4->CR1 |= TIM_CR1_CEN;
            return true;

        //------- Timer 5 PWM ----------------------------------------------------
        case 5:
            if (cch == 1)
            {
                TIM5->CCER |= TIM_CCER_CC1E;
                TIM5->CCR1 = (uint32_t)(TIM5->ARR + 1) * duty / 100;
            }
            else if (cch == 2)
            {
                TIM5->CCER |= TIM_CCER_CC2E;
                TIM5->CCR2 = (uint32_t)(TIM5->ARR + 1) * duty / 100;
            }
            else if (cch == 3)
            {
                // ST-Link 핀 간섭으로 비활성화
                return false;
            }
            else
            {
                return false;
            }
            TIM5->CR1 |= TIM_CR1_CEN;
            return true;

        default:
            return false;
    }
}

void mcal_pwm_stop(uint8_t pwm_instance, uint8_t cch)
{
    switch (pwm_instance)
    {
        case 1:
            if (cch == 1)
                TIM1->CCER &= ~TIM_CCER_CC1E; // (0x1UL << 0U)
            else if (cch == 2)
                TIM1->CCER &= ~TIM_CCER_CC2E; // (0x1UL << 4U)
            else if (cch == 3)
                TIM1->CCER &= ~TIM_CCER_CC3E; // (0x1UL << 8U)
            break;

        case 2:
            if (cch == 1)
                TIM2->CCER &= ~TIM_CCER_CC1E;
            else if (cch == 2)
                TIM2->CCER &= ~TIM_CCER_CC2E;
            else if (cch == 3)
                TIM2->CCER &= ~TIM_CCER_CC3E;
            break;

        case 3:
            if (cch == 1)
                TIM3->CCER &= ~TIM_CCER_CC1E;
            else if (cch == 2)
                TIM3->CCER &= ~TIM_CCER_CC2E;
            else if (cch == 3)
                TIM3->CCER &= ~TIM_CCER_CC3E;
            break;

        case 4:
            if (cch == 1)
                TIM4->CCER &= ~TIM_CCER_CC1E;
            else if (cch == 2)
                TIM4->CCER &= ~TIM_CCER_CC2E;
            else if (cch == 3)
                TIM4->CCER &= ~TIM_CCER_CC3E;
            break;

        case 5:
            if (cch == 1)
                TIM5->CCER &= ~TIM_CCER_CC1E;
            else if (cch == 2)
                TIM5->CCER &= ~TIM_CCER_CC2E;
            else if (cch == 3)
                TIM5->CCER &= ~TIM_CCER_CC3E;
            break;

        default:
            break;
    }
}