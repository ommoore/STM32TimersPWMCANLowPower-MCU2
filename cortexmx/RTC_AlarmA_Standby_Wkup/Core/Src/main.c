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
void RTC_Init(void);
void RTC_CalendarConfig(void);
void RTC_AlarmConfig(void);
char* getAMPM(uint8_t number);
char* getDayOfWeek(uint8_t number);

UART_HandleTypeDef huart2;
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

	RTC_TimeTypeDef  RTC_TimeRead;
	RTC_DateTypeDef RTC_DateRead;

	HAL_Init(); //Systick interrupt initialized here

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50MHZ);

	GPIO_Init();
	UART2_Init();
	RTC_Init();

	__HAL_RCC_PWR_CLK_ENABLE();

	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		__HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

		printmsg("Woke up from standby mode\r\n");

		HAL_RTC_GetTime(&hrtc, &RTC_TimeRead, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &RTC_DateRead, RTC_FORMAT_BIN);

		printmsg("Current Time is: %02d:%02d:%02d%s\r\n", RTC_TimeRead.Hours, RTC_TimeRead.Minutes, RTC_TimeRead.Seconds, getAMPM(RTC_TimeRead.TimeFormat));
		printmsg("Current Date is: <%s> %02d-%2d-%2d\r\n", getDayOfWeek(RTC_DateRead.WeekDay), RTC_DateRead.Month, RTC_DateRead.Date, RTC_DateRead.Year);

		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
		HAL_Delay(2000);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);

	}
	printmsg("RTC Alarm Test\r\n");

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

	ledgpio.Pin = GPIO_PIN_5 | GPIO_PIN_8;
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

void RTC_Init(void) {

	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 0x7F; 	//127   (+1 = 128 because division of 1+prescaler)
	hrtc.Init.SynchPrediv = 0xFF; 	//255   (+1 = 256 because division of 1+prescaler)
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW; //doesn't matter if output disabled
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;   //doesn't matter if output disabled

	if(HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_handler();
	}
}

void RTC_CalendarConfig(void) {

	RTC_TimeTypeDef RTC_TimeInit;
	RTC_DateTypeDef RTC_DateInit;

	// Configuring the Calendar for Time: 23:15:15 (24h)  Date: 29 January 2022 Saturday
	// (Sending binary values which will get converted to BCD)
	RTC_TimeInit.Hours = 23;
	RTC_TimeInit.Minutes = 15;
	RTC_TimeInit.Seconds = 15;
	//RTC_TimeInit.TimeFormat = RTC_HOURFORMAT12_PM;
	if(HAL_RTC_SetTime(&hrtc, &RTC_TimeInit, RTC_FORMAT_BIN) != HAL_OK) {
		Error_handler();
	}

	RTC_DateInit.Date = 29;
	RTC_DateInit.Month = RTC_MONTH_JANUARY;
	RTC_DateInit.Year = 22;  //2000 + year (22) = 2022
	RTC_DateInit.WeekDay = RTC_WEEKDAY_SATURDAY;
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	//print RTC time and date details
	RTC_TimeTypeDef RTC_TimeRead;
	RTC_DateTypeDef RTC_DateRead;

	RTC_CalendarConfig();

	HAL_RTC_GetTime(&hrtc, &RTC_TimeRead, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTC_DateRead, RTC_FORMAT_BIN);

	printmsg("Current Time is: %02d:%02d:%02d%s\r\n", RTC_TimeRead.Hours, RTC_TimeRead.Minutes, RTC_TimeRead.Seconds, getAMPM(RTC_TimeRead.TimeFormat));
	printmsg("Current Date is: <%s> %02d-%2d-%2d\r\n", getDayOfWeek(RTC_DateRead.WeekDay), RTC_DateRead.Month, RTC_DateRead.Date, RTC_DateRead.Year);

	//Make sure that WUF and RTC Alarm A flags are cleared
	__HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	RTC_AlarmConfig();

	printmsg("Went to STANDBY mode\r\n");
	HAL_PWR_EnterSTANDBYMode();
}

void RTC_AlarmConfig(void) {

	RTC_AlarmTypeDef AlarmA_Set;
	memset(&AlarmA_Set, 0, sizeof(AlarmA_Set));

	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

	//8:00:00 AM
	AlarmA_Set.Alarm = RTC_ALARM_A;
	AlarmA_Set.AlarmTime.Hours = 23;
	AlarmA_Set.AlarmTime.Minutes = 15;
	AlarmA_Set.AlarmTime.Seconds = 30;

	//Masking out the "don't-care" fields
	AlarmA_Set.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	AlarmA_Set.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;

	if(HAL_RTC_SetAlarm_IT(&hrtc, &AlarmA_Set, RTC_FORMAT_BIN) != HAL_OK) {
		Error_handler();
	}
	printmsg("Alarm Set Successful\r\n");
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {

	printmsg("Alarm Triggered\r\n");
}

void Error_handler(void) {

	while(1);
}
