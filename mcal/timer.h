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
