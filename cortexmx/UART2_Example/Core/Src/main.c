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

uint8_t convert_to_upper(uint8_t data);

UART_HandleTypeDef huart2;

char *user_data = "The application is running\r\n";

int main(void) {

	HAL_Init();
	SystemClockConfig();
	UART2_Init();

	uint16_t len_of_data = strlen(user_data);

	HAL_UART_Transmit(&huart2, (uint8_t*)user_data, len_of_data, HAL_MAX_DELAY);

	uint8_t rx_data;
	uint8_t rx_buffer[100];
	uint32_t count = 0;

	while(1) {
		HAL_UART_Receive(&huart2, &rx_data, 1, HAL_MAX_DELAY);
		if(rx_data == '\r') {
			break;
		} else {
			rx_buffer[count++] = convert_to_upper(rx_data);
		}

	}

	HAL_UART_Transmit(&huart2, rx_buffer, count, HAL_MAX_DELAY);
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

uint8_t convert_to_upper(uint8_t data) {

	if((data >= 'a') && (data <= 'z')) {
		data = data - ('a' - 'A'); //data = data - 32;
	}

	return data;
}

void Error_handler(void) {
	while(1);
}
