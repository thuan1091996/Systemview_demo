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
#include "usb_otg.h"
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	volatile int adc_voltage[10] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
int example_divide_by_zero(int denominator) {
  int rv = 5;
  if (denominator == 0) {
    rv = 1 / denominator;
  }
  return rv;
}

__attribute__ ((naked))  void HardFault_Handler(void) {
    // get current Stack Pointer
    __asm volatile("MRS R0, MSP");
    __asm volatile("B HardFault_Handler_main");
}

void DumpExceptionRegister(uint32_t* pMSP)
{
  printf(" MSP = %p\n", pMSP);
  printf("  R0 = 0x%lx\n", pMSP[0]);  // May have argument of function
  printf("  R1 = 0x%lx\n", pMSP[1]);  // May have argument of function
  printf("  R2 = 0x%lx\n", pMSP[2]);  // May have argument of function
  printf("  R3 = 0x%lx\n", pMSP[3]);  // May have argument of function
  printf(" R12 = 0x%lx\n", pMSP[4]);  // IP holds an intermediate value of a calculation
  printf("  LR = 0x%lx\n", pMSP[5]);  // Address of the next instruction before the exception
  printf("  PC = 0x%lx\n", pMSP[6]);  // CPU was executing the instruction at PC
  printf("xPSR = 0x%lx\n", pMSP[7]);  // Status of system before execution at PC completes
}

void HardFault_Handler_main(uint32_t* pMSP) {
  printf("Exception: Usage Fault\n");
  DumpExceptionRegister(pMSP);

  uint32_t* pUFSR = (uint32_t*)0xE000ED2A;
  printf("UFSR = 0x%lx\n", *pUFSR & 0xFFFF);

  while(1);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void test_me(void)
{
//	uint8_t* p_test_u8 = (uint8_t*)&temp_buff[100];
//	for(uint8_t idx=0; idx<10; idx++)
//		(uint8_t*)(p_test_u8[idx]) = idx;
}
uint32_t guess_me;
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
  MX_USB_OTG_FS_HCD_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  __initADC1();

  /* USER CODE END 2 */

  /* Init scheduler */
//  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
//  MX_FREERTOS_Init();

  /* Start scheduler */
//  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    adc_voltage[0] = hal__ADCReadMV(0);
    adc_voltage[1] = hal__ADCReadMV(1);
    adc_voltage[2] = hal__ADCReadMV(2);
    adc_voltage[3] = hal__ADCReadMV(3);
    adc_voltage[4] = hal__ADCReadMV(4);
    adc_voltage[5] = hal__ADCReadMV(5);
    adc_voltage[6] = hal__ADCReadMV(6);
    adc_voltage[7] = hal__ADCReadMV(7);
    adc_voltage[8] = hal__ADCReadMV(8);
    adc_voltage[9] = hal__ADCReadMV(9);
    memset(adc_voltage, 0, sizeof(adc_voltage));
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
