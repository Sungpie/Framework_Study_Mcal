#include "stm32f4xx.h"
#include "timer.h"

bool mcal_timer_repeat_init(uint8_t timer_instance, uint32_t freq, uint32_t arr)
{
    // 1차 매개변수 유효 범위 확인 (0 방어 및 최대 주파수 체크)
    if ((freq > 0) && (freq <= 96000000U) && (arr > 0))
    {
        //------- Timer 1 (16-bit, APB2 Bus) ----------------------------
        if ((timer_instance == 1) && (arr <= 0xFFFFU))
        {
            // Timer 1 Clock Enable
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

            // Downcounting (DIR=1), Repeat Mode (OPM=0)
            TIM1->CR1 = TIM_CR1_DIR;

            // 분주비(PSC) 및 ARR 설정
            TIM1->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM1->ARR = arr - 1;

            return true;
        }

        //------- Timer 2 (32-bit, APB1 Bus) ----------------------------
        else if (timer_instance == 2)
        {
            // Timer 2 Clock Enable
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

            // Downcounting (DIR=1), Repeat Mode (OPM=0)
            TIM2->CR1 = TIM_CR1_DIR;

            // 분주비 및 ARR 설정
            TIM2->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM2->ARR = arr - 1;

            return true;
        } 
        
        //------- Timer 3 (16-bit, APB1 Bus) ----------------------------
        else if ((timer_instance == 3) && (arr <= 0xFFFFU))
        {
            // Timer 3 Clock Enable
            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

            // Downcounting (DIR=1), Repeat Mode (OPM=0)
            TIM3->CR1 = TIM_CR1_DIR;

            // 분주비 및 ARR 설정
            TIM3->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM3->ARR = arr - 1;

            return true;
        }
        
        //------- Timer 4 (16-bit, APB1 Bus) ----------------------------
        else if ((timer_instance == 4) && (arr <= 0xFFFFU))
        {
            // Timer 4 Clock Enable
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

            // Downcounting (DIR=1), Repeat Mode (OPM=0)
            TIM4->CR1 = TIM_CR1_DIR;

            // 분주비 및 ARR 설정
            TIM4->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM4->ARR = arr - 1;

            return true;
        }

        //------- Timer 5 (32-bit, APB1 Bus) ----------------------------
        else if (timer_instance == 5)
        {
            // Timer 5 Clock Enable
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

            // Downcounting (DIR=1), Repeat Mode (OPM=0)
            TIM5->CR1 = TIM_CR1_DIR;

            // 분주비 및 ARR 설정
            TIM5->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM5->ARR = arr - 1;

            return true;
        }
        return false;
    }
    return false;
}

bool mcal_timer_oneshot_init(uint8_t timer_instance, uint32_t freq, uint32_t arr)
{
    // 1차 매개변수 유효 범위 확인
    if ((freq > 0) && (freq <= 96000000U) && (arr > 0))
    {
        //------- Timer 1 (16-bit, APB2 Bus) ----------------------------
        if ((timer_instance == 1) && (arr <= 0xFFFFU))
        {
            // Timer 1 Clock Enable
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

            // Downcounting (DIR=1), One-shot Mode (OPM=1)
            TIM1->CR1 = (TIM_CR1_DIR | TIM_CR1_OPM);

            // 분주비 및 ARR 설정
            TIM1->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM1->ARR = arr - 1;

            return true;
        }

        //------- Timer 2 (32-bit, APB1 Bus) ----------------------------
        else if (timer_instance == 2)
        {
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

            TIM2->CR1 = (TIM_CR1_DIR | TIM_CR1_OPM);

            TIM2->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM2->ARR = arr - 1;

            return true;
        } 
        
        //------- Timer 3 (16-bit, APB1 Bus) ----------------------------
        else if ((timer_instance == 3) && (arr <= 0xFFFFU))
        {
            RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

            TIM3->CR1 = (TIM_CR1_DIR | TIM_CR1_OPM);

            TIM3->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM3->ARR = arr - 1;

            return true;
        }
        
        //------- Timer 4 (16-bit, APB1 Bus) ----------------------------
        else if ((timer_instance == 4) && (arr <= 0xFFFFU))
        {
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

            TIM4->CR1 = (TIM_CR1_DIR | TIM_CR1_OPM);

            TIM4->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM4->ARR = arr - 1;

            return true;
        }

        //------- Timer 5 (32-bit, APB1 Bus) ----------------------------
        else if (timer_instance == 5)
        {
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

            TIM5->CR1 = (TIM_CR1_DIR | TIM_CR1_OPM);

            TIM5->PSC = (uint32_t)(96000000.0f / (float)freq + 0.5f) - 1;
            TIM5->ARR = arr - 1;

            return true;
        }
        return false;
    }
    return false;
}

