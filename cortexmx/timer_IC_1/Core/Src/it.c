/*
 * it.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;

void SysTick_Handler(void) {

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM2_IRQHandler(void) {

	HAL_TIM_IRQHandler(&htim2);
}

void TIM6_IRQHandler(void) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_TIM_IRQHandler(&htim6);
}
