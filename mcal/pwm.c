#include "stm32f4xx.h"
#include "pwm.h"
#include "macro.h"

bool mcal_pwm_init(uint8_t pwm_instance, uint32_t freq, uint32_t arr)
{
    if ((freq > 0) && (freq <= 96000000U) && (arr > 0))
    {
        //------- Timer 1 PWM (16-bit, APB2 Bus) Channel 1 (PA8), Channel 2 (PA9), Channel 3 (PA10) -----------
        if ((pwm_instance == 1) && (arr <= 0xFFFFU))
        {
            // 1. TIM1(APB2 Bit 0) 및 GPIOA(AHB1 Bit 0) 클록 인가 ★ [주의: APB1이 아니라 APB2 버스임]
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  // TIM1 클록 켜기 (Bit 0)
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // PA8, PA9, PA10 사용을 위한 GPIOA 클록 켜기 (Bit 0)

            // 2. GPIO 모드 설정 (PA8, PA9, PA10을 AF 모드 = 2로 설정)
            GPIOA->MODER &= ~((3U << 16) | (3U << 18) | (3U << 20));
            GPIOA->MODER |=  ((2U << 16) | (2U << 18) | (2U << 20));

            // 3. Alternate Function 매핑 (TIM1은 AF1 = 1)
            GPIOA->AFR[1] &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 8));
            GPIOA->AFR[1] |=  ((1U << 0) | (1U << 4) | (1U << 8)); // AF1 매핑

            // 4. Downcounting + ARR Preload 활성화 (CMSIS: TIM_CR1_DIR | TIM_CR1_ARPE)
            TIM1->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정 (PWM 모드 1 & Preload 활성화)
            // CCMR1: CH1 (PWM1 | OC1PE), CH2 (PWM1 | OC2PE)
            TIM1->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            // CCMR2: CH3 (PWM1 | OC3PE)
            TIM1->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            // 6. 극성 초기화 (Active High)
            TIM1->CCER = 0;

            // 7. [고급 타이머 필수] 메인 출력 활성화 (MOE = Main Output Enable)
            TIM1->BDTR |= TIM_BDTR_MOE;

            // 8. PSC 계산 (반올림 포함, 96MHz 기준)
            double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;

            // 9. PSC 레지스터 유효 범위(16비트: 0 ~ 65535) 검증
            if (calculated_psc < 0.0 || calculated_psc > 65535.0)
            {
                return false; // 설정 불가능한 주파수/ARR 조합 방어
            }

            // 10. 레지스터 값 적용
            TIM1->PSC = (uint32_t)calculated_psc;
            TIM1->ARR = arr - 1;

            // 11. Manual Update(UG 발생)로 레지스터 값 즉시 하드웨어에 로드
            TIM1->EGR |= TIM_EGR_UG;

            return true;
        }

        //------- Timer 2 PWM (32-bit, APB1 Bus) Channel 1 (PA15), Channel 2 (PB3), Channel 3 (PB10) -----------
        else if ((pwm_instance == 2) && (arr <= 0xFFFFU))
        {
            // 1. TIM2(APB1 Bit 0) 및 GPIOA(AHB1 Bit 0), GPIOB(AHB1 Bit 1) 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // TIM2 클록 켜기 (Bit 0)
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // PA15 사용을 위한 GPIOA 클록 (Bit 0)
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // PB3, PB10 사용을 위한 GPIOB 클록 (Bit 1)

            // 2. GPIO 모드 설정 (MODER: 핀 번호 * 2 비트 위치, AF 모드 = 2)
            GPIOA->MODER &= ~(3U << 30);
            GPIOA->MODER |=  (2U << 30);

            GPIOB->MODER &= ~((3U << 6) | (3U << 20));
            GPIOB->MODER |=  ((2U << 6) | (2U << 20));

            // 3. Alternate Function 매핑 (TIM2는 모든 채널이 AF1 = 1)
            GPIOA->AFR[1] &= ~(0xFU << 28);
            GPIOA->AFR[1] |=  (1U << 28); // AF1 매핑

            GPIOB->AFR[0] &= ~(0xFU << 12);
            GPIOB->AFR[0] |=  (1U << 12); // AF1 매핑

            GPIOB->AFR[1] &= ~(0xFU << 8);
            GPIOB->AFR[1] |=  (1U << 8);  // AF1 매핑

            // 4. Downcounting + ARR Preload 활성화
            TIM2->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Timer 2 Channel 1, 2 (CCMR1) 및 Channel 3 (CCMR2) 설정 (PWM 모드 1 & Preload 활성화)
            TIM2->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM2->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            // 6. 극성 초기화 (Active High)
            TIM2->CCER = 0;

            // 7. PSC 계산 (반올림 포함, 96MHz 기준)
            double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;

            // 8. PSC 레지스터 유효 범위(16비트: 0 ~ 65535) 검증
            if (calculated_psc < 0.0 || calculated_psc > 65535.0)
            {
                return false; // 설정 불가능한 주파수/ARR 조합 방어
            }

            // 9. 레지스터 값 적용
            TIM2->PSC = (uint32_t)calculated_psc;
            TIM2->ARR = arr - 1;

            // 10. Manual Update(UG 발생)로 설정값 즉시 하드웨어에 로드
            TIM2->EGR |= TIM_EGR_UG;

            return true;
        }

        //------- Timer 3 PWM (16-bit, APB1 Bus) Channel 1, 2, 3 (PC6, PC7, PC8) -----------
        else if ((pwm_instance == 3) && (arr <= 0xFFFFU))
        {
            // 1. TIM3(APB1 Bit 1) 및 GPIOC(AHB1 Bit 2) 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

            // 2. PC6, PC7, PC8을 대체 기능(AF 모드 = 2)으로 설정
            GPIOC->MODER &= ~((3U << 12) | (3U << 14) | (3U << 16));
            GPIOC->MODER |=  ((2U << 12) | (2U << 14) | (2U << 16));

            // 3. PC6, PC7, PC8 핀에 AF2(TIM3 기능) 매핑
            GPIOC->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));
            GPIOC->AFR[0] |=  ((2U << 24) | (2U << 28));

            GPIOC->AFR[1] &= ~(0xFU << 0);
            GPIOC->AFR[1] |=  (2U << 0);

            // 4. Downcounting + ARR Preload 활성화
            TIM3->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Timer 3 Channel 1, 2 (CCMR1) 및 Channel 3 (CCMR2) 설정 (PWM 모드 1 & Preload 활성화)
            TIM3->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM3->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            // 6. 극성 초기화 (Active High)
            TIM3->CCER = 0;

            // 7. PSC 계산 (반올림 포함)
            double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;

            // 8. PSC 레지스터 유효 범위(16비트: 0 ~ 65535) 검증
            if (calculated_psc < 0.0 || calculated_psc > 65535.0)
            {
                return false; // 설정 불가능한 주파수/ARR 조합 방어
            }

            // 9. 레지스터 값 적용
            TIM3->PSC = (uint32_t)calculated_psc;
            TIM3->ARR = arr - 1;

            // 10. Manual Update(UG 발생)로 레지스터 값 즉시 로드
            TIM3->EGR |= TIM_EGR_UG;

            return true;
        }

        //------- Timer 4 PWM (16-bit, APB1 Bus) Channel 1 (PB6), Channel 2 (PB7), Channel 3 (PB8) -----------
        else if ((pwm_instance == 4) && (arr <= 0xFFFFU))
        {
            // 1. TIM4(APB1 Bit 2) 및 GPIOB(AHB1 Bit 1) 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;  // TIM4 클록 활성화
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // GPIOB 클록 활성화

            // 2. GPIO 모드 설정: PB6, PB7, PB8을 대체 기능(AF 모드 = 2)으로 설정
            GPIOB->MODER &= ~((3U << 12) | (3U << 14) | (3U << 16));
            GPIOB->MODER |=  ((2U << 12) | (2U << 14) | (2U << 16));

            // 3. Alternate Function 매핑 (TIM4 기능 = AF2)
            GPIOB->AFR[0] &= ~((0xFU << 24) | (0xFU << 28));
            GPIOB->AFR[0] |=  ((2U << 24) | (2U << 28)); // AF2 매핑

            GPIOB->AFR[1] &= ~(0xFU << 0);
            GPIOB->AFR[1] |=  (2U << 0);                  // AF2 매핑

            // 4. Downcounting + ARR Preload 활성화
            TIM4->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // 5. Output Compare 모드 설정 (PWM 모드 1 & Preload 활성화)
            TIM4->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM4->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            // 6. 극성 초기화 (Active High)
            TIM4->CCER = 0;

            // 7. PSC 계산 (반올림 포함, 96MHz 기준)
            double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;

            // 8. PSC 레지스터 유효 범위(16비트: 0 ~ 65535) 검증
            if (calculated_psc < 0.0 || calculated_psc > 65535.0)
            {
                return false; // 설정 불가능한 주파수/ARR 조합 방어
            }

            // 9. 레지스터 값 적용
            TIM4->PSC = (uint32_t)calculated_psc;
            TIM4->ARR = arr - 1;

            // 10. Manual Update(UG 발생)로 레지스터 값 즉시 로드
            TIM4->EGR |= TIM_EGR_UG;

            return true;
        }   

         //------- Timer 5 PWM (32-bit, APB1 Bus) Channel 1 2 -----------
        else if ((pwm_instance == 5))
        {
            // 1. TIM5(APB1 Bit 3) 및 GPIOA(AHB1 Bit 0) 클록 인가
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

            GPIOA->MODER &= ~((3U << 0) | (3U << 2) | (3U << 4));
            GPIOA->MODER |=  ((2U << 0) | (2U << 2) | (2U << 4));

            // 3. PA0, PA1, PA2 핀에 AF2(TIM5 기능) 매핑
            GPIOA->AFR[0] &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 8));
            GPIOA->AFR[0] |=  ((2U << 0) | (2U << 4) | (2U << 8));

            // Downcounting + ARR Preload 활성화
            TIM5->CR1 = TIM_CR1_DIR | TIM_CR1_ARPE;

            // Timer 5 Channel 1, 2, 3 설정 (PWM 모드 1 & Preload 활성화)
            TIM5->CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE) |
                          (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);
            TIM5->CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

            TIM5->CCER = 0;

            // 1. PSC 계산 (반올림 포함)
            double calculated_psc = (96000000.0 / ((double)freq * (double)arr) + 0.5) - 1.0;

            // 2. PSC 레지스터 유효 범위(16비트: 0 ~ 65535) 검증
            if (calculated_psc < 0.0 || calculated_psc > 65535.0)
            {
                return false; // 설정 불가능한 주파수/ARR 조합 방어
            }

            // 3. 레지스터 값 적용
            TIM5->PSC = (uint32_t)calculated_psc;
            TIM5->ARR = arr - 1;

            // Manual Update(UG 발생)
            TIM5->EGR |= TIM_EGR_UG;

            return true;
        }
        return false;
    }
    return false;
} 

