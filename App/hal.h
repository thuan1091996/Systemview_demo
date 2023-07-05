#ifndef HAL_H
#define HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------*/
/*							 Includes and dependencies						    */
/*------------------------------------------------------------------------------*/
#if TESTING_UART3
#include "stm32f4xx_hal.h"
#else
#include "stm32g0xx_hal.h"
#endif /* End of TESTING_UART3 */


#include "stdbool.h"

/*------------------------------------------------------------------------------*/
/*					  		   Preprocessor Constants						    */
/*------------------------------------------------------------------------------*/
#define SUCCESS                 0
#define FAILURE                 -1

#define param_check(param)	    if ( !(param) ) return FAILURE
#define error_check(con, error) if ( con ) return error


/*-----------------------------------------------------------------------------*/
/*							    Function prototypes					     	   */
/*-----------------------------------------------------------------------------*/

//Serial LTE driver for nrf9160
//Integrates with hal driver (hal.h)
//nrf9160 is connected via UART. UART is configured in hal.h

//Implements API calls defined here: 
//General Commands: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fref_at_commands%2FREF%2Fat_commands%2Fintro.html
//NRF9160 Serial LTE Commands: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/AT_commands_intro.html#slm-at-intro


//All commands need to have "\r\n" appended to the end. This is the carriage return and line feed characters.

//uses snprintf for char* formatting and interpretation
//Uses STD C library "string.h" for string manipulation - https://www.tutorialspoint.com/c_standard_library/string_h.htm

//Commands Implemented:


//Public Functions - Meant for direct use - all block for response to return data.
int nrf_slte__power_on(void); //Implements [AT+CFUN=21] (Enables LTE modem.), Waits for "OK" response. Returns 0 if ok. Returns -1 if error.
int nrf_slte__power_off(void); //Implements [AT+CFUN=0] (Disables LTE modem.), Waits for "OK" response. Returns 0 if ok. Returns -1 if error.
int nrf_slte__connected(void); //Implements [AT+COPS?] returns 1 if connected, 0 if not connected, -1 if error //https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/nw_service/cops_read.html
long nrf_slte__get_time(void); //Implements [AT#CARRIER="time","read"] to get time, returns seconds since UTC time 0. //https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/CARRIER_AT_commands.html#lwm2m-carrier-library-xcarrier
int nrf_slte__get_rssi(void); //Implements [AT+CESQ] (Signal Quality Report), Waits for Response, Returns RSSI value in dBm. https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/mob_termination_ctrl_status/cesq_set.html
int nrf_slte__get_SimPresent(void); //Implements [TBD] command, returns 1 if SIM is present, 0 if not present, -1 if error
int nrf_slte__setCA(char* ca); //calls "__nrf_slte__clearCert()" to clear the certificate, if it exists. Then, Implements [AT%CMNG=0,12354,0,\"<ca>\"] to set CA certificate, where <ca> represents the contents of ca, with the null terminator removed. Then, enables the modem using "nrf_slte__power_on()" Returns 0 if ok. Returns -1 if error. https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/Generic_AT_commands.html#native-tls-cmng-xcmng

int nrf_slte__httpsGET(char* url, char* response, uint16_t maxlength); //if url = "google.com/myurl" Implements [AT#XHTTPCCON=1,\"google.com\",443,12354], waits for a valid reply, then implements [AT#XHTTPCREQ=\"GET\",\"/myurl\"]. Returns 0 if ok. Returns -1 if error. Returns response in response char array. https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/HTTPC_AT_commands.html
int nrf_slte__httpsPOST(char* url, char* JSONdata, char* agent, char* response, uint16_t maxlength); //if url = "google.com/myurl" Implements [AT#XHTTPCCON=1,\"google.com\",443,12354], waits for a valid reply, then implements [AT#XHTTPCREQ=\"POST\",\"/myurl\",\"User-Agent: <agent>\r\n\",\"application/json\","<JSONdata>\"] where <agent> is the contents of agent, with the null terminator removed, and <JSONdata> is the contents of JSONdata, with the null terminator removed. Returns 0 if ok. Returns -1 if error. Returns response in response char array. https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/HTTPC_AT_commands.html

