/**
 ******************************************************************************
 * @file 	configs.c
 * @author 	Can GULMEZ
 * @brief 	Peripheral configurations of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

/* Global and shared objects */

RCC_OscInitTypeDef iosc	= {0};	/* Oscillator */
RCC_ClkInitTypeDef iclk	= {0};	/* Clk */
GPIO_InitTypeDef igpio = {0};		/* Generic IO */
UART_HandleTypeDef huart4 = {0};	/* Debug Port */
UART_HandleTypeDef huart5 = {0};	/* LoRa Module Port */
UART_HandleTypeDef huart7 = {0};	/* GPS ModulePort */
SPI_HandleTypeDef	hspi1 = {0};	/* IMU Sensor Port */
SD_HandleTypeDef hsdmmc1 = {0};	/* SD Card Port */
DFSDM_Channel_HandleTypeDef hdfsdm1c[CHANNEL_COUNT] = {0};
DFSDM_Filter_HandleTypeDef hdfsdm1f[CHANNEL_COUNT] = {0};

/**
 * Configurate the oscillator and clock sources.
 */
void configOscClk(void)
{
	/* Initialize the system oscillator. */
	initOscillator(
		iosc, OSC_TYPE, OSC_HSE_STATE, OSC_PLL_STATE, OSC_PLL_SRC, 
		OSC_PLLM, OSC_PLLN, OSC_PLLP, OSC_PLLQ, OSC_PLLR, OSC_PLLRGE, 
		OSC_PLLVCOSEL, OSC_PLLFRACN
	);
	HAL_RCC_OscConfig(&iosc); 

	/* Initialize the system clock. */
	initClock(
		iclk, CLOCK_TYPE, CLOCK_SOURCE, CLOCK_SYS_DIV, CLOCK_AHB_DIV, 
		CLOCK_APB1_DIV, CLOCK_APB2_DIV, CLOCK_APB3_DIV, CLOCK_APB4_DIV
	);
	HAL_RCC_ClockConfig(&iclk, FLASH_LATENCY_3);

	/* Enable data and instruction caches. */
	SCB_EnableDCache();
	SCB_EnableICache();
}

/**
 * Configurate the debug port.
 */
void configDebugPort(void)
{
	/* Initialize the GPIOA peripheral. */
	initGPIO(
		igpio, DEBUG_PIN_TX | DEBUG_PIN_RX, DEBUG_GPIO_MODE, DEBUG_PULL, 
		DEBUG_ALTERNATE
	);
	HAL_GPIO_Init(DEBUG_PORT, &igpio);

	/* Initialize the UART4 peripheral. */
	initUART(
		huart4, DEBUG_INSTANCE, DEBUG_BAUDRATE, DEBUG_UART_MODE, 
		DEBUG_WORDLEN, DEBUG_STOPBITS, DEBUG_PARITY, DEBUG_HWCONTROL, 
		DEBUG_SAMPLING
	);
	HAL_UART_Init(&huart4);
}

/**
 * Configure the IMU sensor.
 */
void configIMUSensor(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOA peripheral. */
	initGPIO(
		igpio, IMU_PIN_NSS, IMU_GPIO_MODE_O, IMU_PULL, NULL
	);
	HAL_GPIO_Init(IMU_PORTA, &igpio);
	IMU_NSS_HIGH();

	initGPIO(
		igpio, IMU_PIN_INT2, IMU_GPIO_MODE_I, IMU_PULL, NULL
	);
	HAL_GPIO_Init(IMU_PORTA, &igpio);

	initGPIO(
		igpio, IMU_PIN_SCK | IMU_PIN_MISO | IMU_PIN_MOSI, 
		IMU_GPIO_MODE_AF, IMU_PULL, IMU_ALTERNATE
	);
	HAL_GPIO_Init(IMU_PORTA, &igpio);

	/* Initialize the GPIOB peripheral. */
	initGPIO(
		igpio, IMU_PIN_INT1, IMU_GPIO_MODE_I, IMU_PULL, NULL
	);
	HAL_GPIO_Init(IMU_PORTB, &igpio);

	/* Initialize the SPI1 peripheral. */
	initSPI(
		hspi1, IMU_INSTANCE, IMU_SPI_MODE, IMU_DIRECTION, IMU_DATASIZE, 
		IMU_POLARITY, IMU_PHASE, IMU_NSS, IMU_PRESCALER, IMU_FIRSTBIT, 
		IMU_TIMODE
	);
	status = HAL_SPI_Init(&hspi1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SPI1 peripheral!");
}

/**
 * Configure the microphone sensors.
 */
void configMicSensors(void)
{
	int i;
	HAL_StatusTypeDef status;

	/* Initialize the GPIOB peripheral. */
	initGPIO(
		igpio, MIC_PIN_DATAIN1 | MIC_PIN_CKOUT, MIC_GPIO_MODE, MIC_PULL, 
		MIC_ALTERNATE
	);
	HAL_GPIO_Init(MIC_PORTB, &igpio);

	/* Initialize the GPIOC peripheral. */
	initGPIO(
		igpio, MIC_PIN_DATAIN0 | MIC_PIN_DATAIN2 | MIC_PIN_DATAIN3, 
		MIC_GPIO_MODE, MIC_PULL, MIC_ALTERNATE
	);
	HAL_GPIO_Init(MIC_PORTC, &igpio);

	/* Initialize the DFSDM1 peripheral. */
	for (i = 0; i < CHANNEL_COUNT; i++)
	{
		/* Initialize the channel. */
		initDFSDMChannel(hdfsdm1c, i, MIC_DIVIDER, MIC_FILTER_TYPE, 
			MIC_OFFSET, MIC_RIGHT_SHIFT);
		
		status = HAL_DFSDM_ChannelInit(&hdfsdm1c[i]);
		if (status != HAL_OK)
			printError(status, "Failed to initialize DFSDM channel!");

		/* Initialize the filter. */
		initDFSDMFilter(hdfsdm1f, i, MIC_SINC_ORDER, MIC_OVERSAMPLING);

		status = HAL_DFSDM_FilterInit(&hdfsdm1f[i]);
		if (status != HAL_OK)
			printError(status, "Failed to initialize DFSDM filter!");

		/* Assign the the filter to the channel. */
		status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f[i], i, 
			DFSDM_CONTINUOUS_CONV_ON);
		if (status != HAL_OK)
			printError(status, "Failed to assign the filter to channel!");
	}
}

