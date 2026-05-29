/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MASTER_ADDR (0x52 << 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool proto = false;
bool mode = true;

uint8_t data_tx = 0xCD;
uint8_t data_rx = 0;

char buf[50];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0)
    {
    	proto = !proto;
    }

    if(GPIO_Pin == GPIO_PIN_1)
    {
    	mode = !mode;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_I2C2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();

  HAL_Delay(500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if (proto)
	  {
			ssd1306_Fill(Black);

			ssd1306_SetCursor(0, 0);
			snprintf(buf, sizeof(buf), "I2C Slave");
			ssd1306_WriteString(buf, Font_7x10, White);

			if (mode)
			{
				ssd1306_SetCursor(0, 11);
				snprintf(buf, sizeof(buf), "Receive");
				ssd1306_WriteString(buf, Font_7x10, White);

				HAL_StatusTypeDef ret = HAL_I2C_Slave_Receive(&hi2c2, &data_rx, 1, 200);

				ssd1306_SetCursor(0, 22);
				if (ret == HAL_OK)
				{
				    snprintf(buf, sizeof(buf), "Data: 0x%02X", data_rx);
				}
				else
				{
				    snprintf(buf, sizeof(buf), "Data: None");
				}
				ssd1306_WriteString(buf, Font_7x10, White);
			}
			else
			{
		        ssd1306_SetCursor(0, 11);
		        snprintf(buf, sizeof(buf), "Transmit");
		        ssd1306_WriteString(buf, Font_7x10, White);

		        HAL_I2C_Slave_Transmit(&hi2c2, &data_tx, 1, 200);
			}
			ssd1306_UpdateScreen();
	  }
	  else
	  {
		  ssd1306_Fill(Black);

		  ssd1306_SetCursor(0, 0);
		  snprintf(buf, sizeof(buf), "SPI Slave");
		  ssd1306_WriteString(buf, Font_7x10, White);

		  ssd1306_SetCursor(0, 11);
		  snprintf(buf, sizeof(buf), "TX:0x%02X", data_tx);
		  ssd1306_WriteString(buf, Font_7x10, White);

		  HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, &data_tx, &data_rx, 1, 1000);

		  ssd1306_SetCursor(0, 22);
		  if (ret == HAL_OK)
		      snprintf(buf, sizeof(buf), "RX:0x%02X", data_rx);
		  else
		      snprintf(buf, sizeof(buf), "Waiting...");
		  ssd1306_WriteString(buf, Font_7x10, White);;
		  ssd1306_UpdateScreen();

		  HAL_Delay(500);
	  }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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
