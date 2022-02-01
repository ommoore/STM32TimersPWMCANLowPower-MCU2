/*
 * it.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"
extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim6;
extern void CAN1_Tx(uint8_t remote);

void SysTick_Handler(void) {

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void CAN1_TX_IRQHandler(void) {

	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void) {

	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX1_IRQHandler(void) {

	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_SCE_IRQHandler(void) {

	HAL_CAN_IRQHandler(&hcan1);
}

void TIM6_DAC_IRQHandler(void) {

	HAL_TIM_IRQHandler(&htim6);
}

void EXTI15_10_IRQHandler(void) {

	HAL_TIM_Base_Start_IT(&htim6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
