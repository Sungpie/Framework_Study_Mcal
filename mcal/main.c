#include "stm32f4xx.h"
#include "common.h"
#include "timer.h"
#include "pwm.h"

volatile uint32_t second = 0;

void TIM1_UP_TIM10_IRQHandler(void)
{
    second++;
	MCAL_CLEAR_BIT(TIM1->SR, 0);
	NVIC_ClearPendingIRQ(25);
}

void Clock_Init(void)
{
    RCC->CR |= (1 << 0); 
    while(!MCAL_CHECK_BIT_SET(RCC->CR, 1));

    FLASH->ACR = (1<<12)|(1<<11);
    FLASH->ACR = (1<<10)|(1<<9)|(1<<8)|(0x3 << 0);

    RCC->PLLCFGR = (8<<24)|(0<<22)|(1<<16)|(192<<6)|(8<<0);

    MCAL_SET_BIT(RCC->CR, 24);
    while(!MCAL_CHECK_BIT_SET(RCC->CR, 25));

    RCC->CFGR = (0<<13)|(4<<10)|(0<<4);

    MCAL_WRITE_BLOCK(RCC->CFGR, 0x3, 0x2, 0);
    while(MCAL_EXTRACT_AREA(RCC->CFGR, 0x3, 2) != 0x2);
}


#if 1
static void Sys_Init(int baud) 
{
	SCB->CPACR |= (0x3 << 10*2)|(0x3 << 11*2); 
	Clock_Init();

	//Uart2_Init(baud);
	//setvbuf(stdout, NULL, _IONBF, 0);

	MCAL_SET_BIT(RCC->AHB1ENR, 0);
	MCAL_SET_BIT(GPIOA->MODER, 10);
	MCAL_CLEAR_BIT(GPIOA->ODR, 5);
	
}
#endif

void _Invalid_ISR(void) { while(1); }

void Main(void)
{
	Sys_Init(115200);

	// mcal_timer_repeat_init(0, 10000, 10000);
	// mcal_timer_start(0);

	// uint32_t last_second = 0;

	mcal_pwm_init(2, 1000, 1000);

	mcal_pwm_start(2, 1, 10);
	mcal_pwm_start(2, 2, 100);
	mcal_pwm_start(2, 3, 100);

	// // 1. GPIOA 클록 활성화
    // MCAL_SET_BIT(RCC->AHB1ENR, 0);

    // // 2. PA2 핀을 일반 출력 모드(MODER = 01)로 설정
    // GPIOA->MODER &= ~(3U << 4);
    // GPIOA->MODER |=  (1U << 4);

    // // 3. PA2 핀에 3.3V(High) 출력
    // MCAL_SET_BIT(GPIOA->ODR, 2);
	for(;;)
	{
		// if(second != last_second)
		// {
		// 	last_second = second;
		// 	MCAL_INVERT_BIT(GPIOA->ODR, 5);
		// }


	}
}