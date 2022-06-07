#include "AUDIO.h"

const uint32_t I2SFreq[8] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000};
const uint32_t I2SPLLN[8] = {256, 429, 213, 429, 426, 271, 258, 344};
const uint32_t I2SPLLR[8] = {5, 4, 4, 4, 4, 6, 3, 1};

static AUDIO_DrvTypeDef           *pAudioDrv;
I2S_HandleTypeDef                 hAudioOutI2s;

static uint8_t I2S3_Init(uint32_t AudioFreq);
extern AUDIO_DrvTypeDef cs43l22_drv;

uint8_t AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{    
  uint8_t ret = AUDIO_OK;
  
  AUDIO_OUT_ClockConfig(&hAudioOutI2s, AudioFreq, NULL);
  
  hAudioOutI2s.Instance = I2S3;
  if(HAL_I2S_GetState(&hAudioOutI2s) == HAL_I2S_STATE_RESET)
  {
    AUDIO_OUT_MspInit(&hAudioOutI2s, NULL);
  }

  if(I2S3_Init(AudioFreq) != AUDIO_OK)
  {
    ret = AUDIO_ERROR;
  }

  if(ret == AUDIO_OK)
  {
    if(((cs43l22_drv.ReadID(AUDIO_I2C_ADDRESS)) & CS43L22_ID_MASK) == CS43L22_ID)
    {  
      pAudioDrv = &cs43l22_drv; 
    }
    else
    {
      ret = AUDIO_ERROR;
    }
  }
  
  if(ret == AUDIO_OK)
  {
    pAudioDrv->Init(AUDIO_I2C_ADDRESS, OutputDevice, Volume, AudioFreq);
  }
  
  return ret;
}

uint8_t AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size)
{
  if(pAudioDrv->Play(AUDIO_I2C_ADDRESS, pBuffer, Size) != 0)
  {
    return AUDIO_ERROR;
  }
  else 
  {
    HAL_I2S_Transmit_DMA(&hAudioOutI2s, pBuffer, DMA_MAX(Size/AUDIODATA_SIZE)); 
    
    return AUDIO_OK;
  }
}

void AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size)
{
  HAL_I2S_Transmit_DMA(&hAudioOutI2s, pData, Size); 
}

uint8_t AUDIO_OUT_Pause(void)
{    
  if(pAudioDrv->Pause(AUDIO_I2C_ADDRESS) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    HAL_I2S_DMAPause(&hAudioOutI2s);
    
    return AUDIO_OK;
  }
}

uint8_t AUDIO_OUT_Resume(void)
{    
  if(pAudioDrv->Resume(AUDIO_I2C_ADDRESS) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    HAL_I2S_DMAResume(&hAudioOutI2s);
    
    return AUDIO_OK;
  }
}

uint8_t AUDIO_OUT_Stop(uint32_t Option)
{
  HAL_I2S_DMAStop(&hAudioOutI2s);
  
  if(pAudioDrv->Stop(AUDIO_I2C_ADDRESS, Option) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    if(Option == CODEC_PDWN_HW)
    { 
      HAL_Delay(1);
      
      HAL_GPIO_WritePin(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, GPIO_PIN_RESET);
    }
    
    return AUDIO_OK;
  }
}

uint8_t AUDIO_OUT_SetVolume(uint8_t Volume)
{
  if(pAudioDrv->SetVolume(AUDIO_I2C_ADDRESS, Volume) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    return AUDIO_OK;
  }
}

uint8_t AUDIO_OUT_SetMute(uint32_t Cmd)
{ 
  if(pAudioDrv->SetMute(AUDIO_I2C_ADDRESS, Cmd) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    return AUDIO_OK;
  }
}

uint8_t AUDIO_OUT_SetOutputMode(uint8_t Output)
{
  if(pAudioDrv->SetOutputMode(AUDIO_I2C_ADDRESS, Output) != 0)
  {
    return AUDIO_ERROR;
  }
  else
  {
    return AUDIO_OK;
  }
}

void AUDIO_OUT_SetFrequency(uint32_t AudioFreq)
{ 
  RCC_PeriphCLKInitTypeDef rccclkinit;
  
  HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);
  if ((AudioFreq & 0x7) == 0)
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = 192;
    rccclkinit.PLLI2S.PLLI2SR = 6;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
  else
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = 290;
    rccclkinit.PLLI2S.PLLI2SR = 2;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
  
  I2S3_Init(AudioFreq);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == I2S3)
  {
    AUDIO_OUT_TransferComplete_CallBack();
  }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == I2S3)
  {
    AUDIO_OUT_HalfTransfer_CallBack();
  }
}

__weak void AUDIO_OUT_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq, void *Params)
{ 
  RCC_PeriphCLKInitTypeDef rccclkinit;
  uint8_t index = 0, freqindex = 0xFF;
  
  for(index = 0; index < 8; index++)
  {
    if(I2SFreq[index] == AudioFreq)
    {
      freqindex = index;
    }
  }
  HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);
  if ((freqindex & 0x7) == 0)
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = I2SPLLN[freqindex];
    rccclkinit.PLLI2S.PLLI2SR = I2SPLLR[freqindex];
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
  else 
  {
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = 258;
    rccclkinit.PLLI2S.PLLI2SR = 3;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
  }
}

