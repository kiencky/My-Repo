/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CLCD_I2C.h"
#include "rc522.h"
#include "stdio.h"
#include "string.h"
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
CLCD_I2C_Name LCD1;
uint8_t serial[6];
uint8_t Readcard[5];
char UID[9];
char user[16];
char str[32]="";
uint8_t buffer_Rx[1];
uint8_t Cua = 0;
uint8_t state[6];
uint8_t occupied=0;
uint8_t a=0;
uint8_t b=0;
uint8_t c=0;
uint8_t d=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void Servo()
{
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,15);
	HAL_Delay(3000);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,25);
}

void LCD_Welcome()
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_SetCursor(&LCD1,4,0);
	CLCD_I2C_WriteString(&LCD1,"Welcome");
	CLCD_I2C_SetCursor(&LCD1,0,1);
	CLCD_I2C_WriteString(&LCD1,"Moi ban quet the");
	
}

void LCD_False()
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_SetCursor(&LCD1,4,0);
	CLCD_I2C_WriteString(&LCD1,"The sai");
	CLCD_I2C_SetCursor(&LCD1,0,1);
	CLCD_I2C_WriteString(&LCD1,"Moi ban quet lai");
	for(uint8_t i=0;i<3;i++)
	{
		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
		HAL_Delay(300);
		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
		HAL_Delay(300);
	}
}

void Bao_Chay()
{
	if(HAL_GPIO_ReadPin(Fire_GPIO_Port,Fire_Pin) == 0){
		CLCD_I2C_SetCursor(&LCD1,3,0);
		CLCD_I2C_WriteString(&LCD1,"Warning!!!");
		CLCD_I2C_SetCursor(&LCD1,0,1);
		CLCD_I2C_WriteString(&LCD1,"Canh bao co chay");
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,15);
		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
	}
	else {
//		LCD_Welcome();
		CLCD_I2C_SetCursor(&LCD1,3,0);
		CLCD_I2C_WriteString(&LCD1," Welcome  ");
		CLCD_I2C_SetCursor(&LCD1,0,1);
		CLCD_I2C_WriteString(&LCD1,"Moi ban quet the");
		if(Cua==0){
			HAL_Delay(50);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,5);
		}
		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
	}
}

void Full_slot()
{
	if((HAL_GPIO_ReadPin(S1_GPIO_Port,S1_Pin) == 0) && (HAL_GPIO_ReadPin(S2_GPIO_Port,S2_Pin) == 0) &&
		(HAL_GPIO_ReadPin(S3_GPIO_Port,S3_Pin) == 0) && (HAL_GPIO_ReadPin(S4_GPIO_Port,S4_Pin) == 0))
	{
		CLCD_I2C_Clear(&LCD1);
		CLCD_I2C_SetCursor(&LCD1,3,0);
		CLCD_I2C_WriteString(&LCD1,"Warning!!!");
		CLCD_I2C_SetCursor(&LCD1,0,1);
		CLCD_I2C_WriteString(&LCD1,"Bai do xe da day");
	}
}

void State_baidoxe()
{
	state[0]='@';
	if(HAL_GPIO_ReadPin(S1_GPIO_Port,S1_Pin) == 0)
	{
		state[1] = '1';
	}
	else state[1] = '0';
	if(HAL_GPIO_ReadPin(S2_GPIO_Port,S2_Pin) == 0)
	{
		state[2] = '1';
	}
	else state[2] = '0';
	if(HAL_GPIO_ReadPin(S3_GPIO_Port,S3_Pin) == 0)
	{
		state[3] = '1';
	}
	else state[3] = '0';
	if(HAL_GPIO_ReadPin(S4_GPIO_Port,S4_Pin) == 0)
	{
		state[4] = '1';
	}
	else state[4] = '0';
	state[5] = '\0';
	occupied=0;
	for(int i=1; i < sizeof(state)-1; i++)
	{
		if(state[i]=='1')
			occupied++;
	}	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1 )
	{
		if(buffer_Rx[0] == 'Y')
		{
			a = 1;
			HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
		}
		if(buffer_Rx[0] == 'O')
		{
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,15);
		}
		if(buffer_Rx[0] == 'C')
		{
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,25);
		}
		if(buffer_Rx[0] == 'R')
		{
			HAL_UART_Transmit(&huart1,(uint8_t *)state,sizeof(state),100);
		}
		HAL_UART_Receive_IT(&huart1,buffer_Rx,sizeof(buffer_Rx));
	}
}

void User_data()
{
	if (occupied == 4)
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)state,sizeof(state),100);
//		CLCD_I2C_Clear(&LCD1);
//		CLCD_I2C_SetCursor(&LCD1,5,0);
//		CLCD_I2C_WriteString(&LCD1,"Sorry");
//		CLCD_I2C_SetCursor(&LCD1,0,1);
//		CLCD_I2C_WriteString(&LCD1,"Bai do xe da day");
//		HAL_Delay(3000);
//		LCD_Welcome();
	}
	else
	{
		HAL_UART_Transmit(&huart1,(uint8_t *)state,sizeof(state),100);
		HAL_Delay(500);
		HAL_UART_Transmit(&huart1,(uint8_t *)UID,sizeof(UID),100);
	}
}

void array_to_string(uint8_t array[],uint8_t len,char buffer[])
{
	for(uint8_t i=0;i<4;i++)
	{
		uint8_t nib1 = (array[i]>>4) & 0x0F;
		uint8_t nib2 = (array[i]>>0) & 0x0F;
		buffer[i*2+0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
		buffer[i*2+1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
	}
	buffer[len*2] = '\0';

}
void Check_ID()
{
	if(!TM_MFRC522_Request(PICC_REQIDL,serial)){
			if(!TM_MFRC522_Anticoll(serial)){					
				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
				HAL_Delay(300);
				HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_RESET);
				HAL_Delay(100);
				for (uint8_t i=1;i<5;i++){
					Readcard[i-1]=serial[i];
				}
				array_to_string(Readcard,4,str);
				strcpy(UID,str);
				User_data();
				TM_MFRC522_Halt();	
			}
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
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	TM_MFRC522_Init();
	CLCD_I2C_Init(&LCD1,&hi2c1,0x4e,20,4);
	LCD_Welcome();
	HAL_UART_Receive_IT(&huart1,buffer_Rx,sizeof(buffer_Rx));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		State_baidoxe();
		Check_ID();
		if (a == 1)
		{
			Servo();
			a=0;
		}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 199;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, LED_Pin|SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin SS_Pin */
  GPIO_InitStruct.Pin = LED_Pin|SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Button_Pin Fire_Pin S4_Pin */
  GPIO_InitStruct.Pin = Button_Pin|Fire_Pin|S4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : S1_Pin S2_Pin S3_Pin */
  GPIO_InitStruct.Pin = S1_Pin|S2_Pin|S3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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
