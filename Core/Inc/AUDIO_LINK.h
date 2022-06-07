#ifndef __AUDIO_LINK_H
#define __AUDIO_LINK_H

#ifdef __cplusplus
 extern "C" {
#endif
                                              
#include "stm32f4xx_hal.h"

#define AUDIO_I2Cx                          I2C1
#define AUDIO_I2Cx_CLOCK_ENABLE()           __HAL_RCC_I2C1_CLK_ENABLE()
#define AUDIO_I2Cx_GPIO_PORT                GPIOB                       /* GPIOB */
#define AUDIO_I2Cx_SCL_PIN                  GPIO_PIN_6                  /* PB.06 */
#define AUDIO_I2Cx_SDA_PIN                  GPIO_PIN_9                  /* PB.09 */
#define AUDIO_I2Cx_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define AUDIO_I2Cx_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()
#define AUDIO_I2Cx_AF                       GPIO_AF4_I2C1

#define AUDIO_I2Cx_FORCE_RESET()            __HAL_RCC_I2C1_FORCE_RESET()
#define AUDIO_I2Cx_RELEASE_RESET()          __HAL_RCC_I2C1_RELEASE_RESET()

#define AUDIO_I2Cx_EV_IRQn                  I2C1_EV_IRQn
#define AUDIO_I2Cx_ER_IRQn                  I2C1_ER_IRQn

#define I2Cx_TIMEOUT_MAX                        0xA000 /*<! The value of the maximal timeout for I2C waiting loops */
#define I2Cx_MAX_COMMUNICATION_FREQ             ((uint32_t) 100000)


#define AUDIO_I2C_ADDRESS                       0x94

#define CODEC_AUDIO_POWER_OFF()      HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_RESET)
#define CODEC_AUDIO_POWER_ON()       HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_SET)

#define AUDIO_RESET_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define AUDIO_RESET_PIN                         GPIO_PIN_4
#define AUDIO_RESET_GPIO                        GPIOD

#ifdef __cplusplus
}
#endif

#endif
