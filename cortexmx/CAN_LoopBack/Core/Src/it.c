/*
 * it.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"

void SysTick_Handler(void) {

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
