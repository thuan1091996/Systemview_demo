/*------------------------------------------------------------------------------*/
/*							 Includes and dependencies						    */
/*------------------------------------------------------------------------------*/
#include "hal.h"

/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor defines			    			    */
/*------------------------------------------------------------------------------*/
#define ADC_TIMEOUT_MS	            (3000)
#define ADC_DEFAULT_RESOLUTION      ADC_RESOLUTION_12B
#define ADC_DEFAULT_SAMPLINGTIME    ADC_SAMPLETIME_144CYCLES

/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor MACRO			    			    */
/*------------------------------------------------------------------------------*/
#define ADC_PIN_NUM                 (sizeof(ADC_PIN)/sizeof(tADCDef))

/*------------------------------------------------------------------------------*/
/*					  		   Typedefs			    			                */
/*------------------------------------------------------------------------------*/
typedef struct
{
    tGpioDef     Gpio;
    uint8_t      Channel;
} tADCDef;
/*------------------------------------------------------------------------------*/
/*					  		   Variables Declare			    			    */
/*------------------------------------------------------------------------------*/
const tADCDef ADC_PIN[] = {
        { .Channel = ADC_CHANNEL_0, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_0}  /*0*/},
        { .Channel = ADC_CHANNEL_1, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_1}  /*1*/},
        { .Channel = ADC_CHANNEL_2, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_2}  /*2*/},
        { .Channel = ADC_CHANNEL_3, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_3}  /*3*/},
        { .Channel = ADC_CHANNEL_4, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_4}  /*4*/},
        { .Channel = ADC_CHANNEL_5, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_5}  /*5*/},
        { .Channel = ADC_CHANNEL_6, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_6}  /*6*/},
        { .Channel = ADC_CHANNEL_7, .Gpio = {.Port = GPIOA, .Pin = GPIO_PIN_7}  /*7*/},
        { .Channel = ADC_CHANNEL_8, .Gpio = {.Port = GPIOB, .Pin = GPIO_PIN_0}  /*8*/},
        { .Channel = ADC_CHANNEL_9, .Gpio = {.Port = GPIOB, .Pin = GPIO_PIN_1}  /*9*/},
};

ADC_HandleTypeDef hadc1;
/*------------------------------------------------------------------------------*/
/*					  	  Function Private Implement		    			    */
/*------------------------------------------------------------------------------*/
static int __ADC1_config_channel(uint8_t channel)
{
    // Configure the channel
    ADC_ChannelConfTypeDef adc_channel_config = {0};
    adc_channel_config.Channel = channel;
    adc_channel_config.Rank = 1;
    adc_channel_config.SamplingTime = ADC_DEFAULT_SAMPLINGTIME;
    if (HAL_OK != HAL_ADC_ConfigChannel(&hadc1, &adc_channel_config))
    {
        return FAILURE;
    }
    return SUCCESS;
}

static int __ADC1_read(uint8_t channel, uint16_t *adc_raw)
{
    // Configure the channel
    if ( SUCCESS != __ADC1_config_channel(channel))
    {
        return FAILURE;
    }

    if (HAL_OK != HAL_ADC_Start(&hadc1))
    {
        return FAILURE;
    }

    int ret_val;
    do
    {
        if (HAL_OK != HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT_MS))
        {
            ret_val = FAILURE;
            break;
        }
        *adc_raw = HAL_ADC_GetValue(&hadc1);
            ret_val = SUCCESS;
    } while (0);
    HAL_ADC_Stop(&hadc1);
    return ret_val;
}

static int __ADC1_read_Vref(uint16_t* vref_raw)
{
    // Configure the channel
    if ( SUCCESS != __ADC1_config_channel(ADC_CHANNEL_VREFINT))
    {
        return FAILURE;
    }

    if (HAL_OK != HAL_ADC_Start(&hadc1))
    {
        return FAILURE;
    }
    if (HAL_OK != HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT_MS))
    {
    	return FAILURE;
    }
    *vref_raw = HAL_ADC_GetValue(&hadc1);

    if (HAL_OK != HAL_ADC_Stop(&hadc1))
    {
        return FAILURE;
    }
    return SUCCESS;
}


/*------------------------------------------------------------------------------*/
/*					  	 Function prototypes Implement	    				    */
/*------------------------------------------------------------------------------*/
int __initADC1()
{
	/*ADC Pin configuration*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* ADC1 GPIO Configuration *\
    PA0     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    */
    // Pin configuration for PORTA
    GPIO_InitStruct.Pin =   GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
                            GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // Pin configuration for PORTB
    GPIO_InitStruct.Pin =   GPIO_PIN_0|GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*ADC clock configuration*/
#if 0
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
#endif /* End of 0 */
    __HAL_RCC_ADC1_CLK_ENABLE();

    //  ADC configuration
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_DEFAULT_RESOLUTION;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_39CYCLES_5;
    hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_39CYCLES_5;
    hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.LowPowerAutoPowerOff = DISABLE;
    hadc1.Init.OversamplingMode = DISABLE;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        return FAILURE;
    }
    return SUCCESS;
}


/**
  * @brief  This function will read the value of the adc pin
  * @param  pin : the pin to read
  * @retval the raw value of the adc
  */
int hal__ADCRead(uint8_t pin)
{
    param_check((pin >= 0) && (pin < ADC_PIN_NUM));

    // Check if ADC is initialized
    if((hadc1.State & HAL_ADC_STATE_READY) == 0)
    {
    	if ( SUCCESS != __initADC1())
    		return FAILURE;
    }

    uint16_t adc_raw;
    if( SUCCESS != __ADC1_read(ADC_PIN[pin].Channel, &adc_raw))
    {
    	return FAILURE;
    }
    return adc_raw;
}

int hal__ADCReadMV(uint8_t pin)
{
    int raw_adc = hal__ADCRead(pin);
    // Read and calculate Vref voltage
    uint16_t vref_raw, vref_volt;
    if (__ADC1_read_Vref(&vref_raw) != SUCCESS)
    {
        return FAILURE;
    }
    vref_volt = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vref_raw, ADC_DEFAULT_RESOLUTION);
    // Calculate the voltage of the pin
    int adc_volt = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_volt, raw_adc, ADC_DEFAULT_RESOLUTION);
    return adc_volt;
}


