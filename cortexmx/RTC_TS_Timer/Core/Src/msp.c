/*
 * msp.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"

void HAL_MspInit(void) {
	/* Low level processor specific inits */

	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

	//2. Enable the required system exceptions of the arm cortex mx processor
	//System Control Block (SCB) -> System Handler Control and State Register (SHCSR)
	SCB->SHCSR |= (0x7 << 16);  //Set bits 16,17,18 (MEMFAULTENA, BUSFAULTENA, USGFAULTENA)

	//3. Configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	/* BusFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	/* UsageFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	/* SVCall_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	/* DebugMonitor_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc) {

	if(hrtc->Instance == RTC) {
		/* Peripheral clock enable */
		__HAL_RCC_RTC_ENABLE();
	}
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc) {

	if(hrtc->Instance == RTC) {
		/* Peripheral clock disable */
		__HAL_RCC_RTC_DISABLE();
	}
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {

	if(htim_base->Instance == TIM6) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {

	if(htim_base->Instance == TIM6) {
		/* Peripheral clock disable */
		__HAL_RCC_TIM6_CLK_DISABLE();
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if(huart->Instance == USART2) {

		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {

	if(huart->Instance == USART2) {
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);
	}
}

