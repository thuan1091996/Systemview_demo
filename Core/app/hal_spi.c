/*------------------------------------------------------------------------------*/
/*							 Includes and dependencies						    */
/*------------------------------------------------------------------------------*/
#include "hal.h"

/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor defines			    			    */
/*------------------------------------------------------------------------------*/
#define SPI_DEFAULT_TIMEOUT     1000
/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor MACRO			    			    */
/*------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/*					  		   Typedefs			    			                */
/*------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/*					  		   Variables Declare			    			    */
/*------------------------------------------------------------------------------*/

SPI_HandleTypeDef __spi_handle[2];
/*------------------------------------------------------------------------------*/
/*					  	  Function Private Implement		    			    */
/*------------------------------------------------------------------------------*/
static int __initSPI1(void)
{
    if (HAL_SPI_GetState(&__spi_handle[0]) == HAL_SPI_STATE_RESET)
    {
        // SPI1 GPIO Configuration

        /*  GPIO pin configs 
            PB3 -------> SPI1_SCK
            PB4 -------> SPI1_MISO
            PB5 -------> SPI1_MOSI
            PA10 ------> SPI1_NSS
        */

        /* Enable GPIO clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef  gpioinitstruct = {0};

        /* Configure SPI SCK */
        gpioinitstruct.Pin = GPIO_PIN_3;
        gpioinitstruct.Mode = GPIO_MODE_AF_PP;
        gpioinitstruct.Pull  = GPIO_PULLUP;
        gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#if defined(STM32G070xx)
        gpioinitstruct.Alternate = GPIO_AF0_SPI1;
#else /* !defined(STM32G070xx) */
        gpioinitstruct.Alternate = GPIO_AF5_SPI1;
#endif /* End of defined(STM32G070xx) */
        HAL_GPIO_Init(GPIOB, &gpioinitstruct);

        /* Configure SPI MISO and MOSI */ 
        gpioinitstruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
#if defined(STM32G070xx)
        gpioinitstruct.Alternate = GPIO_AF0_SPI1;
#else /* !defined(STM32G070xx) */
        gpioinitstruct.Alternate = GPIO_AF5_SPI1;
#endif /* End of defined(STM32G070xx) */
        gpioinitstruct.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOB, &gpioinitstruct);

        /* Configure SPI NSS */
        gpioinitstruct.Pin  = GPIO_PIN_10;
        gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;  
        gpioinitstruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioinitstruct);

        /*** Configure the SPI peripheral ***/ 
        /* Enable SPI clock */
        __HAL_RCC_SPI1_CLK_ENABLE();

        // SPI configuration
        __spi_handle[0].Instance = SPI1;
        // Master full-duplex configuration, data size = 8bits, CPOL = 0, CPHA = 0, NSS = 1
        __spi_handle[0].Init.Mode = SPI_MODE_MASTER;
        __spi_handle[0].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;

        __spi_handle[0].Init.DataSize = SPI_DATASIZE_8BIT;
        __spi_handle[0].Init.FirstBit = SPI_FIRSTBIT_MSB;

        __spi_handle[0].Init.Direction = SPI_DIRECTION_2LINES;
        __spi_handle[0].Init.NSS = SPI_NSS_SOFT;
        __spi_handle[0].Init.CLKPhase = SPI_PHASE_1EDGE;
        __spi_handle[0].Init.CLKPolarity = SPI_POLARITY_LOW;
        __spi_handle[0].Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        __spi_handle[0].Init.CRCPolynomial = 7;
        __spi_handle[0].Init.TIMode = SPI_TIMODE_DISABLE;