bool mcal_pwm_freq_change(uint8_t pwm_instance, uint32_t freq)
{
    if(freq == 0)
        return false;

    //------- Timer 1 PWM ----------------------------------------------------
    else if(pwm_instance == 1)
    {
        TIM1->PSC = (unsigned int)(96000000/(double)freq + 0.5) - 1;

        return true;
    }

    //------- Timer 2 PWM ----------------------------------------------------
    else if(pwm_instance == 2)
    {
        TIM2->PSC = (unsigned int)(96000000/(double)freq + 0.5) - 1;

        return true;
    }

    //------- Timer 3 PWM ----------------------------------------------------
    else if(pwm_instance == 3)
    {
        TIM3->PSC = (unsigned int)(96000000/(double)freq + 0.5) - 1;

        return true;
    }

    //------- Timer 4 PWM ----------------------------------------------------
    else if(pwm_instance == 4)
    {
        TIM4->PSC = (unsigned int)(96000000/(double)freq + 0.5) - 1;

        return true;
    }

    //------- Timer 5 PWM ----------------------------------------------------
    else if(pwm_instance == 5)
    {
        TIM5->PSC = (unsigned int)(96000000/(double)freq + 0.5) - 1;

        return true;
    }

    return false;
}

bool mcal_pwm_arr_change(uint8_t pwm_instance, uint32_t arr)
{
    if(arr == 0)
        return false;

    //------- Timer 1 PWM ----------------------------------------------------
    else if(pwm_instance == 1)
    {
        TIM1->ARR = arr - 1;
        
        return true;
    }

    //------- Timer 2 PWM ----------------------------------------------------
    else if(pwm_instance == 2)
    {
        TIM2->ARR = arr - 1;
        
        return true;
    }

    //------- Timer 3 PWM ----------------------------------------------------
    else if(pwm_instance == 3)
    {
        TIM3->ARR = arr - 1;
        
        return true;
    }

    //------- Timer 4 PWM ----------------------------------------------------
    else if(pwm_instance == 4)
    {
        TIM4->ARR = arr - 1;
        
        return true;
    }

    //------- Timer 5 PWM ----------------------------------------------------
    else if(pwm_instance == 5)
    {
        TIM5->ARR = arr - 1;
        
        return true;
    }
    return false;
}

