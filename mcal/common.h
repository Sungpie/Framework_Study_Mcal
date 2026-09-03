#ifndef MCAL_COMMON_H
#define MCAL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//  ========== Status ===========
typedef enum
{
    MCAL_OK = 0x00U,

    MCAL_ERROR = 0x01U

} mcal_status_t;

#endif /* MCAL_COMMON_H */