#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#define SUCCESS                         (0)
#define FAILURE                         (-1)
#define AT_BUFFER_SIZE                  (1024UL)
#define DEBUG_OS                        (1)

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#define AT_DEFAULT_UART_PORT            (1)
#define AT_DEFAULT_TIMEOUT_MS           (3000UL)
#define AT_BUFFER_SIZE                  (1024UL)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

#if DEBUG_OS
#define PORT_DELAY_MS(MS)               Sleep(MS)      
#define PORT_GET_SYSTIME_MS()           clock()
#define param_check(param)	            assert(param)      
#else
#define PORT_DELAY_MS(MS)               HAL_Delay(MS)
#define PORT_GET_SYSTIME_MS()           HAL_GetTick()
#define param_check(param)	            if ( !(param) ) return FAILURE
#endif
/*
 * *********************************************************************************************
 * WRAPPER FUNCTIONS
 * *********************************************************************************************
 */
int hal__UARTWrite(uint8_t uartNum, uint8_t *data, uint16_t len)
{
    fflush(stdin);
    printf("Sending: %s\n", data);
    return SUCCESS;
}


/** \file at_cmd.c
 *  \brief This module contains the
 */
/******************************************************************************
* Includes
*******************************************************************************/



/*
 * *********************************************************************************************
 * TESTING FUNCTIONS
 * *********************************************************************************************
 */


/******************************************************************************
* Module Typedefs
*******************************************************************************/
typedef struct 
{
    char rx_data[AT_BUFFER_SIZE];
    uint16_t rx_len;
} at_resp_data_mailbox_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
at_resp_data_mailbox_t at_rx_data = {0};

/******************************************************************************
* Internal Function Prototypes
*******************************************************************************/
int mailbox__put_data(at_resp_data_mailbox_t* mailbox, char* data, uint16_t len)
{
    param_check(mailbox != NULL);
    param_check(data != NULL);
    if(mailbox->rx_len + len > sizeof(mailbox->rx_data))
        return FAILURE;
    memcpy(&mailbox->rx_data[mailbox->rx_len], data, len);
    mailbox->rx_len += len;
    return SUCCESS;
}

int mailbox__get_data(at_resp_data_mailbox_t* mailbox, char* data, uint16_t len)
{
    param_check(mailbox != NULL);
    param_check(data != NULL);
    if(mailbox->rx_len < len)
        return FAILURE;
    memcpy(data, mailbox->rx_data, len);
    mailbox->rx_len -= len;
    memmove(mailbox->rx_data, &mailbox->rx_data[len], mailbox->rx_len);
    return SUCCESS;
}

int mailbox__get_len(at_resp_data_mailbox_t* mailbox)
{
    param_check(mailbox != NULL);
    return mailbox->rx_len;
}

void mailbox__flush(at_resp_data_mailbox_t* mailbox)
{
    param_check(mailbox != NULL);
    mailbox->rx_len = 0;
    memset(mailbox->rx_data, 0, sizeof(mailbox->rx_data));
}

#if DEBUG_OS == 1
int __nrf_slte__wait_4response(char* expected_resp, uint16_t timeout_ms)
{
    uint16_t recv_idx = 0;
    uint8_t rcv_buf[AT_BUFFER_SIZE]; // Temp buffer to receive data from UART
    memset(rcv_buf, 0, sizeof(rcv_buf));
    uint64_t max_recv_timeout = PORT_GET_SYSTIME_MS() + AT_DEFAULT_TIMEOUT_MS;
    // Wait until maximum AT_DEFAULT_TIMEOUT_MS to receive expected_resp
    while (PORT_GET_SYSTIME_MS() < max_recv_timeout)
    {
        char input_recv[AT_BUFFER_SIZE] = {0};
        scanf("%[^\n]", input_recv);
        uint16_t recv_len = strlen(input_recv);
        printf("Receiving: %s \r\n \r\n", input_recv);

        if(recv_idx + recv_len > sizeof(rcv_buf) )
            break; // Overflow
            
        if(recv_len > 0)
        {
            // hal__UARTRead(AT_DEFAULT_UART_PORT, &rcv_buf[recv_idx], recv_len);
            memcpy(&rcv_buf[recv_idx], input_recv, recv_len);
            recv_idx += recv_len;

            if(strstr(rcv_buf, "ERR") != NULL)
                return FAILURE;
            else if(strstr(rcv_buf, expected_resp) != NULL)
            {
                mailbox__put_data(&at_rx_data, rcv_buf, recv_idx);
                return SUCCESS;
            }
        }
        PORT_DELAY_MS(100); // Wait for UART buffer to fill up
    }
    return FAILURE;
}
#else
int __nrf_slte__wait_4response(char* expected_resp, uint16_t timeout_ms)
{
    uint16_t recv_idx = 0;
    uint8_t rcv_buf[AT_BUFFER_SIZE]; // Temp buffer to receive data from UART
    memset(rcv_buf, 0, sizeof(rcv_buf));

    uint64_t max_recv_timeout = PORT_GET_SYSTIME_MS() + AT_DEFAULT_TIMEOUT_MS;
    // Wait until maximum AT_DEFAULT_TIMEOUT_MS to receive expected_resp
    while (PORT_GET_SYSTIME_MS() < max_recv_timeout)
    {
        uint16_t recv_len = hal__UARTAvailable(AT_DEFAULT_UART_PORT);

        if(recv_idx + recv_len > sizeof(rcv_buf) )
            break; // Overflow
            
        if(recv_len > 0)
        {
            hal__UARTRead(AT_DEFAULT_UART_PORT, &rcv_buf[recv_idx], recv_len);
            recv_idx += recv_len;

            if(strstr(rcv_buf, "ERR") != NULL)
                return FAILURE;
            else if(strstr(rcv_buf, expected_resp) != NULL)
            {
                mailbox__put_data(&at_rx_data, rcv_buf, recv_idx);
                return SUCCESS;
            }
        }
        PORT_DELAY_MS(100); // Wait for UART buffer to fill up
    }
    return FAILURE;
}
#endif /* End of DEBUG_OS == 1 */

