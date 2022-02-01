/*
 * it.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"

extern TIM_HandleTypeDef htim6;

void SysTick_Handler(void) {

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI15_10_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void TIM6_DAC_IRQHandler(void) {

	HAL_TIM_IRQHandler(&htim6);
}
