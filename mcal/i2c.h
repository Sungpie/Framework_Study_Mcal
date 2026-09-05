#include "common.h"

typedef enum {
    MCAL_I2C_OK = 0,  /* 정상적으로 통신 완료 */
    MCAL_I2C_ERROR,   /* NACK 수신 또는 주소 불일치 등 오류 */
    MCAL_I2C_BUSY,    /* 아직 통신 중 */
    MCAL_I2C_TIMEOUT  /* 센서 응답 지연 */
} mcal_i2c_status_t;

bool mcal_i2c_init(uint8_t channel, uint8_t mode, uint32_t Freq);

mcal_i2c_status_t mcal_i2c_write(uint8_t channel, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint16_t len, uint16_t timeout);
mcal_i2c_status_t mcal_i2c_read(uint8_t channel, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len, uint16_t timeout);