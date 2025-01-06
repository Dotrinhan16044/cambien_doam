#include "main.h"
#include "lcd_i2c.h"
//#include "ds1307_for_stm32_hal.h"
#include <stdio.h>

// Khai b�o c�c handle cho I2C v� ADC
I2C_HandleTypeDef hi2c2;
ADC_HandleTypeDef hadc1;
RTC_HandleTypeDef hrtc;
#define MAX_SOIL_MOISTURE 60
// Kh?i t?o c?u tr�c cho LCD
LCD_I2C_HandleTypeDef lcd;

uint16_t soilMoisture = 0;  // Bi?n luu gi� tr? d? ?m d?t
uint8_t pumpState = 0;      // Tr?ng th�i bom nu?c
uint8_t systemActive = 0;   // Tr?ng th�i ho?t d?ng h? th?ng

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_RTC_Init(void);
void Display_Time_And_Soil_LCD(/*uint8_t hour, uint8_t minute, uint8_t second,*/ uint8_t soilMoisturePercentage);
uint16_t Read_Soil_Moisture(void);
void Handle_Buttons(void);
void batcoi(void);
void hienthi(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_I2C2_Init();
    MX_RTC_Init();
    // Kh?i t?o LCD
    lcd_i2c_init(&lcd, &hi2c2, 16, 2, 0x4E);

    // Kh?i t?o DS1307
   // DS1307_Init(&hi2c1);

    while (1)
    {
        Handle_Buttons();
  
        if (!systemActive) {
            HAL_Delay(500);
            continue;
       }
				hienthi();

      /*  uint8_t hour = DS1307_GetHour();
        uint8_t minute = DS1307_GetMinute();
        uint8_t second = DS1307_GetSecond(); */

        
//        // �i?u khi?n bom d?a tr�n d? ?m
      
//   batcoi();
 
    }
}
void batcoi(void)
	{
		soilMoisture = Read_Soil_Moisture();
        uint8_t soilMoisturePercentage = (100 - ((soilMoisture *100)/ 4095) );
	 if (soilMoisturePercentage < MAX_SOIL_MOISTURE) 
		 {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1); // B?t bom nu?c
			 } else {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0); // Tat bom nu?c
			 }
	
	
	
	}
	void hienthi(void)
		{
			soilMoisture = Read_Soil_Moisture();
        uint8_t soilMoisturePercentage = (100 - (soilMoisture*100) / 4095);
   
        Display_Time_And_Soil_LCD(/*hour, minute, second,*/ soilMoisturePercentage);
       HAL_Delay(500);  // Delay m?i gi�y
		 if (soilMoisturePercentage < MAX_SOIL_MOISTURE) {
         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); // B?t bom nu?c
				 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); 
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0); // Bat coi
				  lcd_set_cursor(&lcd, 0, 0);
          lcd_send_string(&lcd,"hd");
				   HAL_Delay(500);  // Delay m?i gi�y
				  
				
        } else {
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); // T?t bom nu?c
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); 
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1); // Tat coi
					 lcd_set_cursor(&lcd, 0, 0);
           lcd_send_string(&lcd, "khd");
					  HAL_Delay(500);  // Delay m?i gi�y
				
//				
        }
		
		}
void Display_Time_And_Soil_LCD(/*uint8_t hour, uint8_t minute, uint8_t second,*/ uint8_t soilMoisturePercentage)
{
   // char timeString[16];
    char soilString[16];
  //  sprintf(timeString, "Time: %02d:%02d:%02d", hour, minute, second);
    sprintf(soilString, "Soil: %d%%", soilMoisturePercentage);

    lcd_clear(&lcd);
   // lcd_set_cursor(&lcd, 0, 0);
   // lcd_send_string(&lcd, timeString);
    lcd_set_cursor(&lcd, 0, 1);
    lcd_send_string(&lcd, soilString);
}

uint16_t Read_Soil_Moisture(void)
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}

void Handle_Buttons(void) {
    int startButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);   // �?c tr?ng th�i c?a n�t START
    int resetButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);   // �?c tr?ng th�i c?a n�t RESET
    int manualButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);  // �?c tr?ng th�i c?a n�t MANUAL

    // N?u n�t START du?c nh?n
    if (startButton == 0) {
        systemActive = 1;  // K�ch ho?t h? th?ng
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);  // B?t d�n LED xanh d? b�o h? th?ng dang ho?t d?ng
        lcd_clear(&lcd);
        lcd_set_cursor(&lcd, 0, 0);
        lcd_send_string(&lcd, "Started");
        HAL_Delay(1000);  
			  lcd_clear(&lcd);// Hi?n th? th�ng b�o n�y trong 1 gi�y
    }

    // N?u n�t RESET du?c nh?n
      else if (resetButton == 0) {
        
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  // T?t d�n LED xanh
       // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);  // T?t bom nu?c
        lcd_clear(&lcd);
        lcd_set_cursor(&lcd, 0, 0);
        lcd_send_string(&lcd, " tat may bom");
        HAL_Delay(1000);  // Hi?n th? th�ng b�o n�y trong 1 gi�y
   }

    // N?u n�t MANUAL du?c nh?n
    else if (manualButton == 0) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 ,GPIO_PIN_RESET);  // K�ch ho?t bom nu?c ngay l?p t?c
        lcd_clear(&lcd);
        lcd_set_cursor(&lcd, 0, 0);
        lcd_send_string(&lcd, "bat may bom");
        HAL_Delay(1000);  // Hi?n th? th�ng b�o n�y trong 1 gi�y
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV128;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)== 0x2608)
	{
  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  } 
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2608);
	}
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
