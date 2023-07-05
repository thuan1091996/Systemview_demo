
/*------------------------------------------------------------------------------*/
/*							 Includes and dependencies						    */
/*------------------------------------------------------------------------------*/
#include "hal.h"
#include "cmsis_os.h"
/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor defines			    			    */
/*------------------------------------------------------------------------------*/
#define UART_BUFFER_SIZE	64

/*------------------------------------------------------------------------------*/
/*					 		Typedefs enums & structs					  	    */
/*------------------------------------------------------------------------------*/
typedef struct
{
	uint8_t rx_data;
	uint8_t buffer[UART_BUFFER_SIZE];
	volatile uint16_t write_idx;
	volatile uint16_t read_idx;
} tBuffer;

// How the buffer is used
// write_idx = index of next byte to be read

/*------------------------------------------------------------------------------*/
/*					  		   Variables Declare			    			    */
/*------------------------------------------------------------------------------*/
UART_HandleTypeDef __uart_handle[2];
tBuffer __uart_rx_buffer[2];

/*------------------------------------------------------------------------------*/
/*					  	  Function Private Implement		    			    */
/*------------------------------------------------------------------------------*/
#if TESTING_UART3
#else
int __initUART1()
{
    int ret = SUCCESS;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/* Uart clock Configuration */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    ret |= ( HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) == HAL_OK ) ? SUCCESS : FAILURE;
    __HAL_RCC_USART1_CLK_ENABLE();

    /* Uart Configuration */
    __uart_handle[0].Instance = USART3;
    __uart_handle[0].Init.BaudRate = 115200;
    __uart_handle[0].Init.WordLength = UART_WORDLENGTH_8B;
    __uart_handle[0].Init.StopBits = UART_STOPBITS_1;
    __uart_handle[0].Init.Parity = UART_PARITY_NONE;
    __uart_handle[0].Init.Mode = UART_MODE_TX_RX;
    __uart_handle[0].Init.HwFlowCtl = UART_HWCONTROL_NONE;
    __uart_handle[0].Init.OverSampling = UART_OVERSAMPLING_16;
    __uart_handle[0].Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    __uart_handle[0].Init.ClockPrescaler = UART_PRESCALER_DIV1;
    __uart_handle[0].AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	ret |= ( HAL_UART_Init(&__uart_handle[0]) == HAL_OK ) ? SUCCESS : FAILURE;

    /* GPIO Configuration
        PC4     ------> USART1_TX
        PC5     ------> USART1_RX
    */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Uart interrupt Configuration */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    return ret;
}

int __initUART2()
{
    int ret = SUCCESS;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/* Uart clock Configuration */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    ret |= ( HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) == HAL_OK ) ? SUCCESS : FAILURE;
    __HAL_RCC_USART2_CLK_ENABLE();

    /* Uart Configuration */
    __uart_handle[1].Instance = USART2;
    __uart_handle[1].Init.BaudRate = 115200;
    __uart_handle[1].Init.WordLength = UART_WORDLENGTH_8B;
    __uart_handle[1].Init.StopBits = UART_STOPBITS_1;
    __uart_handle[1].Init.Parity = UART_PARITY_NONE;
    __uart_handle[1].Init.Mode = UART_MODE_TX_RX;
    __uart_handle[1].Init.HwFlowCtl = UART_HWCONTROL_NONE;
    __uart_handle[1].Init.OverSampling = UART_OVERSAMPLING_16;
    __uart_handle[1].Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    __uart_handle[1].Init.ClockPrescaler = UART_PRESCALER_DIV1;
    __uart_handle[1].AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	ret |= ( HAL_UART_Init(&__uart_handle[1]) == HAL_OK ) ? SUCCESS : FAILURE;

    /* GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
    */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Uart interrupt Configuration */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    return ret;
}
#endif /* End of TESTING_UART3 */