#if defined(STM32G070xx)
        __spi_handle[0].Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
		__spi_handle[0].Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif /* End of defined(STM32G070xx) */

        if ( HAL_SPI_Init(&__spi_handle[0]) != HAL_OK )
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int __initSPI2(void)
{
    if (HAL_SPI_GetState(&__spi_handle[1]) == HAL_SPI_STATE_RESET)
    {
        // SPI2 GPIO Configuration

        /*  GPIO pin configs 
            PB13 -------> SPI2_SCK
            PC2 --------> SPI2_MISO
            PC3 --------> SPI2_MOSI
            PA15 -------> SPI2_NSS
        */

        /* Enable GPIO clock */
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitTypeDef  gpioinitstruct = {0};

        /* Configure SPI SCK */
        gpioinitstruct.Pin = GPIO_PIN_13;
        gpioinitstruct.Mode = GPIO_MODE_AF_PP;
        gpioinitstruct.Pull  = GPIO_PULLUP;
        gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#if defined(STM32G070xx)
        gpioinitstruct.Alternate = GPIO_AF0_SPI2;
#else /* !defined(STM32G070xx) */
        gpioinitstruct.Alternate = GPIO_AF5_SPI2;
#endif /* End of defined(STM32G070xx) */    
        HAL_GPIO_Init(GPIOB, &gpioinitstruct);

        /* Configure SPI MISO and MOSI */ 
        gpioinitstruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
#if defined(STM32G070xx)    
        gpioinitstruct.Alternate = GPIO_AF0_SPI2;
#else /* !defined(STM32G070xx) */  
        gpioinitstruct.Alternate = GPIO_AF5_SPI2;
#endif /* End of defined(STM32G070xx) */
        gpioinitstruct.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init(GPIOC, &gpioinitstruct);

        /* Configure SPI NSS */
        gpioinitstruct.Pin  = GPIO_PIN_15;
        gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;  
        gpioinitstruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioinitstruct);

        /*** Configure the SPI peripheral ***/ 
        /* Enable SPI clock */
        __HAL_RCC_SPI2_CLK_ENABLE();

        // SPI configuration
        __spi_handle[1].Instance = SPI2;
        // Master full-duplex configuration, data size = 8bits, CPOL = 0, CPHA = 0, NSS = 1
        __spi_handle[1].Init.Mode = SPI_MODE_MASTER;
        __spi_handle[1].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;

        __spi_handle[1].Init.DataSize = SPI_DATASIZE_8BIT;
        __spi_handle[1].Init.FirstBit = SPI_FIRSTBIT_MSB;

        __spi_handle[1].Init.Direction = SPI_DIRECTION_2LINES;
        __spi_handle[1].Init.NSS = SPI_NSS_SOFT;
        __spi_handle[1].Init.CLKPhase = SPI_PHASE_1EDGE;
        __spi_handle[1].Init.CLKPolarity = SPI_POLARITY_LOW;
        __spi_handle[1].Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        __spi_handle[1].Init.CRCPolynomial = 7;
        __spi_handle[1].Init.TIMode = SPI_TIMODE_DISABLE;

#if defined(STM32G070xx)
        __spi_handle[1].Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
        __spi_handle[1].Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif /* End of defined(STM32G070xx) */    
    
            if ( HAL_SPI_Init(&__spi_handle[1]) != HAL_OK )
            {
                return FAILURE;
            }
    }
    return SUCCESS;
}

/*------------------------------------------------------------------------------*/
/*					  	 Function prototypes Implement	    				    */
/*------------------------------------------------------------------------------*/
int __InitSPI()
{
    int ret = SUCCESS;

    ret |= __initSPI1();
    ret |= __initSPI2();

    return ret;
}

//Write data to SPI device. Returns number of bytes written on success, -1 on failure. //Ignores data returned from SPI device.
int hal__SPIWRITE(uint8_t spi_num, uint8_t *data, uint16_t len)
{
    param_check( (1 <= spi_num) && (spi_num <= 2) );
	param_check( data != NULL );
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&__spi_handle[spi_num - 1], data, len, SPI_DEFAULT_TIMEOUT);
    if ( status == HAL_OK)
    {
        return len; 
	}
	else if (status == HAL_TIMEOUT)
	{
		return len - __spi_handle[spi_num - 1].TxXferCount;
	}
    return FAILURE;
}

//Read data from SPI device. Returns number of bytes read on success, -1 on failure. //clocks out 0x00 to SPI device.
int hal__SPIREAD(uint8_t spi_num, uint8_t *data, uint16_t len)
{
    param_check( (1 <= spi_num) && (spi_num <= 2) );
	param_check( data != NULL );
	HAL_StatusTypeDef status = HAL_SPI_Receive(&__spi_handle[spi_num - 1], data, len, SPI_DEFAULT_TIMEOUT);
	if ( status == HAL_OK)
	{
		return len;
	}
	else if (status == HAL_TIMEOUT)
	{
		return len - __spi_handle[spi_num - 1].RxXferCount;
	}
	return FAILURE;
}

 //Write data to SPI device while reading data from SPI device. Returns number of bytes read on success, -1 on failure.
int hal__SPIWRITE_READ(uint8_t spi_num, uint8_t* data_out, uint8_t* data_in, uint16_t len)
{
	param_check( (1 <= spi_num) && (spi_num <= 2) );
	param_check( data_out != NULL );
	param_check( data_in != NULL );
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&__spi_handle[spi_num - 1], data_out, data_in, len, SPI_DEFAULT_TIMEOUT);
	if(status == HAL_OK)
	{
		return len;
	}
	else if (status == HAL_TIMEOUT)
	{
		return len - __spi_handle[spi_num - 1].RxXferCount;
	}
	return FAILURE;
}