bool mcal_timer_int_enable(uint8_t timer_instance, bool act)
{
    //------- Timer 1 -------------------------------
    if(timer_instance == 1)
    {
        if(act)
        {
            // 1. 강제 갱신 등으로 남아있을 수 있는 찌꺼기 플래그 청소 (UIF 클리어)
            TIM1->SR &= ~TIM_SR_UIF;

            // 2. CPU 인터럽트 대기열 청소 (25번 창구)
            NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn);

            // 3. 타이머 본체의 Update 인터럽트 출력 스위치 ON (UIE 세트)
            TIM1->DIER |= TIM_DIER_UIE;

            // 4. CPU 코어 인터럽트 관문 활성화 (25번 활성화)
            NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

            return true;
        }

        else
        {
            // 1. 타이머 본체의 인터럽트 출력 스위치 OFF (UIE 해제)
            TIM1->DIER &= ~TIM_DIER_UIE;

            // 2. CPU 코어 인터럽트 관문 차단 (25번 비활성화)
            NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);

            // 3. 혹시 모를 잔여 펜딩 쪽지 파기
            NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn);

            return true;
        } 
    }

    //------- Timer 2 -------------------------------
    else if(timer_instance == 2)
    {
        if(act)
        {
            // 1. TIM2 갱신 인터럽트 플래그 청소 (UIF 클리어)
            TIM2->SR &= ~TIM_SR_UIF;

            // 2. NVIC 코어 대기열 청소 (TIM2_IRQn = 28번)
            NVIC_ClearPendingIRQ(TIM2_IRQn);

            // 3. TIM2 갱신 인터럽트 허용 (UIE 세트)
            TIM2->DIER |= TIM_DIER_UIE;

            // 4. NVIC 코어 인터럽트 관문 활성화
            NVIC_EnableIRQ(TIM2_IRQn);

            return true;
        }

        else
        {
            // 1. 타이머 본체의 인터럽트 출력 스위치 OFF (UIE 해제)
            TIM2->DIER &= ~TIM_DIER_UIE;

            // 2. CPU 코어 인터럽트 관문 차단 (28번 비활성화)
            NVIC_DisableIRQ(TIM2_IRQn);

            // 3. 혹시 모를 잔여 펜딩 쪽지 파기
            NVIC_ClearPendingIRQ(TIM2_IRQn);

            return true;
        }
    }

    //------- Timer 3 -------------------------------
    else if(timer_instance == 3)
    {
        if(act)
        {
            // 1. TIM4 갱신 인터럽트 플래그 청소 (UIF 클리어)
            TIM3->SR &= ~TIM_SR_UIF;

            // 2. NVIC 코어 대기열 청소 (TIM2_IRQn = 29번)
            NVIC_ClearPendingIRQ(TIM3_IRQn);

            // 3. TIM3 갱신 인터럽트 허용 (UIE 세트)
            TIM3->DIER |= TIM_DIER_UIE;

            // 4. NVIC 코어 인터럽트 관문 활성화
            NVIC_EnableIRQ(TIM3_IRQn);

            return true;
        }

        else
        {
            // 1. 타이머 본체의 인터럽트 출력 스위치 OFF (UIE 해제)
            TIM3->DIER &= ~TIM_DIER_UIE;

            // 2. CPU 코어 인터럽트 관문 차단 (29번 비활성화)
            NVIC_DisableIRQ(TIM3_IRQn);

            // 3. 혹시 모를 잔여 펜딩 쪽지 파기
            NVIC_ClearPendingIRQ(TIM3_IRQn);

            return true;
        }
    }

    //------- Timer 4 -------------------------------
    else if(timer_instance == 4)
    {
        if(act)
        {
            // 1. TIM4 갱신 인터럽트 플래그 청소 (UIF 클리어)
            TIM4->SR &= ~TIM_SR_UIF;

            // 2. NVIC 코어 대기열 청소 (TIM4_IRQn = 30번)
            NVIC_ClearPendingIRQ(TIM4_IRQn);

            // 3. TIM4 갱신 인터럽트 허용 (UIE 세트)
            TIM4->DIER |= TIM_DIER_UIE;

            // 4. NVIC 코어 인터럽트 관문 활성화
            NVIC_EnableIRQ(TIM4_IRQn);

            return true;
        }

        else
        {
            // 1. 타이머 본체의 인터럽트 출력 스위치 OFF (UIE 해제)
            TIM4->DIER &= ~TIM_DIER_UIE;

            // 2. CPU 코어 인터럽트 관문 차단 (30번 비활성화)
            NVIC_DisableIRQ(TIM4_IRQn);

            // 3. 혹시 모를 잔여 펜딩 쪽지 파기
            NVIC_ClearPendingIRQ(TIM4_IRQn);

            return true;
        }
    }

    //------- Timer 5 -------------------------------
    else if(timer_instance == 5)
    {
        if(act)
        {
            // 1. TIM5 갱신 인터럽트 플래그 청소 (UIF 클리어)
            TIM5->SR &= ~TIM_SR_UIF;

            // 2. NVIC 코어 대기열 청소 (TIM5_IRQn = 50번)
            NVIC_ClearPendingIRQ(TIM5_IRQn);

            // 3. TIM5 갱신 인터럽트 허용 (UIE 세트)
            TIM5->DIER |= TIM_DIER_UIE;

            // 4. NVIC 코어 인터럽트 관문 활성화
            NVIC_EnableIRQ(TIM5_IRQn);

            return true;
        }

        else
        {
            // 1. 타이머 본체의 인터럽트 출력 스위치 OFF (UIE 해제)
            TIM5->DIER &= ~TIM_DIER_UIE;

            // 2. CPU 코어 인터럽트 관문 차단 (50번 비활성화)
            NVIC_DisableIRQ(TIM5_IRQn);

            // 3. 혹시 모를 잔여 펜딩 쪽지 파기
            NVIC_ClearPendingIRQ(TIM5_IRQn);

            return true;
        }
        
    }

    return false;
}