/**
 * Configure the GPS module.
 */
void configGPSModule(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOE peripheral. */
	initGPIO(
		igpio, GPS_PIN_RX | GPS_PIN_TX, GPS_GPIO_MODE, GPS_PULL, 
		GPS_ALTERNATE
	);
	HAL_GPIO_Init(GPS_PORT, &igpio);
	
	/* Initialize the UART7 peripheral. */
	initUART(
		huart7, GPS_INSTANCE, GPS_BAUDRATE, GPS_UART_MODE, GPS_WORDLEN, 
		GPS_STOPBITS, GPS_PARITY, GPS_HWCONTROL, GPS_SAMPLING
	);
	status = HAL_UART_Init(&huart7);
	if (status != HAL_OK)
		printError(status, "Failied to initialize UART7 peripheral!");
}

/**
 * Configure the SD card.
 */
void configSDCard(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOA peripheral. */
	initGPIO(
		igpio, SD_PIN_CD, SD_GPIO_MODE_O, SD_PULL, NULL
	);
	HAL_GPIO_Init(SD_PORTA, &igpio);

	/* Initialize the GPIOC peripheral. */
	initGPIO(
		igpio, SD_PIN_DAT0 | SD_PIN_DAT1 | SD_PIN_DAT2 | SD_PIN_DAT3 | 
		SD_PIN_CLK, SD_GPIO_MODE_AF, SD_PULL, SD_ALTERNATE
	);
	HAL_GPIO_Init(SD_PORTC, &igpio);

	/* Initialize the GPIOD peripheral. */
	initGPIO(
		igpio, SD_PIN_CMD, SD_GPIO_MODE_AF, SD_PULL, SD_ALTERNATE
	);
	HAL_GPIO_Init(SD_PORTD, &igpio);

	/* Initialize the SDMMC1 peripheral. */
	initSDMMC(
		hsdmmc1, SD_INSTANCE, SD_BUS_WIDE, SD_CLOCK_EDGE, SD_POWER_SAVE, 
		SD_CLOCKDIV, SD_HWCONTROL
	);
	status = HAL_SD_Init(&hsdmmc1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SDMMC1 peripheral!");
}

/**
 * Configure the LoRa module.
 */
void configLoRaModule(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOD peripheral. */
	initGPIO(
		igpio, LORA_PIN_M0 | LORA_PIN_M1, LORA_GPIO_MODE_O, LORA_PULL, 
		NULL
	);
	HAL_GPIO_Init(LORA_PORTD, &igpio);

	initGPIO(
		igpio, LORA_PIN_AUX, LORA_GPIO_MODE_I, LORA_PULL, NULL
	);
	HAL_GPIO_Init(LORA_PORTD, &igpio);

	/* Initialize the GPIOB peripheral. */
	initGPIO(
		igpio, LORA_PIN_TX | LORA_PIN_RX, LORA_GPIO_MODE_AF, LORA_PULL, 
		LORA_ALTERNATE
	);
	HAL_GPIO_Init(LORA_PORTB, &igpio);

	/* Initialize the UART5 peripheral. */
	initUART(
		huart5, LORA_INSTANCE, LORA_BAUDRATE, LORA_MODE, LORA_WORDLEN, 
		LORA_STOPBITS, LORA_PARITY, LORA_HWCONTROL, LORA_SAMPLING
	);
	status = HAL_UART_Init(&huart5);
	if (status != HAL_OK)
		printError(status, "Failed to initialize UART5 peripheral!");
}

/**
 * Configure the LEDs.
 */
void configLEDs(void)
{
	/* Initialize the GPIOE peripheral. */
	initGPIO(
		igpio, LED_PIN_HEARTBEAT | LED_PIN_ERROR | LED_PIN_DETECT, 
		LED_MODE, LED_PULL, NULL
	);
	HAL_GPIO_Init(LED_PORT, &igpio);
}

/**
 * Configure the servo motors.
 */
void configServoMotors(void)
{

}

/**
 * Configure the watch dog timer.
 */
void configWatchdog(void)
{

}
 