//Initialize UART to 115200 8N1. Returns 0 on success, -1 on failure.
int __InitUART()
{
    int ret = SUCCESS;
#if TESTING_UART3
    ret |= ( HAL_UART_Receive_IT(&__uart_handle[0], &__uart_rx_buffer[0].rx_data, 1) == HAL_OK ) ? SUCCESS : FAILURE;
#else

    ret |= __initUART1();
    ret |= __initUART2();

    ret |= ( HAL_UART_Receive_IT(&__uart_handle[0], &__uart_rx_buffer[0].rx_data, 1) == HAL_OK ) ? SUCCESS : FAILURE;
    ret |= ( HAL_UART_Receive_IT(&__uart_handle[1], &__uart_rx_buffer[1].rx_data, 1) == HAL_OK ) ? SUCCESS : FAILURE;
#endif /* End of TESTING_UART3 */
    return ret;
}
/*------------------------------------------------------------------------------*/
/*					  	   AT command handlers					                */
/*------------------------------------------------------------------------------*/


#define AT_DEFAULT_UART_PORT            (1)
#define AT_DEFAULT_TIMEOUT_MS           (3000UL)
#define AT_BUFFER_SIZE                  (1024UL)

#define PORT_DELAY_MS(MS)               vTaskDelay((MS)/portTICK_PERIOD_MS) // delay in ms
#define PORT_GET_SYSTIME_MS()           (uint32_t)(xTaskGetTickCount()*1000 / configTICK_RATE_HZ) // convert ticks to ms

int __nrf_slte__send_command(char* command)
{
    return hal__UARTWrite(AT_DEFAULT_UART_PORT, command, strnlen(command, AT_BUFFER_SIZE));
}

int __nrf_slte__get_resp(char* resp, uint16_t maxlength)
{
    param_check(resp != NULL);
    uint16_t recv_idx = 0;
    uint8_t recv_buf[maxlength]; // Temp buffer to receive data from UART
    memset(recv_buf, 0, sizeof(recv_buf));
	uint64_t max_recv_timeout = PORT_GET_SYSTIME_MS() + AT_DEFAULT_TIMEOUT_MS;
    // Wait until maximum AT_DEFAULT_TIMEOUT_MS to receive "\r\n"
    while (PORT_GET_SYSTIME_MS() < max_recv_timeout)
    {
        uint16_t recv_len = hal__UARTAvailable(AT_DEFAULT_UART_PORT);

        if(recv_idx + recv_len > sizeof(recv_buf) )
            break; // Overflow
            
        if(recv_len > 0)
        {
            hal__UARTRead(AT_DEFAULT_UART_PORT, &recv_buf[recv_idx], recv_len);
            recv_idx += recv_len;
            if(strstr(recv_buf, "\r\n") != NULL)
            {
                memcpy(resp, recv_buf, recv_idx);
                return recv_idx;
            }
        }
        PORT_DELAY_MS(100); // Wait for UART buffer to fill up
    }
    return FAILURE;
}

/*
 *  Implements [AT+CFUN=21] (Enables LTE modem.)
 *  Waits for "OK" response. Returns SUCCESS if ok. Returns FAILURE if error.
 */
int nrf_slte__power_on(void)
{
    int ret = FAILURE;
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT+CFUN=21\r\n") != SUCCESS)
        return FAILURE; // Failed to send command
    if (__nrf_slte__get_resp(resp, sizeof(resp)) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "")

        if(strstr(resp, "OK") != NULL)
            ret = SUCCESS;
    return ret;
} 

/*
 * Implements [AT+CFUN=0] (Disables LTE modem.)
 * Waits for "OK" response. Returns SUCCESS if ok. Returns FAILURE if error.
 */
int nrf_slte__power_off(void)
{
    int ret = FAILURE;
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT+CFUN=0\r\n") != SUCCESS)
        return FAILURE; // Failed to send command
    if (__nrf_slte__get_resp(resp, sizeof(resp)) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "")

        if(strstr(resp, "OK") != NULL)
            ret = SUCCESS;
    return ret;
}

/*
 * Implements [AT+COPS?] 
 * returns 1 if connected, 0 if not connected, -1 if error 
 * //https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/nw_service/cops_read.html
 */
int nrf_slte__connected(void);




int hal__ATWaitReponse(uint8_t uartNum, uint8_t* data, uint32_t timeout)
{
    param_check( (1 <= uartNum) && (uartNum <= 2) );
    while(!timeout)
    {
        int len = hal__UARTAvailable(uartNum);
        hal__UARTRead(uartNum, data, len);
    }
}


