#include "common.h"
#include "macro.h"

bool mcal_pwm_init(uint8_t pwm_instance, uint32_t freq, uint32_t arr); //일단 성공 실패만

bool mcal_pwm_freq_change(uint8_t pwm_instance, uint32_t freq); //arr까지 변경하면 사용 시 매개변수가 헷갈릴 수 있어보여서
bool mcal_pwm_arr_change(uint8_t pwm_instance, uint32_t arr);

bool mcal_pwm_start(uint8_t pwm_instance, uint8_t cch, uint8_t duty);
void mcal_pwm_stop(uint8_t pwm_instance, uint8_t cch);