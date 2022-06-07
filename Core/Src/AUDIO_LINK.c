#include "AUDIO_LINK.h"

uint32_t I2cxTimeout = I2Cx_TIMEOUT_MAX;

static I2C_HandleTypeDef I2cHandle;

static void    I2Cx_Init(void);
static void    I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value);
static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg);
static void    I2Cx_Error (void);
static void    I2Cx_MspInit(I2C_HandleTypeDef *hi2c);

void    AUDIO_IO_Init(void);
void    AUDIO_IO_DeInit(void);
void    AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);

static void I2Cx_Init(void)
{
  if(HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET)
  {
    I2cHandle.Instance = AUDIO_I2Cx;
    I2cHandle.Init.OwnAddress1 =  0x43;
    I2cHandle.Init.ClockSpeed = I2Cx_MAX_COMMUNICATION_FREQ;
    I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    I2cHandle.Init.OwnAddress2 = 0x00;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;

    I2Cx_MspInit(&I2cHandle);
    HAL_I2C_Init(&I2cHandle);
  }
}

static void I2Cx_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  
  if(status != HAL_OK)
  {
    I2Cx_Error();
  }
}

static uint8_t I2Cx_ReadData(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
  
  if(status != HAL_OK)
  {
    I2Cx_Error();
  }
  return value;
}

static void I2Cx_Error(void)
{
  HAL_I2C_DeInit(&I2cHandle);
  
  I2Cx_Init();
}

static void I2Cx_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  AUDIO_I2Cx_CLOCK_ENABLE();

  AUDIO_I2Cx_GPIO_CLK_ENABLE();

  GPIO_InitStructure.Pin = AUDIO_I2Cx_SDA_PIN | AUDIO_I2Cx_SCL_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate = AUDIO_I2Cx_AF;

  HAL_GPIO_Init(AUDIO_I2Cx_GPIO_PORT, &GPIO_InitStructure);

  AUDIO_I2Cx_FORCE_RESET();

  AUDIO_I2Cx_RELEASE_RESET();

  HAL_NVIC_SetPriority(AUDIO_I2Cx_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(AUDIO_I2Cx_EV_IRQn);

  HAL_NVIC_SetPriority(AUDIO_I2Cx_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(AUDIO_I2Cx_ER_IRQn);
}

void AUDIO_IO_Init(void) 
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  AUDIO_RESET_GPIO_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = AUDIO_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStruct);
  
  I2Cx_Init();
  
  CODEC_AUDIO_POWER_OFF();
  
  HAL_Delay(5); 
  
  CODEC_AUDIO_POWER_ON();
  
  HAL_Delay(5); 
}

void AUDIO_IO_DeInit(void) 
{
  
}

void AUDIO_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_WriteData(Addr, Reg, Value);
}

uint8_t AUDIO_IO_Read (uint8_t Addr, uint8_t Reg)
{
  return I2Cx_ReadData(Addr, Reg);
}
