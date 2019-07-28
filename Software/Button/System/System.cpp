#include "System.h"
#include "stm32f0xx_hal.h"
#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>

#include "nRF905.h"
#include "nRF905_defs.h"

#include "stm32f0xx_hal.h"
#include "spi.h"
#include "adc.h"
#include "gpio.h"
#include "usart.h"

extern SPI_HandleTypeDef hspi1;
extern ADC_HandleTypeDef hadc;
//extern DMA_HandleTypeDef hdma_adc;
//extern UART_HandleTypeDef huart1;


const uint8_t maxretrys = 15;
const uint16_t maxwaittime = 80;
uint8_t package_no = 245;

namespace System {

	void ResumeClock()
	{
		HAL_ResumeTick();

		RCC_OscInitTypeDef RCC_OscInitStruct;
		RCC_ClkInitTypeDef RCC_ClkInitStruct;

		/**Initializes the CPU, AHB and APB busses clocks
		*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.HSICalibrationValue = 16;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
		RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
		if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		/**Initializes the CPU, AHB and APB busses clocks
		*/
		RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								  |RCC_CLOCKTYPE_PCLK1;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		/**Configure the Systick interrupt time
		*/
		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

		/**Configure the Systick
		*/
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		/* SysTick_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);


		HAL_Init();
		MX_GPIO_Init();
		MX_SPI1_Init();
		MX_ADC_Init();
		HAL_ADC_MspInit(&hadc);


	}


	void PowerDown()
	{
		HAL_SPI_DeInit(&hspi1);
		HAL_ADC_DeInit(&hadc);

		GPIO_InitTypeDef GPIO_InitStruct;


		GPIO_InitStruct.Pin = DR_Pin|GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DR_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = CSN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(CSN_GPIO_Port, &GPIO_InitStruct);

		HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(PWRUP_GPIO_Port, PWRUP_Pin, GPIO_PIN_RESET);

		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	}

	uint8_t nRF905sendtimestamp(uint32_t address, uint16_t from)
	{

		uint8_t retrys = 0;
		uint8_t send = 0;

		uint8_t data[NRF905_MAX_PAYLOAD] = {0};

		data[0] = 130;
		data[1] = from;
		data[2] = 2;

		data[5] = package_no;
		package_no++;

		System::print("Sending\n");

		uint32_t start = HAL_GetTick();

		while(retrys <maxretrys && !send)
		{
			uint32_t delay = HAL_GetTick() - start;
			data[3] = delay>>8;
			data[4] = delay & 0x00FF;

			data[6] = retrys;

			while(!nRF905_TX(address, data, NRF905_MAX_PAYLOAD, NRF905_NEXTMODE_RX));

			HAL_Delay(1);

			uint8_t waittime = 0;
			uint8_t gotdata = 0;
			while(!(gotdata = dataReady()) && waittime <maxwaittime)
			{
				HAL_Delay(1);
				waittime++;
			}

			System::print("wait %u\n", retrys);

			uint8_t buffer[NRF905_MAX_PAYLOAD];
			if(gotdata)
				nRF905_read(buffer, sizeof(buffer));

			while(waittime < maxwaittime )
			{
				HAL_Delay(1);
				waittime++;
			}

			uint8_t compare[NRF905_MAX_PAYLOAD] = { 0 };
			compare[0] = 65;
			compare[1] = 67;
			compare[2] = 75;

			uint8_t received = 1;
			for (uint8_t i=0; i< NRF905_MAX_PAYLOAD; i++)
				if( buffer[i] != compare[i]) received =0;
			if(received) send = 1;
			//System::print("retrys %u\n", retrys);
			retrys++;
		}

		return send;
	}

	uint8_t nRF905send(uint32_t address, uint16_t type, uint16_t payload, uint16_t from)
	{
		uint8_t retrys = 0;
		uint8_t send = 0;

		uint8_t data[NRF905_MAX_PAYLOAD] = {0};

		data[0] = 130;
		data[1] = from;
		data[2] = type;

		data[3] = payload>>8;
		data[4] = payload & 0x00FF;

		data[5] = package_no;
		package_no++;

		System::print("Sending\n");

		while(retrys <maxretrys && !send)
		{

			data[6] = retrys;
			while(!nRF905_TX(address, data, NRF905_MAX_PAYLOAD, NRF905_NEXTMODE_RX));

			HAL_Delay(1);

			uint8_t waittime = 0;
			uint8_t gotdata = 0;
			while(!(gotdata = dataReady()) && waittime <maxwaittime)
			{
				HAL_Delay(1);
				waittime++;
			}

			System::print("wait %u\n", retrys);

			uint8_t buffer[NRF905_MAX_PAYLOAD];
			if(gotdata)
				nRF905_read(buffer, sizeof(buffer));

			while(waittime <maxwaittime )
			{
				HAL_Delay(1);
				waittime++;
			}

			uint8_t compare[NRF905_MAX_PAYLOAD] = { 0 };
			compare[0] = 65;
			compare[1] = 67;
			compare[2] = 75;

			uint8_t received = 1;
			for (uint8_t i=0; i< NRF905_MAX_PAYLOAD; i++)
				if( buffer[i] != compare[i]) received =0;
			if(received) send = 1;
			//System::print("retrys %u\n", retrys);
			retrys++;
		}

		return send;
	}

	void print(const char *fmt, ...) {
	//#if DEBUG
	#ifndef NDEBUG
		char buffer[100];

		va_list arp;
		va_start(arp, fmt);
		uint16_t length = vsnprintf(buffer, sizeof(buffer), fmt, arp);
		va_end(arp);

		HAL_UART_Transmit(&huart1, (uint8_t*) buffer, length, 1000);
	#endif
	//#endif
	}
}
