/*
 * main.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"

void Error_handler(void);
void GPIO_Init(void);
void TIM6_Init(void);
void SystemClock_Config(void);

TIM_HandleTypeDef htim6;


int main(void) {

	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
	TIM6_Init();

	//Start timer (Interrupt mode - Non-Blocking)
	HAL_TIM_Base_Start_IT(&htim6);

	while(1);

	return 0;
}

void SystemClock_Config(void) {

}

void GPIO_Init(void) {

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}

void TIM6_Init(void) {

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 24;
	htim6.Init.Period = 64000 - 1;
	if(HAL_TIM_Base_Init(&htim6) != HAL_OK) {
		Error_handler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void Error_handler(void) {
	while(1);
}
