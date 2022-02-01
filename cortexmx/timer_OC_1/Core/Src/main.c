/*
 * main.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"
#include <stdio.h>
#include <string.h>

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void UART2_Init(void);
void TIM2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;

uint32_t pulse1_value = 25000; 	//to produce 500Hz
uint32_t pulse2_value = 12500;	//to produce 1kHz
uint32_t pulse3_value = 6250;	//to produce 2kHz
uint32_t pulse4_value = 3125;	//to produce 4kHz

uint32_t ccr_content;


int main(void) {

	HAL_Init();
	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50MHZ);
	GPIO_Init();
	UART2_Init();
	TIM2_Init();

	if(HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}
	if(HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}
	if(HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}
	if(HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}

	while(1);

	return 0;
}

void SystemClock_Config_HSE(uint8_t clock_freq) {

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t flash_latency = 0;

	//Using HSE to derive PLL
	//Activate LSE Crystal Oscillator on Nucleo Board
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.LSEState = RCC_LSE_ON;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	switch(clock_freq) {
		case SYS_CLOCK_FREQ_50MHZ: {
			osc_init.PLL.PLLM = 16;
			osc_init.PLL.PLLN = 100;
			osc_init.PLL.PLLP = RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;
			osc_init.PLL.PLLR = 2;

			clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
					 	 	 	  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2);
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			flash_latency = 1;
			break;
		}
		case SYS_CLOCK_FREQ_84MHZ: {
			osc_init.PLL.PLLM = 16;
			osc_init.PLL.PLLN = 168;
			osc_init.PLL.PLLP = RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;
			osc_init.PLL.PLLR = 2;

			clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
					 	 	 	  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2);
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			flash_latency = 2;
			break;
		}
		case SYS_CLOCK_FREQ_120MHZ: {
			osc_init.PLL.PLLM = 16;
			osc_init.PLL.PLLN = 240;
			osc_init.PLL.PLLP = RCC_PLLP_DIV2;
			osc_init.PLL.PLLQ = 2;
			osc_init.PLL.PLLR = 2;

			clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
					 	 	 	  RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2);
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			flash_latency = 3;
			break;
		}
		default:
			return;
	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_handler();
	}

	if(HAL_RCC_ClockConfig(&clk_init, flash_latency) != HAL_OK) {
		Error_handler();
	}

	//Configure the SYSTICK timer interrupt frequency for every 1ms
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	//Configure SYSTICK
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	//SYSTICK IRQn interrupt configuration
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

void GPIO_Init(void) {

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}

void UART2_Init(void) {

	huart2.Instance = USART2; //linking
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;

	if(HAL_UART_Init(&huart2) != HAL_OK) {

		//There is a problem
		Error_handler();
	}
}

void TIM2_Init(void) {
	TIM_OC_InitTypeDef tim2OC_init;

	htim2.Instance = TIM2;
	htim2.Init.Period = 0xFFFFFFFF; 		//32-bit timer (TIM2)
	htim2.Init.Prescaler = 1;  				//SYSCLK = 50MHZ, so prescaler of 1 (divides by 2) makes the timer CNT_CLK 25MHz
	if(HAL_TIM_OC_Init(&htim2) != HAL_OK) { //Initializes timer output compare time base
		Error_handler();
	}

	//Channel 1 - 500Hz output
	tim2OC_init.OCMode = TIM_OCMODE_TOGGLE;
	tim2OC_init.OCPolarity = TIM_OCPOLARITY_HIGH; //CC1P bit field of TIM2_CCER register. signal path: oc1ref -> (here) mux selection -> OE Circuit -> OC1
	tim2OC_init.Pulse = pulse1_value;
	if(HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_init, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}

	//Channel 2 - 1kHz output
	tim2OC_init.Pulse = pulse2_value;
	if(HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_init, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}

	//Channel 3 - 2kHz output
	tim2OC_init.Pulse = pulse3_value;
	if(HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_init, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}

	//Channel 4 - 4kHz output
	tim2OC_init.Pulse = pulse4_value;
	if(HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_init, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	/* TIM2_CH1 toggling with f = 500Hz */
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr_content + pulse1_value);
	}

	/* TIM2_CH2 toggling with f = 1kHz */
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr_content + pulse2_value);
	}

	/* TIM2_CH3 toggling with f = 2kHz */
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, ccr_content + pulse3_value);
	}

	/* TIM2_CH4 toggling with f = 4kHz */
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, ccr_content + pulse4_value);
	}
}

void Error_handler(void) {
	while(1);
}