__weak void AUDIO_OUT_MspInit(I2S_HandleTypeDef *hi2s, void *Params)
{
  static DMA_HandleTypeDef hdma_i2sTx;
  GPIO_InitTypeDef  GPIO_InitStruct;

  I2S3_CLK_ENABLE();

  I2S3_SCK_SD_CLK_ENABLE();
  I2S3_WS_CLK_ENABLE();

  GPIO_InitStruct.Pin         = I2S3_SCK_PIN | I2S3_SD_PIN; 
  GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull        = GPIO_NOPULL;
  GPIO_InitStruct.Speed       = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate   = I2S3_SCK_SD_WS_AF;
  HAL_GPIO_Init(I2S3_SCK_SD_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin         = I2S3_WS_PIN ;
  HAL_GPIO_Init(I2S3_WS_GPIO_PORT, &GPIO_InitStruct); 

  I2S3_MCK_CLK_ENABLE();
  GPIO_InitStruct.Pin         = I2S3_MCK_PIN; 
  HAL_GPIO_Init(I2S3_MCK_GPIO_PORT, &GPIO_InitStruct);   

  I2S3_DMAx_CLK_ENABLE(); 
  
  if(hi2s->Instance == I2S3)
  {
    hdma_i2sTx.Init.Channel             = I2S3_DMAx_CHANNEL;  
    hdma_i2sTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_i2sTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_i2sTx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_i2sTx.Init.PeriphDataAlignment = I2S3_DMAx_PERIPH_DATA_SIZE;
    hdma_i2sTx.Init.MemDataAlignment    = I2S3_DMAx_MEM_DATA_SIZE;
    hdma_i2sTx.Init.Mode                = DMA_NORMAL;
    hdma_i2sTx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_i2sTx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;         
    hdma_i2sTx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_i2sTx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_i2sTx.Init.PeriphBurst         = DMA_PBURST_SINGLE; 

    hdma_i2sTx.Instance                 = I2S3_DMAx_STREAM;

    __HAL_LINKDMA(hi2s, hdmatx, hdma_i2sTx);
    
    HAL_DMA_DeInit(&hdma_i2sTx);
    
    HAL_DMA_Init(&hdma_i2sTx);
  }
  
  HAL_NVIC_SetPriority(I2S3_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(I2S3_DMAx_IRQ);  
}

__weak void AUDIO_OUT_MspDeInit(I2S_HandleTypeDef *hi2s, void *Params)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;

  HAL_NVIC_DisableIRQ(I2S3_DMAx_IRQ); 
  
  if(hi2s->Instance == I2S3)
  {
    HAL_DMA_DeInit(hi2s->hdmatx);
  }

  __HAL_I2S_DISABLE(hi2s);

  GPIO_InitStruct.Pin = I2S3_SCK_PIN | I2S3_SD_PIN;
  HAL_GPIO_DeInit(I2S3_SCK_SD_GPIO_PORT, GPIO_InitStruct.Pin);
  
  GPIO_InitStruct.Pin = I2S3_WS_PIN;
  HAL_GPIO_DeInit(I2S3_WS_GPIO_PORT, GPIO_InitStruct.Pin);
  
  GPIO_InitStruct.Pin = I2S3_MCK_PIN;
  HAL_GPIO_DeInit(I2S3_MCK_GPIO_PORT, GPIO_InitStruct.Pin); 

  I2S3_CLK_DISABLE();
}

__weak void AUDIO_OUT_TransferComplete_CallBack(void)
{
}

__weak void AUDIO_OUT_HalfTransfer_CallBack(void)
{
}

__weak void AUDIO_OUT_Error_CallBack(void)
{
}

static uint8_t I2S3_Init(uint32_t AudioFreq)
{
  hAudioOutI2s.Instance         = I2S3;
  
  __HAL_I2S_DISABLE(&hAudioOutI2s);
  
  hAudioOutI2s.Init.AudioFreq   = AudioFreq;
  hAudioOutI2s.Init.ClockSource = I2S_CLOCK_PLL;
  hAudioOutI2s.Init.CPOL        = I2S_CPOL_LOW;
  hAudioOutI2s.Init.DataFormat  = I2S_DATAFORMAT_16B;
  hAudioOutI2s.Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE;
  hAudioOutI2s.Init.Mode        = I2S_MODE_MASTER_TX;
  hAudioOutI2s.Init.Standard    = I2S_STANDARD;

  if(HAL_I2S_Init(&hAudioOutI2s) != HAL_OK)
  {
    return AUDIO_ERROR;
  }
  else
  {
    return AUDIO_OK;
  }
}
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == I2S3)
  {
    AUDIO_OUT_Error_CallBack();
  }
}

