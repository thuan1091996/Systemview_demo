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
#include "cmsis_os.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../app/hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_TEST_N25Q				(0)
#define FLASH_TEST_MX25				(1)
#define MX25_SPI_PORT1                  (0) /* 1 -> SPI1, 0 -> SPI2 */
#define MX25_DEFAULT_IMG_ADDR           (0x7000)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile int adc_voltage[10] = {0};
#if (FLASH_TEST_MX25 == 1)
uint8_t flash_info[20] = {0};
volatile bool g_test_flash = 1;
#include "../app/MX25Series.h"
MX25Series_t flash_test = {0};
uint8_t buff_read[512] = {0};
uint8_t buff_write[10] = {1, 2, 3};
#endif /* End of (FLASH_TEST_MX25 == 1) */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Config_Systemview(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	 /* Number of clock cycles that happened after CPU reset */
	 DWT->CYCCNT = 0;
	/*enable cycle counting feature*/
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;
 
  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
 
  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}

// Config all external flash pins as analog input
void flash_pins_deinit(void)
{
    HAL_GPIO_DeInit(GPIOD, FLASH_RESET_PIN_Pin | FLASH_WP_PIN_Pin);
    #if (MX25_SPI_PORT1 != 0)

    /*Configure GPIO pin : SPI1 NSS */
    HAL_GPIO_DeInit(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin);
    HAL_SPI_MspDeInit(&hspi1);

    #else /* !(MX25_SPI_PORT1 != 0) */

    /*Configure GPIO pin : SPI2 NSS */
    HAL_GPIO_DeInit(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin);
    HAL_SPI_MspDeInit(&hspi2);

    #endif /* End of (MX25_SPI_PORT1 != 0) */

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
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  Config_Systemview();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#if (FLASH_TEST_MX25 != 0)
    #if (MX25_SPI_PORT1 != 0)
    if (MX25Series_status_ok == MX25Series_init(&flash_test, &MX25R6435F_Chip_Def_Low_Power, SPI2_NSS_Pin_NUMBER,
                                                FLASH_RESET_PIN_NUMBER, FLASH_WP_PIN_NUMBER, 0, &hspi1))
    #else /* !(MX25_SPI_PORT1 != 0) */
    if (MX25Series_status_ok == MX25Series_init(&flash_test, &MX25R6435F_Chip_Def_Low_Power, SPI2_NSS_Pin_NUMBER,
                                                FLASH_RESET_PIN_NUMBER, FLASH_WP_PIN_NUMBER, 0, &hspi2))
    #endif /* End of (MX25_SPI_PORT1 != 0) */
    {
        if (MX25Series_status_ok == MX25Series_read_identification(&flash_test, &flash_info[0], &flash_info[1], &flash_info[2]))
        {
            printf("MX25Series_init ok\r\n");
            // Check valid image
            if (!is_image_valid(MX25_DEFAULT_IMG_ADDR))
            {
                printf("Found valid image \r\n");
            }
            else
            {
                printf("Image is invalid\r\n");
            }
        }
        else
            printf("MX25Series_read_manufacture_and_device_id fail\r\n");
    }
    else
        printf("MX25Series_init fail\r\n");
#endif
    flash_pins_deinit();
  while (1) {

	  while(g_test_flash == true)
	  {
#if (FLASH_TEST_N25Q != 0)
#include "../app/n25q128a.h"
		  N25Q_ReadID(flash_info, 20);
#endif (FLASH_TEST_N25Q != 0)

	  }
    /* USER CODE END WHILE */

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
  while (1) {
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
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
