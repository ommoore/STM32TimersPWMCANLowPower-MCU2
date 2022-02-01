/*
 * main.c
 *
 *  Created on: Jan 7, 2022
 *      Author: Oliver
 */

#include "main.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void SystemClock_Config_HSE(uint8_t clock_freq);
void GPIO_Init(void);
void UART2_Init(void);
void TIM6_Init(void);
void RTC_Init(void);
void RTC_CalendarConfig(void);
void RTC_AlarmConfig(void);
char* getAMPM(uint8_t number);
char* getDayOfWeek(uint8_t number);

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim6;
RTC_HandleTypeDef hrtc;

void printmsg(char *format, ...) {

	char str[80];

	/* Extract the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
}

int main(void) {

	HAL_Init(); //Systick interrupt initialized here

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_84MHZ);

	GPIO_Init();
	UART2_Init();
	RTC_Init();

	RTC_CalendarConfig();

	while(1)
	{}

	return 0;
}

void SystemClock_Config_HSE(uint8_t clock_freq) {

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t flash_latency = 0;

	//Using HSE to derive PLL
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;
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
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef ledgpio, buttongpio;

	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);

	buttongpio.Pin = GPIO_PIN_13;
	buttongpio.Mode = GPIO_MODE_IT_FALLING;
	buttongpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &buttongpio);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void UART2_Init(void) {

	huart2.Instance = USART2; //linking
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX;

	if(HAL_UART_Init(&huart2) != HAL_OK) {

		//There is a problem
		Error_handler();
	}
}

void TIM6_Init(void) {
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 4999;
	htim6.Init.Period = 10000-1;
	if(HAL_TIM_Base_Init(&htim6) != HAL_OK) {
		Error_handler();
	}
}

void RTC_Init(void) {

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = RTC_ASYNC_PREDIV; 	//127   (+1 = 128 because division of 1+prescaler)
	hrtc.Init.SynchPrediv = RTC_SYNC_PREDIV; 	//255   (+1 = 256 because division of 1+prescaler)
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; //doesn't matter if output disabled
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;   //doesn't matter if output disabled
	__HAL_RTC_RESET_HANDLE_STATE(&hrtc);
	if(HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_handler();
	}
}

void RTC_CalendarConfig(void) {

	RTC_TimeTypeDef RTC_TimeInit;
	RTC_DateTypeDef RTC_DateInit;

	// Configuring the Calendar for Time: 3:30:15 PM  Date: 31 January 2022 Monday
	// (Sending binary values which will get converted to BCD)
	RTC_TimeInit.Hours = 3;
	RTC_TimeInit.Minutes = 30;
	RTC_TimeInit.Seconds = 15;
	RTC_TimeInit.TimeFormat = RTC_HOURFORMAT12_PM;
	RTC_TimeInit.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeInit.StoreOperation = RTC_STOREOPERATION_RESET;
	if(HAL_RTC_SetTime(&hrtc, &RTC_TimeInit, RTC_FORMAT_BIN) != HAL_OK) {
		Error_handler();
	}

	RTC_DateInit.Date = 31;
	RTC_DateInit.Month = RTC_MONTH_JANUARY;
	RTC_DateInit.Year = 22;  //2000 + year (22) = 2022
	RTC_DateInit.WeekDay = RTC_WEEKDAY_MONDAY;
	if(HAL_RTC_SetDate(&hrtc, &RTC_DateInit, RTC_FORMAT_BIN) != HAL_OK) {
		Error_handler();
	}
}

char* getAMPM(uint8_t number) {

	char *AMPM[] = { "AM", "PM", "ER"};

	if(number == RTC_HOURFORMAT12_AM)
		//AM
		return AMPM[0];
	else if(number == RTC_HOURFORMAT12_PM)
		//PM
		return AMPM[1];
	else
		//Error
		return AMPM[2];
}

char* getDayOfWeek(uint8_t number) {

	char *weekday[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

	return weekday[number-1];
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	char showtime[50];

	RTC_TimeTypeDef RTC_TimeRead;
	RTC_DateTypeDef RTC_DateRead;
	memset(&RTC_TimeRead, 0, sizeof(RTC_TimeRead));
	memset(&RTC_DateRead, 0, sizeof(RTC_DateRead));

	HAL_RTC_GetTime(&hrtc, &RTC_TimeRead, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTC_DateRead, RTC_FORMAT_BIN);

	sprintf((char*)showtime, "%02d:%02d:%02d  ", RTC_TimeRead.Hours, RTC_TimeRead.Minutes, RTC_TimeRead.Seconds);
	HAL_UART_Transmit(&huart2, (uint8_t*)showtime, strlen(showtime), HAL_MAX_DELAY);

	memset(showtime,0,sizeof(showtime));

	sprintf((char*)showtime, "%02d-%2d-%2d\r\n", RTC_DateRead.Month, RTC_DateRead.Date, 2000 + RTC_DateRead.Year);
	HAL_UART_Transmit(&huart2, (uint8_t*)showtime, strlen(showtime), HAL_MAX_DELAY);
}

void Error_handler(void) {

	while(1);
}