// 센서값을 받아올 때, 계속해서 갱신해야 하므로 start 함수에 duty 값이 있어야 함.
bool mcal_pwm_start(uint8_t pwm_instance, uint8_t cch, uint8_t duty)
{
    //------- Timer 1 PWM ----------------------------------------------------
    if(pwm_instance == 1)
    {
        if(cch == 1)
        {
            // Stop 후에 다시 Start 하게 되면 channel 켜줘야 함. (CMSIS: TIM_CCER_CC1E)
            TIM1->CCER |= TIM_CCER_CC1E;
            TIM1->CCR1 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
        }
            
        else if(cch == 2)
        {
            TIM1->CCER |= TIM_CCER_CC2E;
            TIM1->CCR2 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
        }
	         
        else if(cch == 3)
        {
            TIM1->CCER |= TIM_CCER_CC3E;    
            TIM1->CCR3 = (uint32_t)(TIM1->ARR + 1) * duty / 100;
        }
        else
            return false;

        // PWM 파형 생성 시작 (CMSIS: TIM_CR1_CEN)
        TIM1->CR1 |= TIM_CR1_CEN;

        return true;
    }

    //------- Timer 2 PWM ----------------------------------------------------
    if(pwm_instance == 2)
    {
        if(cch == 1)
        {
            TIM2->CCER |= TIM_CCER_CC1E;
            TIM2->CCR1 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
        }
            
        else if(cch == 2)
        {
            TIM2->CCER |= TIM_CCER_CC2E;
            TIM2->CCR2 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
        }
	         
        else if(cch == 3)
        {
            TIM2->CCER |= TIM_CCER_CC3E;    
            TIM2->CCR3 = (uint32_t)(TIM2->ARR + 1) * duty / 100;
        }
        else
            return false;

        TIM2->CR1 |= TIM_CR1_CEN;

        return true;
    }
    
    //------- Timer 3 PWM ----------------------------------------------------
    else if(pwm_instance == 3)
    {
        if(cch == 1)
        {
            TIM3->CCER |= TIM_CCER_CC1E;
            TIM3->CCR1 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
        }
            
        else if(cch == 2)
        {
            TIM3->CCER |= TIM_CCER_CC2E;
            TIM3->CCR2 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
        }
	         
        else if(cch == 3)
        {
            TIM3->CCER |= TIM_CCER_CC3E;    
            TIM3->CCR3 = (uint32_t)(TIM3->ARR + 1) * duty / 100;
        }
        else
            return false;

        TIM3->CR1 |= TIM_CR1_CEN;

        return true;
    }
    
    //------- Timer 4 PWM ----------------------------------------------------
    else if(pwm_instance == 4)
    {
        if(cch == 1)
        {
            TIM4->CCER |= TIM_CCER_CC1E;
            TIM4->CCR1 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
        }
            
        else if(cch == 2)
        {
            TIM4->CCER |= TIM_CCER_CC2E;
            TIM4->CCR2 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
        }
	         
        else if(cch == 3)
        {
            TIM4->CCER |= TIM_CCER_CC3E;    
            TIM4->CCR3 = (uint32_t)(TIM4->ARR + 1) * duty / 100;
        }
            
        else
            return false;

        TIM4->CR1 |= TIM_CR1_CEN;

        return true;
    }

    //------- Timer 5 PWM ----------------------------------------------------
    else if(pwm_instance == 5)
    {
        if(cch == 1)
        {
            TIM5->CCER |= TIM_CCER_CC1E;
            TIM5->CCR1 = (uint32_t)(TIM5->ARR + 1) * duty / 100;
        }
            
        else if(cch == 2)
        {
            TIM5->CCER |= TIM_CCER_CC2E;
            TIM5->CCR2 = (uint32_t)(TIM5->ARR + 1) * duty / 100;
        }
	         
        else if(cch == 3)
        {
            // ST Link 때문에 사용 X
            return false;
        }
            
        else
            return false;

        TIM5->CR1 |= TIM_CR1_CEN;

        return true;
    }
    return false;
}

