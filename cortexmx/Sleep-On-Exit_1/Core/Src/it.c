/*
 * it.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"

extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;

void SysTick_Handler(void) {

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(void) {

	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htim6);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

void USART2_IRQHandler(void) {

	HAL_UART_IRQHandler(&huart2);
}