/*------------------------------------------------------------------------------*/
/*					  	   Function prototypes Implement					    */
/*------------------------------------------------------------------------------*/
int hal__UARTAvailable(uint8_t uartNum)
{
	param_check( (1 <= uartNum) && (uartNum <= 2) );

	tBuffer *buffer = &__uart_rx_buffer[uartNum - 1];

    return (uint16_t)(UART_BUFFER_SIZE + buffer->write_idx - buffer->read_idx) % UART_BUFFER_SIZE;
}

int hal__UARTWrite_uint8(uint8_t uartNum, uint8_t data)
{
	param_check( (1 <= uartNum) && (uartNum <= 2) );

	int ret = SUCCESS;
	ret = ( HAL_UART_Transmit(&__uart_handle[uartNum - 1], &data, 1, 1000) == HAL_OK ) ? SUCCESS : FAILURE;
    return ret;
}

int hal__UARTWrite(uint8_t uartNum, uint8_t *data, uint16_t len)
{
	param_check( (1 <= uartNum) && (uartNum <= 2) );
	param_check( data );

    int ret = SUCCESS;
    ret = ( HAL_UART_Transmit(&__uart_handle[uartNum - 1], data, len, 1000) == HAL_OK ) ? SUCCESS : FAILURE;
    return ret;
}

int hal__UARTRead_uint8(uint8_t uartNum, uint8_t *data)
{
	param_check( (1 <= uartNum) && (uartNum <= 2) );
	param_check( data );

	int ret = FAILURE;
	tBuffer *buffer = &__uart_rx_buffer[uartNum - 1];

	if (buffer->write_idx != buffer->read_idx)
	{
		*data = buffer->buffer[buffer->read_idx];
		buffer->read_idx = (uint16_t)(buffer->read_idx + 1) % UART_BUFFER_SIZE;
		ret = SUCCESS;
	}

    return ret;
}

int hal__UARTRead(uint8_t uartNum, uint8_t *data, uint16_t len)
{
	param_check( (1 <= uartNum) && (uartNum <= 2) );
	param_check( data );

	int ret = FAILURE;
	tBuffer *buffer = &__uart_rx_buffer[uartNum - 1];

	if (buffer->write_idx != buffer->read_idx)
	{
    ret = (uint16_t)(UART_BUFFER_SIZE + buffer->write_idx - buffer->read_idx) % UART_BUFFER_SIZE;	//Length can read
		ret = (ret <= len) ? ret : len;

		for (int i = 0; i < ret; i++)
		{
			*(data + i) = buffer->buffer[buffer->read_idx];
			buffer->read_idx = (uint16_t)(buffer->read_idx + 1) % UART_BUFFER_SIZE;
		}
	}

    return ret;
}

/*------------------------------------------------------------------------------*/
/*					  STM32G0xx Peripheral Interrupt Handlers  				    */
/*------------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&__uart_handle[0]);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&__uart_handle[1]);
}

void __hal_store_rx(tBuffer *buffer)
{
	uint16_t index = (uint16_t)(buffer->write_idx + 1) % UART_BUFFER_SIZE;

	if (index != buffer->read_idx)
	{
		buffer->buffer[buffer->write_idx] = buffer->rx_data;
		buffer->write_idx = index;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == __uart_handle[0].Instance)
    {
    	__hal_store_rx(&__uart_rx_buffer[0]);
        HAL_UART_Receive_IT(huart, &__uart_rx_buffer[0].rx_data, 1);
    }
    else if (huart->Instance == __uart_handle[1].Instance)
    {
    	__hal_store_rx(&__uart_rx_buffer[1]);
        HAL_UART_Receive_IT(huart, &__uart_rx_buffer[1].rx_data, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == __uart_handle[0].Instance)
    {
        HAL_UART_Receive_IT(huart, &__uart_rx_buffer[0].rx_data, 1);
    }
    else if (huart->Instance == __uart_handle[1].Instance)
    {
        HAL_UART_Receive_IT(huart, &__uart_rx_buffer[1].rx_data, 1);
    }
}

