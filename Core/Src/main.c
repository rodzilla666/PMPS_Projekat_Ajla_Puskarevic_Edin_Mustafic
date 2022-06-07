/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "usb_host.h"
#include "gpio.h"
#include "MY_LIS3DSH.h"
#include "File_Handling.h"
#include "waveplayer.h"
#include "AUDIO_LINK.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


LIS3DSH_DataRaw myData;
int tiltedLeft, tiltedRight, tiltedForward, tiltedBack, isFinished;

uint32_t lastVolume = 0;

extern ApplicationTypeDef Appli_state;
extern AUDIO_PLAYBACK_StateTypeDef AudioState;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	if(GPIO_PIN == GPIO_PIN_0)
	{
		if(AudioState == AUDIO_STATE_PLAY)
		{
			lastVolume = GetCurrentVolume();
			AudioState = AUDIO_STATE_PAUSE;
			AUDIO_OUT_SetVolume(0);
		}
		else if(AudioState == AUDIO_STATE_PAUSE)
		{
			AudioState = AUDIO_STATE_PLAY;
			AUDIO_OUT_SetVolume(lastVolume);
		}
	}
}

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  LIS3DSH_InitTypeDef myAccConfigDef;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_DMA_Init();
  MX_USB_HOST_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  MX_DriverVbusFS(0);

  myAccConfigDef.dataRate = LIS3DSH_DATARATE_12_5;
   myAccConfigDef.fullScale = LIS3DSH_FULLSCALE_4;
   myAccConfigDef.antiAliasingBW = LIS3DSH_FILTER_BW_50;
   myAccConfigDef.enableAxes = LIS3DSH_XYZ_ENABLE;
   myAccConfigDef.interruptEnable = false;

   LIS3DSH_Init(&hspi1, &myAccConfigDef);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1)
     {
       MX_USB_HOST_Process();

       if(Appli_state == APPLICATION_READY)
       {
       	Mount_USB();
       	AUDIO_PLAYER_Start(0);
       	while(!isFinished)
       	{

       		if(AudioState != AUDIO_STATE_PAUSE)
       		{
       			AUDIO_PLAYER_Process(TRUE);
       		}
       		HandleTilt();
       		if(AudioState == AUDIO_STATE_STOP)
       		{
       			isFinished = 1;
       		}
       	}

       }
     }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */

void HandleTilt()
{
	myData = LIS3DSH_GetDataRaw();
	if(tiltedForward == 0 && tiltedBack == 0)
	{
		if(myData.x > 5000 && tiltedRight == 0)
		{
			// TITED RIGHT
			tiltedRight = 1;
			AudioState = AUDIO_STATE_NEXT;
			if(tiltedLeft == 1)
			{
				HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
				tiltedLeft = 0;
			}
			HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
		}
		else if(myData.x < -5000 && tiltedLeft == 0)
		{
			// TILTED LEFT
			tiltedLeft = 1;
			AudioState = AUDIO_STATE_PREVIOUS;
			if(tiltedRight == 1)
			{
				HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
				tiltedRight = 0;
			}
			HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		}
		else if(myData.x > -500 && myData.x < 500)
		{
			// NOT TILTED ON X
			if(tiltedRight == 1)
			{
				HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
			}
			if(tiltedLeft == 1)
			{
				HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
			}
			tiltedRight = 0;
			tiltedLeft = 0;
		}
	}
	if(tiltedRight == 0 && tiltedLeft == 0)
	{
		if(myData.y > 4500 && tiltedForward == 0)
		{
			// TILTED FORWARD
			tiltedForward = 1;
			AudioState = AUDIO_STATE_VOLUME_UP;
			if(tiltedBack == 1)
			{
				HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
				tiltedBack = 0;
			}
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		}
		else if(myData.y < -4500 && tiltedBack == 0)
		{
			// TILTED BACK
			tiltedBack = 1;
			AudioState = AUDIO_STATE_VOLUME_DOWN;
			if(tiltedForward == 1)
			{
				HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
				tiltedForward = 0;
			}
			HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
		}
		else if(myData.y > -500 && myData.y < 500)
		{
			// NOT TILTED ON Y
			if(tiltedForward == 1)
			{
				HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			}
			if(tiltedBack == 1)
			{
				HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
			}
			tiltedForward = 0;
			tiltedBack = 0;
		}
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