int __nrf_slte__get_resp(char* resp, uint16_t maxlength)
{
    param_check(resp != NULL);
    uint16_t cur_len = mailbox__get_len(&at_rx_data);
    if( cur_len != 0)
    {
        mailbox__get_data(&at_rx_data, resp, cur_len);
        mailbox__flush(&at_rx_data);
    }
    return cur_len;
}


/**
 * @brief Get the rssi from CESQ response string
 * @param cesq_response 
 * @return RSSI in dBm
 * @note: More on: https://devzone.nordicsemi.com/f/nordic-q-a/71958/how-know-rssi
 */
int __nrf_slte__cal_rssi_from_cesq(char* cesq_response) 
{
    param_check(cesq_response != NULL);
    int rsrq, rsrp, rssi;
    int status = sscanf(cesq_response, "+CESQ: %*d,%*d,%*d,%*d,%d,%d", &rsrq, &rsrp);
    if(status != 2)
        return FAILURE;
    rssi = (double)(10 * log10(6) + (rsrp - 140) - (rsrq - 19.5));
    return rssi;
}

/**
 * @brief Get the unix timestamp from datetime object   
 * 
 * @param datetime 
 * @return FAILURE if error, else unix timestamp in seconds
 */
long __nrf_slte__get_unix_timestamp(const char* datetime)
{
    int year, month, day, hour, minute, second;
    param_check(datetime != NULL);
    if(sscanf(datetime, "UTC_TIME: %d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second) != 6)
    {
        printf("Invalid datetime format\n");
        return FAILURE;
    }

    struct tm current_time = {0};
    current_time.tm_year = year - 1900; // Year since 1900
    current_time.tm_mon = month - 1; // Months since January [0-11]
    current_time.tm_mday = day; 
    current_time.tm_hour = hour;
    current_time.tm_min = minute;
    current_time.tm_sec = second;

    time_t t = mktime(&current_time);

    // Check if time conversion is successful
    if(t == -1){
        printf("Error: unable to make time using mktime\n");
        return FAILURE;
    }

    return (long)t;
}

/**
 * @brief Parse the URL to get the host and path
 * 
 * @param url 
 * @param host 
 * @param path 
 */
void __nrf_slte__parse_url(const char* url, char* host, char* path)
{
    param_check(url != NULL);
    param_check(host != NULL);
    param_check(path != NULL);

    /* Extract hostname and path from URL */
    const char* slash = strchr(url, '/');
    if (slash) 
    { 
        // '/' found, Get the length of the host by subtracting pointers
        uint8_t host_len = slash - url;
        memcpy(host, url, host_len);
        host[host_len] = '\0';
        strcpy(path, slash);
    } 
    else 
    {
        // No '/' found, assume that the entire URL is the host
        strcpy(host, url);
        path[0] = '/';
        path[1] = '\0';
    }
}

/*
 * Implements [AT+CFUN=4] (turns off modem), Waits for OK. 
 * Implements [AT%CMNG=3,12354,0] Clears certificate #12354 if it exists. Returns 0 if ok. Returns -1 if error.   
 */
int __nrf_slte__clearCert()
{
    char resp[AT_BUFFER_SIZE] = {0};

    if (__nrf_slte__send_command("AT+CFUN=4\r\n") != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    if (__nrf_slte__send_command("AT%CMNG=3,12354,0\r\n") != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")
    return SUCCESS;
}

/**
 * @brief: Get the content length from the response header lines
 * 
 * @param resp: The response header from the server 
 * @return int: The content length of the response body
 */
int __nrf_slte__get_http_content_len(const char* resp)
{
    param_check(resp != NULL);
    int content_length;
    char* content_length_str = strstr(resp, "Content-Length: ");
    if(content_length_str == NULL)
        return FAILURE; // Invalid response
    sscanf(content_length_str, "Content-Length: %d", &content_length);
    return content_length;
}

int __nrf_slte__send_command(char* command)
{
    param_check(command != NULL);
    mailbox__flush(&at_rx_data);
    return hal__UARTWrite(AT_DEFAULT_UART_PORT, command, strnlen(command, AT_BUFFER_SIZE));
}


/******************************************************************************
* Function Definitions
*******************************************************************************/
/*
 *  Implements [AT+CFUN=21] (Enables LTE modem.)
 *  Waits for "OK" response. Returns SUCCESS if ok. Returns FAILURE if error.
 */
int nrf_slte__power_on(void)
{
    if (__nrf_slte__send_command("AT+CFUN=21\r\n") != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    return SUCCESS;
} 

/*
 * Implements [AT+CFUN=0] (Disables LTE modem.)
 * Waits for "OK" response. Returns SUCCESS if ok. Returns FAILURE if error.
 */
int nrf_slte__power_off(void)
{
    if (__nrf_slte__send_command("AT+CFUN=0\r\n") != SUCCESS)
        return FAILURE; // Failed to send command
    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")
    return SUCCESS;
}


/*
 * Implements [AT+CESQ] (Signal Quality Report), Waits for Response, Returns RSSI value in dBm.
 * https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/mob_termination_ctrl_status/cesq_set.html 
 * Example:
 * "AT+CESQ"
 * "+CESQ: 99,99,255,255,31,62 <CR><LF> OK"
 */
int nrf_slte__get_rssi(void)
{
    int ret = FAILURE;
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT+CESQ\r\n") != SUCCESS)
        return FAILURE; // Failed to send command
    
    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    int recv_len = __nrf_slte__get_resp(resp, AT_BUFFER_SIZE);
    char* cesq_response_str = strstr(resp, "+CESQ: ");
    if(cesq_response_str == NULL)
        return FAILURE; // Invalid response

    //Get first token to extract RSSI
    cesq_response_str = strtok(cesq_response_str, "\n");
    if(cesq_response_str == NULL)
        return FAILURE; // Invalid response
    
    int rssi = __nrf_slte__cal_rssi_from_cesq(cesq_response_str);

    return rssi;
}

/*
 * TODO: Implement
 * Implements [AT+COPS?] 
 * Expected Response: +COPS: (<status>,"long","short","numeric",<AcT>) // AcT - Access Technology
 * returns 1 if connected, 0 if not connected, -1 if error 
 * //https://infocenter.nordicsemi.com/topic/ref_at_commands/REF/at_commands/nw_service/cops_read.html
 */
int nrf_slte__connected(void)
{
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT+COPS?\r\n") != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")
    
    int recv_len = __nrf_slte__get_resp(resp, AT_BUFFER_SIZE);
    
    char* cops_response_str = strstr(resp, "ERROR");
    if(cops_response_str != NULL)
        return FAILURE; // Invalid response

    cops_response_str = strstr(resp, "+COPS: ");
    if(cops_response_str == NULL)
        return FAILURE; // Invalid response

    // Retrieve network status
    char* network_status = sscanf(cops_response_str, "+COPS: %d", &network_status);
    if(network_status == NULL)
        return FAILURE; // Invalid response
    
    if(network_status == 2)
        return 1; // Connected
    else
        return 0; // Not connected
}
/*
 * Send +CPIN=? 
 * returns 1 if SIM is present (Resp == "READY"), 0 if not present (???), -1 if error
 */
int nrf_slte__get_SimPresent(void)
{
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT+CPIN=?\r\n") != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("READY", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    return 1;
}

/*
 * TODO: Implement
 * Implements [AT#CARRIER="time","read"] to get time returns seconds since UTC time 0. 
 * https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/CARRIER_AT_commands.html#lwm2m-carrier-library-xcarrier
 */
long nrf_slte__get_time(void)
{
    int ret = FAILURE;
    char resp[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__send_command("AT#XCARRIER=\"time\"") != SUCCESS)
        return FAILURE; // Failed to send command
    if (__nrf_slte__get_resp(resp, sizeof(resp)) == FAILURE)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    // Parse response given example: #XCARRIER: UTC_TIME: 2022-12-30T14:56:46Z, UTC_OFFSET: 60, TIMEZONE: Europe/Paris
    char* utc_time_str = strstr(resp, "UTC_TIME: ");
    if(utc_time_str == NULL)
        return FAILURE; // Invalid response
    char* utc_date_time_str = sscanf(utc_time_str, "UTC_TIME: %s", utc_time_str);
    if(utc_date_time_str == NULL)
        return FAILURE; // Invalid response

    long unix_timestamp = __nrf_slte__get_unix_timestamp(utc_date_time_str);
    if(unix_timestamp == FAILURE)
        return FAILURE; // Invalid response

    return unix_timestamp;
}

/*
 * calls "__nrf_slte__clearCert()" to clear the certificate, if it exists. 
 * Then, Implements [AT%CMNG=0,12354,0,\"<ca>\"] to set CA certificate,
 *  where <ca> represents the contents of ca, with the null terminator removed.
 *  Then, enables the modem using "nrf_slte__power_on()" Returns 0 if ok. Returns -1 if error. 
 * https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/Generic_AT_commands.html#native-tls-cmng-xcmng
 */
int nrf_slte__setCA(char* ca)
{
    char resp[AT_BUFFER_SIZE] = {0};
    char at_tx_buffer[AT_BUFFER_SIZE] = {0};
    if (__nrf_slte__clearCert() != SUCCESS)
        return FAILURE; // Failed to clear certificate //TODO: What if cert not exist

    sprintf(at_tx_buffer, "AT%CMNG=0,12354,0,%s\r\n", ca);
    if (__nrf_slte__send_command(at_tx_buffer) != SUCCESS)
        return FAILURE; // Failed to send command
    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")
    if ( nrf_slte__power_on() != SUCCESS)
        return FAILURE; // Failed to power on
    return SUCCESS;
}

// /*
//  * //if url = "google.com/myurl" Implements [AT#XHTTPCCON=1,\"google.com\",443,12354], waits for a valid reply
//  * then implements [AT#XHTTPCREQ=\"GET\",\"/myurl\"]. 
//  * Returns 0 if ok. Returns -1 if error. 
//  * Returns response in response char array. 
//  * https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/applications/serial_lte_modem/doc/HTTPC_AT_commands.html
//  */
// int nrf_slte__httpsGET(char* url, char* response, uint16_t maxlength)
// {
//     int status;
//     char resp[AT_BUFFER_SIZE] = {0};
//     char at_send_buffer[255] = {0};
//     char host[256] = {0};
//     char path[256] = {0};

//     /* Extract hostname and path from URL */
//     __nrf_slte__parse_url(url, host, path);
//     printf("Host: %s\n", host);
//     printf("Path: %s\n", path);

//     sprintf(at_send_buffer, "AT#XHTTPCCON=1,\"%s\",443,12354\r\n", host);
//     // Connect to HTTPS server using IPv4
//     if (__nrf_slte__send_command(at_send_buffer) != SUCCESS)
//         return FAILURE; // Failed to send command

//     if (__nrf_slte__get_resp(resp, sizeof(resp)) != SUCCESS)
//         return FAILURE; // Failed to receive resp (no "OK" received within timeout")

//     resp = strstr(resp, "XHTTPCCON");
//     if(resp == NULL)
//         return FAILURE; // Invalid response

//     scanf(resp, "XHTTPCCON: %d", &status);
//     if(status != 1)
//         return FAILURE; // Failed to connect to server

//     // Connected to server, send GET request
//     memset(resp, 0, strlen(resp));
//     memset(at_send_buffer, 0, strlen(at_send_buffer));
//     sprintf(at_send_buffer, "AT#XHTTPCREQ=\"GET\",\"%s\"\r\n", path);
//     if (__nrf_slte__send_command(at_send_buffer) != SUCCESS)
//         return FAILURE; // Failed to send command

//     if (__nrf_slte__get_resp(resp, sizeof(resp)) != SUCCESS)
//         return FAILURE; // Failed to receive resp (no "OK" received within timeout")
    
// //  Input
// //  HTTP header lines
// //  #XHTTPCRSP:337,1
// //  HTTP body
// //  #XHTTPCRSP:337,1

//     // Parse content length from HTTP response header lines
//     int content_length = __nrf_slte__get_http_content_len(resp);
//     if(content_length <= 0 || content_length > maxlength)
//         return FAILURE; // Invalid response
    
//     // Locate HTTP body
//     int body_len;
//     int state;
//     char* body = strtok(resp, "#XHTTPCRSP");
//     if(body == NULL)
//         return FAILURE; // Invalid response

//     // Copy HTTP body to response buffer
// }

int nrf_slte__httpsGET(char* url, char* response, uint16_t maxlength)
{
    int status;
    char resp[AT_BUFFER_SIZE] = {0};
    char at_send_buffer[255] = {0};
    char host[256] = {0};
    char path[256] = {0};
    char* response_data;
    int recv_len;

    /* Extract hostname and path from URL */
    __nrf_slte__parse_url(url, host, path);
    printf("Host: %s\n", host);
    printf("Path: %s\n", path);

    sprintf(at_send_buffer, "AT#XHTTPCCON=1,\"%s\",443,12354\r\n", host);
    // Connect to HTTPS server using IPv4
    if (__nrf_slte__send_command(at_send_buffer) != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    recv_len = __nrf_slte__get_resp(resp, AT_BUFFER_SIZE);

    response_data = strstr(resp, "#XHTTPCCON");
    if(response_data == NULL)
        return FAILURE; // Invalid response

    sscanf(response_data, "#XHTTPCCON: %d", &status);
    if(status != 1)
        return FAILURE; // Failed to connect to server

    // Connected to server, send GET request
    memset(resp, 0, strlen(resp)); //DELETE ME
    memset(at_send_buffer, 0, strlen(at_send_buffer));
    sprintf(at_send_buffer, "AT#XHTTPCREQ=\"GET\",\"%s\"\r\n", path);
    if (__nrf_slte__send_command(at_send_buffer) != SUCCESS)
        return FAILURE; // Failed to send command

    if (__nrf_slte__wait_4response("OK", 10000) != SUCCESS)
        return FAILURE; // Failed to receive resp (no "OK" received within timeout")

    recv_len = __nrf_slte__get_resp(resp, AT_BUFFER_SIZE);
    
    response_data = strstr(resp, "XHTTPCREQ");
    if(response_data == NULL)
        return FAILURE; // Invalid response
    sscanf(response_data, "XHTTPCREQ: %d", &status);
    if(status < 0)
        return FAILURE; // Failed to send request

//  Input
//  HTTP header lines
//  #XHTTPCRSP:337,1
//  HTTP body
//  #XHTTPCRSP:337,1

    // Parse content length from HTTP response header lines
    int content_length = __nrf_slte__get_http_content_len(resp);
    if(content_length <= 0 || content_length > maxlength)
        return FAILURE; // Invalid response
    
    // Locate HTTP body
    int body_len;
    int state;
    char* body = strtok(resp, "#XHTTPCRSP");
    if(body == NULL)
        return FAILURE; // Invalid response

    // Copy HTTP body to response buffer
    
    
}


volatile uint8_t g_test = 0;
int main() {
    printf("Testing HTTP Parser started \r\n");
    printf("************************************* START *************************************");
    printf("\n\r");
    printf("\n\r");
    
    while(1) {
    int temp_var = 0;
    switch(g_test)
    {

        case 5:
        {
            char http_resp_buffer[1024] = {0};
            nrf_slte__httpsGET("google.com/myurl", http_resp_buffer, 1024);
            break;
        }

        case 1:
        {
            if ( nrf_slte__power_on() != SUCCESS ) {
                printf("Failed to power on modem \r\n ");
            }
            else
            {
                printf("Modem powered on \r\n ");
            }
            break;
        }

        case 2:
        {
            if ( nrf_slte__power_off() != SUCCESS ) {
                printf("Failed to power off modem \r\n ");
            }
            else
            {
                printf("Modem powered off \r\n ");
            }
            break;
        }

        case 3:
        {
            if( (temp_var = nrf_slte__get_rssi()) == FAILURE)
            {
                printf("Failed to get RSSI \r\n ");
            }
            else
            {
                printf("RSSI: %d \r\n ", temp_var);
            }
            break;
        }

        case 4:
        {
            if( (temp_var = nrf_slte__connected()) != 1)
            {
                printf("Failed to get connection status \r\n ");
            }
            else
            {
                printf("Connection status %d \r\n", temp_var);
            }
            break;
        }
        
    }

    
    }
    printf("************************************* END ***************************************");
}