bool mcal_timer_start(uint8_t timer_instance)
{
    //------- Timer 1 -------------------------------
    if (timer_instance == 1)
    {
        // 1. ARR/PSC 강제 즉시 적재 (영점 조절)
        TIM1->EGR |= TIM_EGR_UG;

        // 2. 강제 갱신으로 발생한 UIF 플래그 클리어
        TIM1->SR &= ~TIM_SR_UIF;

        // 3. NVIC 대기열 청소 및 코어 인터럽트 활성화
        NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn);
        NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

        // 4. 타이머 인터럽트 허용 및 카운터 출발
        TIM1->DIER |= TIM_DIER_UIE;
        TIM1->CR1  |= TIM_CR1_CEN;

        return true;
    }
    
    //------- Timer 2 -------------------------------
    else if (timer_instance == 2)
    {
        TIM2->EGR |= TIM_EGR_UG;
        TIM2->SR  &= ~TIM_SR_UIF;

        NVIC_ClearPendingIRQ(TIM2_IRQn);
        NVIC_EnableIRQ(TIM2_IRQn);

        TIM2->DIER |= TIM_DIER_UIE;
        TIM2->CR1  |= TIM_CR1_CEN;

        return true;
    }

    //------- Timer 3 -------------------------------
    else if (timer_instance == 3)
    {
        TIM3->EGR |= TIM_EGR_UG;
        TIM3->SR  &= ~TIM_SR_UIF;

        NVIC_ClearPendingIRQ(TIM3_IRQn);
        NVIC_EnableIRQ(TIM3_IRQn);

        TIM3->DIER |= TIM_DIER_UIE;
        TIM3->CR1  |= TIM_CR1_CEN;

        return true;
    }

    //------- Timer 4 -------------------------------
    else if (timer_instance == 4)
    {
        TIM4->EGR |= TIM_EGR_UG;
        TIM4->SR  &= ~TIM_SR_UIF;

        NVIC_ClearPendingIRQ(TIM4_IRQn);
        NVIC_EnableIRQ(TIM4_IRQn);

        TIM4->DIER |= TIM_DIER_UIE;
        TIM4->CR1  |= TIM_CR1_CEN;

        return true;
    }

    //------- Timer 5 -------------------------------
    else if (timer_instance == 5)
    {
        TIM5->EGR |= TIM_EGR_UG;
        TIM5->SR  &= ~TIM_SR_UIF;

        NVIC_ClearPendingIRQ(TIM5_IRQn);
        NVIC_EnableIRQ(TIM5_IRQn);

        TIM5->DIER |= TIM_DIER_UIE;
        TIM5->CR1  |= TIM_CR1_CEN;

        return true;
    }

    return false;
}

void mcal_timer_stop(uint8_t timer_instance)
{
    //------- Timer 1 -------------------------------
    if (timer_instance == 1)
    {
        TIM1->CR1  &= ~TIM_CR1_CEN;
        TIM1->DIER &= ~TIM_DIER_UIE;
        NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    }
    
    //------- Timer 2 -------------------------------
    else if (timer_instance == 2)
    {
        TIM2->CR1  &= ~TIM_CR1_CEN;
        TIM2->DIER &= ~TIM_DIER_UIE;
        NVIC_DisableIRQ(TIM2_IRQn);
    }

    //------- Timer 3 -------------------------------
    else if (timer_instance == 3)
    {
        TIM3->CR1  &= ~TIM_CR1_CEN;
        TIM3->DIER &= ~TIM_DIER_UIE;
        NVIC_DisableIRQ(TIM3_IRQn);
    }

    //------- Timer 4 -------------------------------
    else if (timer_instance == 4)
    {
        TIM4->CR1  &= ~TIM_CR1_CEN;
        TIM4->DIER &= ~TIM_DIER_UIE;
        NVIC_DisableIRQ(TIM4_IRQn);
    }

    //------- Timer 5 -------------------------------
    else if (timer_instance == 5)
    {
        TIM5->CR1  &= ~TIM_CR1_CEN;
        TIM5->DIER &= ~TIM_DIER_UIE;
        NVIC_DisableIRQ(TIM5_IRQn);
    }
}
