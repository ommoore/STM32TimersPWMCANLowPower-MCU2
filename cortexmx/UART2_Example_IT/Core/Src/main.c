/*
 * main.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include <string.h>
#include "main.h"

void SystemClockConfig(void);
void UART2_Init(void);
void Error_handler(void);

#define TRUE 	1
#define	FALSE 	0

UART_HandleTypeDef huart2;

uint8_t rx_buffer[100];
uint8_t rx_data;
uint32_t count = 0;
uint8_t rxcmplt = FALSE;


char *user_data = "The application is running\r\n";

int main(void) {

	HAL_Init();
	SystemClockConfig();
	UART2_Init();

	uint16_t len_of_data = strlen(user_data);

	HAL_UART_Transmit(&huart2, (uint8_t*)user_data, len_of_data, HAL_MAX_DELAY);

	while(rxcmplt != TRUE) {
		HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	}

	while(1);

	return 0;
}

void SystemClockConfig(void) {

}

void UART2_Init(void) {

	huart2.Instance = USART2; //linking
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;

	if(HAL_UART_Init(&huart2) != HAL_OK) {

		//There is a problem
		Error_handler();
	}
}

//Callback function invoked when the desired number bytes are received by the uart peripheral
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if(huart->Instance == USART2) {
		if(rx_data == '\r') {
			rxcmplt = TRUE;
			rx_buffer[count++] = '\r';
			HAL_UART_Transmit(huart, rx_buffer, count, HAL_MAX_DELAY);

		} else {
			rx_buffer[count++] = rx_data;
		}
	}
}

void Error_handler(void) {
	while(1);
}
