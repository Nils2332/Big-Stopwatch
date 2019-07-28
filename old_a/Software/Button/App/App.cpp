#include "App.h"
#include "stdio.h"
#include "array"

#include "stm32f0xx_hal.h"
#include "System.h"
#include "spi.h"
#include "adc.h"
#include "gpio.h"
//#include "usart.h"

#include "nRF905.h"
#include "nRF905_defs.h"
#include "nRF905_debug.h"

//extern SPI_HandleTypeDef hspi1;
//extern UART_HandleTypeDef huart1;

//extern ADC_HandleTypeDef hadc;
//extern DMA_HandleTypeDef hdma_adc;

//#define RXADDR 0xFE4CA6E5 // Address of this device
#define mask 0xABBAFFE0
#define TXADDR 0xABBAFFE0 // Address of device to send to

#define nobuttons 4


uint16_t device = 1;

uint32_t RXADDR = mask+device;

uint32_t deviceID = RXADDR + device;

uint8_t buttons[nobuttons]={1,1,1,1};

uint8_t buttons2 = 0xF;

void get_buttons()
{
	buttons[0] = HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin);
	buttons[1] = HAL_GPIO_ReadPin(Button2_GPIO_Port, Button2_Pin);
	//buttons[2] = HAL_GPIO_ReadPin(Button3_GPIO_Port, Button3_Pin);
	//buttons[3] = HAL_GPIO_ReadPin(Button4_GPIO_Port, Button4_Pin);

	buttons2 = buttons[0] | buttons[1] <<1 | buttons[2] << 2 | buttons[3] <<3;
	//System::print("%i" ,buttons2);
}


void App_Start()
{
	//nRF905_testclient();

	HAL_Delay(100);

	nRF905_init();

	// Set address of this device
	nRF905_setListenAddress(RXADDR);

	// Put into receive mode
	nRF905_RX();

	HAL_Delay(10);

	System::print("Button %u\n", device);

	uint8_t regs[NRF905_REGISTER_COUNT];
	nRF905_getConfigRegisters(regs);

	uint16_t channel = ((uint16_t)(regs[1] & 0x01)<<8) | regs[0];
	uint32_t freq = (422400UL + (channel * 100UL)) * (1 + ((regs[1] & ~NRF905_MASK_BAND) >> 1));

	System::print("Channel: %u", channel);
	System::print("Freq: %u", freq);
	System::print("KHz");
	System::print("Auto retransmit: %u\n", !!(regs[1] & ~NRF905_MASK_AUTO_RETRAN));
	System::print("Low power RX: %u\n", !!(regs[1] & ~NRF905_MASK_LOW_RX));

	System::print("");

	HAL_Delay(100);

	uint32_t bat= 0;

	uint32_t sum=0;

	for(uint8_t i=0; i<3;i++)
	{
		HAL_ADC_Start(&hadc);
		while(HAL_ADC_PollForConversion(&hadc,0) != HAL_OK);
		HAL_ADC_GetState(&hadc)	;
		HAL_ADC_GetValue(&hadc);
		HAL_ADC_Stop(&hadc);
	}

	for(uint8_t i=0; i<10;i++)
	{
		HAL_ADC_Start(&hadc);
		while(HAL_ADC_PollForConversion(&hadc,0) != HAL_OK);
		HAL_ADC_GetState(&hadc)	;
		bat = HAL_ADC_GetValue(&hadc);
		bat = 4857037 / bat;
		sum = sum + bat;
		HAL_ADC_Stop(&hadc);
	}

	sum = sum/10;
	//char voltage[NRF905_MAX_PAYLOAD] = {0};
	//sprintf(voltage, "Bat: %lu V", bat);

	uint16_t voltage;
	voltage = sum;

	System::print("Voltage %i", bat);

	System::nRF905send(TXADDR, 1, voltage, device);


	while(1)
	{
		//HAL_Delay(50);
		get_buttons();

		nRF905_powerUp();

		HAL_Delay(10);

		//HAL_Delay(150);
		//uint8_t data[28] = {0};

		if(buttons2)		//Button 1
		{

			if(buttons2 == 14)
			{

				System::nRF905sendtimestamp(TXADDR, device);

				System::print("Send\n");

			}
			while(buttons2 == 14)
				get_buttons();

		}


		nRF905_powerDown();
		HAL_Delay(1);

		for(uint8_t i=0; i<nobuttons; i++)
			buttons[i] = 1, buttons2 = 15;

		System::PowerDown();

		System::ResumeClock();
	}
}


