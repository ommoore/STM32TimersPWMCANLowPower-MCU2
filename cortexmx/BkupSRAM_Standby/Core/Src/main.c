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
void Error_handler(void);

UART_HandleTypeDef huart2;

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

	uint32_t* pBackupSRAMbase = 0;
	char write_buf[] = "Hello";

	HAL_Init(); //Systick interrupt initialized here

	SystemClock_Config_HSE(SYS_CLOCK_FREQ_50MHZ);

	GPIO_Init();
	UART2_Init();

	//Turn on the clocks in RCC register for backup SRAM and power controller block
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	//Enable write access to the backup domain
	HAL_PWR_EnableBkUpAccess();

	pBackupSRAMbase = (uint32_t*)BKPSRAM_BASE;

	//Checking cause for Reset (either from pressing Reset button or due to Standby mode exit)
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB != RESET)) {

		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); //Clear Standby Flag
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); //Clear Wakeup Flag

		printmsg("Woke up from Standby Mode\r\n");
		uint8_t data = (uint8_t)*pBackupSRAMbase;
		if(data != 'H') {
			printmsg("Backup SRAM data is lost\r\n");
		} else {
			printmsg("Backup SRAM data is safe\r\n");
		}
	} else {
		for(uint32_t i = 0; i < (strlen(write_buf)+1); i++) {
			//Increment base address by + i as each byte is stored
			//Stores the characters "H e l l o" into the backup SRAM which persists after System Reset
			*(pBackupSRAMbase+i) = write_buf[i];
		}
	}

	//Wait for button (PC13) to be pushed
	printmsg("Press button to enter standby mode.\r\n");
	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) != GPIO_PIN_RESET);

	printmsg("Going to Standby mode\r\n");

	//Enable Wakeup pin 1 (PA0) in PWR_CSR to be used for wakeup from Standby Mode (Can touch PA0 to VDD with jumper)
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	//Enable Backup Voltage Regulator
	HAL_PWREx_EnableBkUpReg();

	//Enter Standby Mode
	HAL_PWR_EnterSTANDBYMode();

	while(1)
	{}

	return 0;
}

void SystemClock_Config_HSE(uint8_t clock_freq) {

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint8_t flash_latency = 0;

	//Using HSE to derive PLL
	//Activate LSE Crystal Oscillator on Nucleo Board
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

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef buttongpio;

	buttongpio.Pin = GPIO_PIN_13;
	buttongpio.Mode = GPIO_MODE_INPUT;
	buttongpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &buttongpio);
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

void Error_handler(void) {

	while(1);
}