void mcal_pwm_stop(uint8_t pwm_instance, uint8_t cch)
{
    if(pwm_instance == 1)
    {
        if(cch == 1)
            TIM1->CCER &= ~TIM_CCER_CC1E;
        
        else if(cch == 2)
            TIM1->CCER &= ~TIM_CCER_CC2E;

        else if(cch == 3)
            TIM1->CCER &= ~TIM_CCER_CC3E;
    }

    else if(pwm_instance == 2)
    {
        if(cch == 1)
            TIM2->CCER &= ~TIM_CCER_CC1E;
        
        else if(cch == 2)
            TIM2->CCER &= ~TIM_CCER_CC2E;

        else if(cch == 3)
            TIM2->CCER &= ~TIM_CCER_CC3E;
    }

    else if(pwm_instance == 3)
    {
        if(cch == 1)
            TIM3->CCER &= ~TIM_CCER_CC1E;
        
        else if(cch == 2)
            TIM3->CCER &= ~TIM_CCER_CC2E;

        else if(cch == 3)
            TIM3->CCER &= ~TIM_CCER_CC3E;
    }

    else if(pwm_instance == 4)
    {
        if(cch == 1)
            TIM4->CCER &= ~TIM_CCER_CC1E;
        
        else if(cch == 2)
            TIM4->CCER &= ~TIM_CCER_CC2E;

        else if(cch == 3)
            TIM4->CCER &= ~TIM_CCER_CC3E;
    }

    else if(pwm_instance == 5)
    {
        if(cch == 1)
            TIM5->CCER &= ~TIM_CCER_CC1E;
        
        else if(cch == 2)
            TIM5->CCER &= ~TIM_CCER_CC2E;

        else if(cch == 3)
            TIM5->CCER &= ~TIM_CCER_CC3E;
    }
}