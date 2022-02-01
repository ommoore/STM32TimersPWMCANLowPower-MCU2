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

int main(void) {

	HAL_Init();
	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50MHZ);
	GPIO_Init();
	UART2_Init();
	TIM2_Init();

	//25% duty cycle
	if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}
	//45% duty cycle
	if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}
	//75% duty cycle
	if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}
	//90% duty cycle
	if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4) != HAL_OK) {
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
	TIM_OC_InitTypeDef tim2PWM_Config;
	memset(&tim2PWM_Config, 0, sizeof(tim2PWM_Config));

	htim2.Instance = TIM2;
	htim2.Init.Period = 10000-1; 				//32-bit timer (TIM2)
	htim2.Init.Prescaler = 4999;  				//SYSCLK = 50MHZ, so prescaler of 4999 makes the timer CNT_CLK 10kHz
	if(HAL_TIM_PWM_Init(&htim2) != HAL_OK) { 	//Initializes timer PWM time base with period 1 second.
		Error_handler();
	}

	//Channel 1 - PWM 25% duty cycle
	tim2PWM_Config.OCMode = TIM_OCMODE_PWM1;
	tim2PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH; //CC1P bit field of TIM2_CCER register. signal path: oc1ref -> (here) mux selection -> OE Circuit -> OC1
													 //polarity high -> ON (25%) then OFF (75%).   polarity low -> OFF (25%) then ON (75%)
	tim2PWM_Config.Pulse = ((htim2.Init.Period * 25) / 100);
	if(HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_1) != HAL_OK) {
		Error_handler();
	}

	//Channel 2 - PWM 45% duty cycle
	tim2PWM_Config.Pulse = ((htim2.Init.Period * 45) / 100);
	if(HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_2) != HAL_OK) {
		Error_handler();
	}

	//Channel 3 - PWM 75% duty cycle
	tim2PWM_Config.Pulse = ((htim2.Init.Period * 75) / 100);
	if(HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_3) != HAL_OK) {
		Error_handler();
	}

	//Channel 4 - PWM 90% duty cycle
	tim2PWM_Config.Pulse = ((htim2.Init.Period * 90) / 100);
	if(HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_4) != HAL_OK) {
		Error_handler();
	}
}

void Error_handler(void) {
	while(1);
}
