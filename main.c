/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "HexToFlash.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


#define VECT_TAB_OFFSET 0x8000;
uint32_t bootFlash = 0x08008000;
uint32_t BootSettingsAdress;
uint32_t resetHandlerValue;
uint32_t mspValue;



uint32_t okudata;
uint32_t FlashAdress;
FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t PageError;

uint8_t RxBuffer[RxBufferSize];

int satir=0;
int update = 0;
uint16_t checksum_kontrol;
uint8_t checksum_kontrol2;

uint16_t flashayazData[16*1024];  // 32kb lik alan 16*(8/8)*1024 = 2048 byte
uint16_t flashyazindex;

char CC;
int doru;
int yanlis;
int flashdatauzunlugu;
int page;
char page_str[10];
int boot;
int sonpage;

char BinAdress[3];
uint16_t BinData[17];
uint16_t BinData2[17];
int kalanindex;

int userAPP=0;

int sector;

struct HexBuff hexbuff;
struct HexInt hexint;
struct BootSetValues bootsetvalues;

int oku,yaz;


uint8_t BootSetValue;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void flash_jump_to_app()
{
    // Debug amaçli: Uygulamaya atlarken UART üzerinden mesaj gönder
    HAL_UART_Transmit(&huart3, (uint8_t *)"Atlama Tamam        \n\r", 21, 1000);

    // Uygulama reset handler fonksiyon isaretçisi tanimla
    void (*bootloader_application_reset_handler)(void);

    // MSP (Main Stack Pointer) degerini ayarla
    mspValue = *(volatile uint32_t*)bootFlash;
    __set_MSP(mspValue);

    // Vektör tablosu baslangiç adresini ayarla
    SCB->VTOR = bootFlash;

    // Reset Handler adresini al ve fonksiyon isaretçisine ata
    resetHandlerValue = *(volatile uint32_t*)(bootFlash + 4);
    bootloader_application_reset_handler = (void(*)(void))resetHandlerValue;

    // Çevresel bilesenleri kapat ve SysTick'i sifirla
    HAL_RCC_DeInit();  // Tüm saat yapilandirmalarini varsayilana döndür
    HAL_DeInit();      // Tüm periferalleri devre disi birak
    //SystemClock_Config();

    // Kesmeleri kapat
		__disable_irq();

    // Uygulamaya atla
    bootloader_application_reset_handler();

    while (1)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,1);  // LED'i yanip söndürerek hatayi göster
        HAL_Delay(500); // 500ms gecikme
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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
 HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RxBuffer, RxBufferSize);
  __HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT);
	
	HexToFlashInit();
	bootGirisEkrani();
	
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
  {
      HAL_UART_Transmit(&huart3, (uint8_t *)"\n\rBOOTLOADER MODUNA GIRILDI  \n\r", 32, 1000);
      HAL_UART_Transmit(&huart3, (uint8_t *)"DATA BEKLENIYOR                \n\r", 36, 1000);
      boot = 1;
  }
	else
	flash_jump_to_app();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HexToFlash();
		BootMenuSettings();
		if(1 == boot )
		{
		BootSeettingsEraseAndWrite();
		}
	  else if(1 == userAPP)
		  userAPP = 1;
	  else
	  {
		  /*HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			HAL_Delay(200);*/
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART3)
	{
		checksum_kontrol=0;
		checksum_kontrol2=0;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RxBuffer, RxBufferSize);
		  __HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT);
		  satir++;
		
		if(RxBuffer[0] == 'q')
			NVIC_SystemReset();
		
		if ((RxBuffer[0] >= '0' && RxBuffer[0] <= '9') ||
    (RxBuffer[0] >= 'A' && RxBuffer[0] <= 'B'))
		{
						if(RxBuffer[0] == '0')
							BootSetValue = BL_GET_VER;
						if(RxBuffer[0] == '1')
							BootSetValue = BL_GET_HELP;
						if(RxBuffer[0] == '2')
							BootSetValue = BL_GET_CID;
						if(RxBuffer[0] == '3')
							BootSetValue = BL_GET_RDP_STATUS;
						if(RxBuffer[0] == '4')
							BootSetValue = BL_GO_TO_ADDR;
						if(RxBuffer[0] == '5')
							BootSetValue = BL_FLASH_ERASE;
						if(RxBuffer[0] == '6')
							BootSetValue = BL_MEM_WRITE;
						if(RxBuffer[0] == '7')
							BootSetValue = BL_ENDIS_RW_PROTECT;
						if(RxBuffer[0] == '8')
							BootSetValue = BL_MEM_READ;
						if(RxBuffer[0] == '9')
							BootSetValue = BL_READ_SECTOR_STATUS;
						if(RxBuffer[0] == 'A')
							BootSetValue = BL_OTP_READ;
						if(RxBuffer[0] == 'B')
							BootSetValue = BL_DIS_R_W_PROTECT;
						satir--;
		}
		else
		{

		  strncpy(hexbuff.Record_Length,RxBuffer + 1,RECORD_LENGTH);
		  hexint.recordLength = strtol(hexbuff.Record_Length,NULL,16);

		  checksum_kontrol += hexint.recordLength;

		  strncpy(hexbuff.Adress,RxBuffer + 3,ADRESS_LENGTH);
		  hexint.adress = strtol(hexbuff.Adress,NULL,16);


		  strncpy(hexbuff.Record_Type,RxBuffer + 7,RECORD_TYPE_LENGTH);
		  hexint.recordType = strtol(hexbuff.Record_Type,NULL,16);

		  checksum_kontrol += hexint.recordType;

		  unsigned char gecici[3];
		  for(int i = 0;i <4;i+=2)
		  {
		  	/* strncpy(gecici ,RxBuffer + i + 3,2);
		  	checksum_kontrol +=  strtol(gecici, NULL, 16);
		  	 BinAdress[i / 2] = (uint8_t)strtol(gecici, NULL, 16);*/

		  	strncpy(gecici, RxBuffer+ i + 3, sizeof(gecici) - 1);  // `gecici` dizisinin boyutunu asmamak için
		  	gecici[sizeof(gecici) - 1] = '\0';
		  	 BinAdress[i / 2] =strtol(gecici, NULL, 16);
		  	checksum_kontrol +=  BinAdress[i / 2];
		  }


		  strncpy(hexbuff.Data,RxBuffer + 9,hexint.recordLength * 2);
		  hexint.data = strtol(hexbuff.Data,NULL,16);

		  //char gecici[100];

		  for(int i=0;i<(hexint.recordLength*2);i+=2)
		  {
		  		/*strncpy(gecici ,RxBuffer + 9 + i,2);
		  		BinData2[i / 2] = (uint8_t)strtol(gecici, NULL, 16);
		  		checksum_kontrol += BinData2[i / 2];*/
			  strncpy(gecici, RxBuffer + 9 + i, sizeof(gecici) - 1);  // `gecici` dizisinin boyutunu asmamak için
			  gecici[sizeof(gecici) - 1] = '\0';
			  BinData2[i / 2] =strtol(gecici, NULL, 16);
			  checksum_kontrol +=  BinData2[i / 2];

		  }

		  if(hexint.recordType == 0)
		  {
		  for(int i = 0;i <(hexint.recordLength * 2);i+=4)
		  {
		  	  strncpy(gecici ,RxBuffer + 9 + i,4);
		  	flashayazData[flashyazindex] = strtoll(gecici, NULL, 16);
		  	BinData[i/4] = strtoll(gecici, NULL, 16);
		  	  flashyazindex++;
		 }
		  int remaining = 8 - (hexint.recordLength + 1) / 2;
		  for(int i =0; i < remaining ;i++)
		  {
		  		flashayazData[flashyazindex] = 0xFFFF;
		  		flashyazindex++;
		  }
		  }

		  int z = 8 + (hexint.recordLength * 2) + 2;

		  strncpy(hexbuff.checkSum,RxBuffer + z-1 ,2);
		  hexint.checksum = strtol(hexbuff.checkSum,NULL,16);
		  checksum_kontrol = checksum_kontrol & 0xFF;
		  checksum_kontrol2 = (checksum_kontrol ^ 0xFF)+1;

		  if(hexint.checksum == checksum_kontrol2 )
		  {
		  		CC = '1';
		  		doru++;
		  }
		  else
		  {
		  		CC = '0';
		  		yanlis++;
		  }
		  HAL_UART_Transmit(&huart3, (uint8_t *)&CC, sizeof(CC), 1000);
	}}
}


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
