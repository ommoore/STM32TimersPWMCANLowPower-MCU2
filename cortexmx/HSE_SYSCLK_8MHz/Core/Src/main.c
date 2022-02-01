/*
 * main.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include <string.h>
#include <stdio.h>
#include "main.h"

#define TRUE 	1
#define	FALSE 	0

void UART2_Init(void);
void Error_handler(void);


UART_HandleTypeDef huart2;

int main(void) {

	//By default, micro-controller uses HSI for SYSCLK.
	//This program configures SYSCLK to use HSE at 8MHz with HCLK at 4MHz, and PCLK1 and PCLK2 both at 2MHz.
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	char msg[100];

	HAL_Init();

	memset(&osc_init,0,sizeof(osc_init));
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS;
	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_handler();
	}

	clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
			 	 	 	  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2);
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

	if(HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK) {
		Error_handler();
	}

	/* ------------------ SYSCLK is now SOURCED by HSE ------------------ */

	__HAL_RCC_HSI_DISABLE(); //saves some current

	/* Modify SYSTICK Configuration */

	//HCLK = 4MHz
	//0.25us = 1 tick
	//1ms = (HCLK_freq / 1000)ticks = (4000000/1000)ticks
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	UART2_Init();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SYSCLK : %ld\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK : %ld\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK1 : %ld\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK2 : %ld\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	while(1);

	return 0;
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

void Error_handler(void) {
	while(1);
}
