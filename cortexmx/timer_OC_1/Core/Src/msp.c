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

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim) {

	GPIO_InitTypeDef tim2OC_ch_gpios;

	//1. Enable TIM2 and GPIOA peripheral clocks
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//2. Configure gpios PA0,1 to behave as TIM2 Channel 1,2 with AF1 mode
	tim2OC_ch_gpios.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	tim2OC_ch_gpios.Mode = GPIO_MODE_AF_PP;
	tim2OC_ch_gpios.Pull = GPIO_NOPULL;
	tim2OC_ch_gpios.Speed = GPIO_SPEED_FREQ_LOW;
	tim2OC_ch_gpios.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &tim2OC_ch_gpios);

	//**Conflict with PA2 and PA3 as they are already used by USART2 peripheral**
	//So we'll use PB10 and PB2 for TIM2 Channel 3,4 with AF1 mode
	tim2OC_ch_gpios.Pin = GPIO_PIN_2 | GPIO_PIN_10;
	tim2OC_ch_gpios.Mode = GPIO_MODE_AF_PP;
	tim2OC_ch_gpios.Pull = GPIO_NOPULL;
	tim2OC_ch_gpios.Speed = GPIO_SPEED_FREQ_LOW;
	tim2OC_ch_gpios.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOB, &tim2OC_ch_gpios);

	//3. Enable the IRQ of TIM2
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	//4. Configure the priority for the IRQ of TIM2
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
}