int nrf_slte__getData(char* data, uint16_t maxlength, bool block); //returns number of characters read if ok. if "block" is true, wait for the next HTTP Response. Handles unsolicited [AT#XHTTPCRSP=... notifications] - https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/HTTPC_AT_commands.html#http-response-xhttpcrsp

//Private Functions - Meant for internal use
int __nrf_slte__send_command(char* command); //Sends a command. This is a null terminated char array. For example: "AT+CFUN=1\0"  //This function is blocking, waits for a response.
int __nrf_slte__get_resp(char* resp, uint16_t maxlength); //Gets a response. returns actual length of the response.

int __nrf_slte__clearCert(); //Implements [AT+CFUN=4] (turns off modem), Waits for OK. Implements [AT%CMNG=3,12354,0] Clears certificate #12354 if it exists. Returns 0 if ok. Returns -1 if error.  


/* HAL Framework for */


int hal__init(); //Initialize HAL. Returns 0 on success, -1 on failure.

/* TIMER_HELPER_FUNCTIONS */
int hal__setDutyCycle(uint8_t channelNum, uint16_t dutyCycle_tenth); //Set Duty Cycle, in tenths of percent. For Example, Passing (1, 50) will set Timer 1 Channel 1 to 5.0%. Returns 0 on success, -1 on failure.
//Note: channelNum should be 0-indexed. (0,1,2,3,4,5,6,7,8) - not related to the real physical pins or timer peripherals.
//Note: Setting this value should not directly replace the capture/compare value - it should save this value and replace the capture/compare value on the next timer overflow.

/* GPIO_HELPER_FUNCTIONS */
int hal__setHigh(uint8_t pinNum);
int hal__setLow(uint8_t pinNum);
int hal__read(uint8_t pinNum);

int hal__setState(uint8_t pinNum, uint8_t state); //(0,0) sets pin 0 as input, (0,1) sets pin 0 as output, (0,2) sets pin as high impedance. Returns 0 on success, -1 on failure.
//Note: link pinNum to the datasheet labels in a logical way PA[0:15], PB[0:15], PC[0:15], etc.

/* UART_HELPER_FUNCTIONS */
int hal__UARTAvailable(uint8_t uartNum); //Returns number of bytes available to read from UART. Returns 0 on success, -1 on failure.  //NOTE: if API limitations only allow for knowing IF data is available, return 1 if data is available.

int hal__UARTWrite_uint8(uint8_t uartNum, uint8_t data); //Write data to UART. Returns 0 on success, -1 on failure.
int hal__UARTWrite(uint8_t uartNum, uint8_t *data, uint16_t len); //Write data to UART. Returns number of bytes written on success, -1 on failure.

int hal__UARTRead_uint8(uint8_t uartNum, uint8_t *data); //Read data from UART. Returns 0 on success, -1 on failure.
int hal__UARTRead(uint8_t uartNum, uint8_t *data, uint16_t len); //Read data from UART. Returns number of bytes read on success, -1 on failure.

/* I2C_HELPER_FUNCTIONS */
bool hal__I2CEXISTS(uint8_t i2c_num, uint8_t ADDR); //Returns true if I2C device exists at ADDR, false if not.

int hal__I2CREAD_uint8(uint8_t i2c_num, uint8_t ADDR, uint8_t REG, uint8_t *data); //Read data from I2C device at ADDR, register REG. Returns 0 on success, -1 on failure.
int hal__I2CREAD(uint8_t i2c_num, uint8_t ADDR, uint8_t REG, uint8_t *data, uint16_t len); //Read data from I2C device at ADDR, register REG. Returns number of bytes read on success, -1 on failure.

int hal__I2CWRITE_uint8(uint8_t i2c_num, uint8_t ADDR, uint8_t REG, uint8_t data); //Write data to I2C device at ADDR, register REG. Returns 0 on success, -1 on failure.
int hal__I2CWRITE(uint8_t i2c_num, uint8_t ADDR, uint8_t REG, uint8_t *data, uint16_t len); //Write data to I2C device at ADDR, register REG. Returns number of bytes written on success, -1 on failure.


#ifdef __cplusplus
}
#endif

#endif /* HAL_H */
