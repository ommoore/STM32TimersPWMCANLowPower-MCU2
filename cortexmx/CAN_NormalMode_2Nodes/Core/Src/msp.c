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
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); //not required, as this will set to default setting

	//2. Enable the required system exceptions of the arm cortex mx processor
	//System Control Block (SCB) -> System Handler Control and State Register (SHCSR)
	SCB->SHCSR |= (0x7 << 16);  //Set bits 16,17,18 (MEMFAULTENA, BUSFAULTENA, USGFAULTENA)

	//3. Configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	//HAL_Init() in main.c already takes care of SysTick_IRQn priority setting
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {

	GPIO_InitTypeDef gpio_uart;

	/* Low level inits of the USART2 peripheral */

	//1. Enable the required peripheral clock for the USART2 and GPIOA peripherals
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//2. Pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_2; //USART2_TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3; //USART2_RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. Enable the peripheral IRQ in the NVIC
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	//4. Set the priorities based on application needs
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {

	GPIO_InitTypeDef gpio_can;

	__HAL_RCC_CAN1_CLK_ENABLE();

	//PA11 CAN1_RX
	//PA12 CAN1_TX
	gpio_can.Pin = GPIO_PIN_11 | GPIO_PIN_12;
	gpio_can.Mode = GPIO_MODE_AF_PP;
	gpio_can.Pull = GPIO_NOPULL;
	gpio_can.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_can.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &gpio_can);

	HAL_NVIC_SetPriority(CAN1_TX_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn,15,0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn,15,0);

	HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
	//1. Enable TIM6 Clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	//2. Enable TIM6 IRQ
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//3. Setup TIM6_DAC_IRQn priority
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
}
