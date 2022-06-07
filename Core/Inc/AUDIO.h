#ifndef __AUDIO_H
#define __AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "cs43l22.h"
#include "AUDIO_LINK.h"

#define CODEC_STANDARD                0x04
#define I2S_STANDARD                  I2S_STANDARD_PHILIPS


 typedef struct
 {
   uint32_t  (*Init)(uint16_t, uint16_t, uint8_t, uint32_t);
   void      (*DeInit)(void);
   uint32_t  (*ReadID)(uint16_t);
   uint32_t  (*Play)(uint16_t, uint16_t*, uint16_t);
   uint32_t  (*Pause)(uint16_t);
   uint32_t  (*Resume)(uint16_t);
   uint32_t  (*Stop)(uint16_t, uint32_t);
   uint32_t  (*SetFrequency)(uint16_t, uint32_t);
   uint32_t  (*SetVolume)(uint16_t, uint8_t);
   uint32_t  (*SetMute)(uint16_t, uint32_t);
   uint32_t  (*SetOutputMode)(uint16_t, uint8_t);
   uint32_t  (*Reset)(uint16_t);
 }AUDIO_DrvTypeDef;

#define I2S3                            SPI3
#define I2S3_CLK_ENABLE()               __HAL_RCC_SPI3_CLK_ENABLE()
#define I2S3_CLK_DISABLE()              __HAL_RCC_SPI3_CLK_DISABLE()
#define I2S3_SCK_SD_WS_AF               GPIO_AF6_SPI3
#define I2S3_SCK_SD_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2S3_MCK_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2S3_WS_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define I2S3_WS_PIN                     GPIO_PIN_4
#define I2S3_SCK_PIN                    GPIO_PIN_10
#define I2S3_SD_PIN                     GPIO_PIN_12
#define I2S3_MCK_PIN                    GPIO_PIN_7
#define I2S3_SCK_SD_GPIO_PORT           GPIOC
#define I2S3_WS_GPIO_PORT               GPIOA
#define I2S3_MCK_GPIO_PORT              GPIOC

#define I2S3_DMAx_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define I2S3_DMAx_CLK_DISABLE()         __HAL_RCC_DMA1_CLK_DISABLE()
#define I2S3_DMAx_STREAM                DMA1_Stream7
#define I2S3_DMAx_CHANNEL               DMA_CHANNEL_0
#define I2S3_DMAx_IRQ                   DMA1_Stream7_IRQn
#define I2S3_DMAx_PERIPH_DATA_SIZE      DMA_PDATAALIGN_HALFWORD
#define I2S3_DMAx_MEM_DATA_SIZE         DMA_MDATAALIGN_HALFWORD
#define DMA_MAX_SZE                     0xFFFF

#define I2S3_IRQHandler                 DMA1_Stream7_IRQHandler

#define AUDIO_OUT_IRQ_PREPRIO           0x0E

#define AUDIODATA_SIZE      2

#define AUDIO_OK                              0
#define AUDIO_ERROR                           1
#define AUDIO_TIMEOUT                         2

#define DMA_MAX(_X_)                (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)
#define HTONS(A)  ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))

uint8_t AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
uint8_t AUDIO_OUT_Play(uint16_t *pBuffer, uint32_t Size);
void    AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size);
uint8_t AUDIO_OUT_Pause(void);
uint8_t AUDIO_OUT_Resume(void);
uint8_t AUDIO_OUT_Stop(uint32_t Option);
uint8_t AUDIO_OUT_SetVolume(uint8_t Volume);
void    AUDIO_OUT_SetFrequency(uint32_t AudioFreq);
uint8_t AUDIO_OUT_SetMute(uint32_t Cmd);
uint8_t AUDIO_OUT_SetOutputMode(uint8_t Output);

void    AUDIO_OUT_TransferComplete_CallBack(void);

void    AUDIO_OUT_HalfTransfer_CallBack(void);

void    AUDIO_OUT_Error_CallBack(void);

void  AUDIO_OUT_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq, void *Params);
void  AUDIO_OUT_MspInit(I2S_HandleTypeDef *hi2s, void *Params);
void  AUDIO_OUT_MspDeInit(I2S_HandleTypeDef *hi2s, void *Params);

#ifdef __cplusplus
}
#endif

#endif
