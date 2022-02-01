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

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {

	//1. Enable TIM6 peripheral clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	//2. Enable the IRQ of TIM6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//3. Configure the priority for the IRQ of TIM6
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
}
