/**
 ******************************************************************************
 * @file 	configs.c
 * @author 	Can Gulmez
 * @brief 	Peripheral configurations of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can Gulmez.
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

UART_HandleTypeDef huart7 = {0};		/* GPS Module */
DMA_HandleTypeDef huart7dma = {0};	/* GPS Module */

SPI_HandleTypeDef	hspi1 = {0};		/* IMU Sensor */
DMA_HandleTypeDef hspi1dma = {0};	/* IMU Sensor */

SD_HandleTypeDef hsdmmc1 = {0};		/* SD Card */
DMA_HandleTypeDef hsdmmc1dma = {0};	/* SD Card */

DFSDM_Channel_HandleTypeDef hdfsdm1c[CHANNEL_COUNT] = {0};	/* Mic Sensor */
DFSDM_Filter_HandleTypeDef hdfsdm1f[CHANNEL_COUNT] = {0};	/* Mic Sensor */
DMA_HandleTypeDef hdfsdm1dma[CHANNEL_COUNT] = {0};				/* Mic Sensor */

/**
 * Configurate the oscillator and clock sources.
 */
void configOscClk(void)
{
	/**
	 * PLL1_M: 5									(25MHz / 5 = 5MHz)
	 * PLL1_N: 80									(5MHz × 80 = 400MHz)
	 * PLL1_P: 4									(400MHz / 4 = 100MHz) -> SYSCLK
	 * PLL1_Q: 8									(400MHz / 8 = 50MHz)  -> For SDMMC, USB, RNG
	 * PLL1_R: 8									(400MHz / 8 = 50MHz)  -> For SPI, I2S
	 * 
	 * AHB Prescaler: 1							HCLK = 100MHz
	 * APB1 Prescaler: 2							PCLK1 = 50MHz
	 * APB2 Prescaler: 2							PCLK2 = 50MHz
	 * APB3 Prescaler: 2							PCLK3 = 50MHz
	 * APB4 Prescaler: 2							PCLK4 = 50MHz
	 */

	/* Initialize the system oscillator. */
	initOscillator(iosc, RCC_OSCILLATORTYPE_HSE, RCC_HSE_ON, RCC_PLL_ON, 
		RCC_PLLSOURCE_HSE, 5, 80, 4, 8, 8, RCC_PLL1VCIRANGE_2, RCC_PLL1VCOWIDE, 0);
	HAL_RCC_OscConfig(&iosc); 

	/* Initialize the system clock. */
	initClock(iclk, RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | 
		RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1, 
		RCC_SYSCLKSOURCE_PLLCLK, RCC_SYSCLK_DIV1, RCC_HCLK_DIV1, RCC_APB1_DIV2, 
		RCC_APB2_DIV2, RCC_APB3_DIV2, RCC_APB4_DIV2);
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
	initGPIO(igpio, DEBUG_PIN_TX | DEBUG_PIN_RX, GPIO_MODE_AF_PP, GPIO_NOPULL, 
		GPIO_AF6_UART4);
	HAL_GPIO_Init(GPIOA, &igpio);

	/* Initialize the UART4 peripheral. */
	initUART(huart4, UART4, 115200, UART_MODE_TX, UART_WORDLENGTH_8B, 
		UART_STOPBITS_1, UART_PARITY_NONE, UART_HWCONTROL_NONE, UART_OVERSAMPLING_16);
	HAL_UART_Init(&huart4);
}

/**
 * Configure the microphone sensors.
 */
void configMicSensors(void)
{
	int i;
	HAL_StatusTypeDef status;

	/**
 	 * System Clock:								100 MHz
 	 * DFSDM Clock:								100 MHz / DIVIDER = 3.125 MHz
 	 * Filter Output Rate:						3.125 MHz / OVERSAMPLING = 97.656 kHz
 	 * Actual Audio Rate:						48.828 kHz / (ORDER + 1) = 24.414 kHz
 	 */

	/* Initialize the GPIOB peripheral. */
	initGPIO(igpio, MIC_PIN_DATAIN1 | MIC_PIN_CKOUT, GPIO_MODE_AF_PP, 
		GPIO_NOPULL, GPIO_AF3_DFSDM1);
	HAL_GPIO_Init(GPIOB, &igpio);

	/* Initialize the GPIOC peripheral. */
	initGPIO(igpio, MIC_PIN_DATAIN0 | MIC_PIN_DATAIN2 | MIC_PIN_DATAIN3, 
		GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF3_DFSDM1);
	HAL_GPIO_Init(GPIOC, &igpio);

	/* Initialize the DFSDM1 peripheral. */
	for (i = 0; i < CHANNEL_COUNT; i++)
	{
		/* Initialize the channel. */
		initDFSDMChannel(hdfsdm1c, i, 32, DFSDM_CHANNEL_FASTSINC_ORDER, 
			0, 0);
		status = HAL_DFSDM_ChannelInit(&hdfsdm1c[i]);
		if (status != HAL_OK)
			printError(status, "Failed to initialize DFSDM channel!");

		/* Initialize the filter. */
		initDFSDMFilter(hdfsdm1f, i, DFSDM_FILTER_SINC3_ORDER, 32);
		status = HAL_DFSDM_FilterInit(&hdfsdm1f[i]);
		if (status != HAL_OK)
			printError(status, "Failed to initialize DFSDM filter!");

		/* Assign the the channel to the filter. */
		status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f[i], i, 
			DFSDM_CONTINUOUS_CONV_ON);
		if (status != HAL_OK)
			printError(status, "Failed to assign the filter to channel!");
		
		/* Initialize the DMA1_Stream0-3 for DFSDM1. */
		initDFSDMDMA(hdfsdm1dma, i, DMA1_Stream0 + i, DMA_REQUEST_DFSDM1_FLT0 + i, 
			DMA_PERIPH_TO_MEMORY, DMA_PINC_DISABLE, DMA_MINC_ENABLE, 
			DMA_PDATAALIGN_WORD, DMA_MDATAALIGN_WORD, DMA_CIRCULAR, DMA_PRIORITY_HIGH, 
			DMA_FIFOMODE_DISABLE);
		status = HAL_DMA_Init(&hdfsdm1dma[i]);
		if (status != HAL_OK)
			printError(status, "Failed to initialize the DMA for DFSDM!");
	
		/* Link DMA1_Stream0-3 to DFSDM1 filter. */
		__HAL_LINKDMA(&hdfsdm1f[i], hdmaReg, hdfsdm1dma[i]);
		
		/* Configure the DMA1_Stream0-3 interrupts. */
		HAL_NVIC_SetPriority(DMA1_Stream0_IRQn + i, 1, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn + i);
	}
}

/**
 * Configure the IMU sensor.
 */
void configIMUSensor(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOA peripheral. */
	initGPIO(igpio, IMU_PIN_NSS, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);
	IMU_NSS_HIGH();

	initGPIO(igpio, IMU_PIN_INT2, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);

	initGPIO(igpio, IMU_PIN_SCK | IMU_PIN_MISO | IMU_PIN_MOSI, GPIO_MODE_AF_PP,
		GPIO_NOPULL, GPIO_AF5_SPI1);
	HAL_GPIO_Init(GPIOA, &igpio);

	/* Initialize the GPIOB peripheral. */
	initGPIO(igpio, IMU_PIN_INT1, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOB, &igpio);

	/* Initialize the SPI1 peripheral. */
	initSPI(hspi1, SPI1, SPI_MODE_MASTER, SPI_DIRECTION_2LINES, SPI_DATASIZE_8BIT, 
		SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_NSS_SOFT, SPI_BAUDRATEPRESCALER_8, 
		SPI_FIRSTBIT_MSB, SPI_TIMODE_DISABLE);
	status = HAL_SPI_Init(&hspi1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SPI1 peripheral!");

	/* Initialize the DMA1_Stream4 for SPI. */
	// initDMA(hspi1dma, DMA1_Stream4, DMA_REQUEST_SPI1_RX, DMA_PERIPH_TO_MEMORY,
	// 	DMA_PINC_DISABLE, DMA_MINC_ENABLE, DMA_PDATAALIGN_WORD, DMA_MDATAALIGN_WORD,
	// 	DMA_CIRCULAR, DMA_PRIORITY_HIGH, DMA_FIFOMODE_DISABLE);
	// status = HAL_DMA_Init(&hspi1dma);
	// if (status != HAL_OK)
	// 	printError(status, "Failed to initialize DMA for SPI1!");

	/* Link the DMA1_Stream4 to SPI1 peripheral. */
	// __HAL_LINKDMA(&hspi1, hdmarx, hspi1dma);

	/* Configure the DMA1_Stream4 interrupts. */
	// HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	// HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 2, 0);
}

/**
 * Configure the GPS module.
 */
void configGPSModule(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOE peripheral. */
	initGPIO(igpio, GPS_PIN_RX | GPS_PIN_TX, GPIO_MODE_AF_PP, GPIO_NOPULL, 
		GPIO_AF7_UART7);
	HAL_GPIO_Init(GPIOE, &igpio);
	
	/* Initialize the UART7 peripheral. */
	initUART(huart7, UART7, 9600, UART_MODE_RX, UART_WORDLENGTH_8B, 
		UART_STOPBITS_1, UART_PARITY_NONE, UART_HWCONTROL_NONE, 
		UART_OVERSAMPLING_16);
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
	initGPIO(igpio, SD_PIN_CD, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);

	/* Initialize the GPIOC peripheral. */
	initGPIO(igpio, SD_PIN_DAT0 | SD_PIN_DAT1 | SD_PIN_DAT2 | SD_PIN_DAT3 | 
		SD_PIN_CLK, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF12_SDMMC1);
	HAL_GPIO_Init(GPIOC, &igpio);

	/* Initialize the GPIOD peripheral. */
	initGPIO(igpio, SD_PIN_CMD, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF12_SDMMC1);
	HAL_GPIO_Init(GPIOD, &igpio);

	/* Initialize the SDMMC1 peripheral. */
	initSDMMC(hsdmmc1, SDMMC1, SDMMC_BUS_WIDE_4B, SDMMC_CLOCK_EDGE_RISING, 
		SDMMC_CLOCK_POWER_SAVE_DISABLE, 1, SDMMC_HARDWARE_FLOW_CONTROL_DISABLE);
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
	initGPIO(igpio, LORA_PIN_M0 | LORA_PIN_M1, GPIO_MODE_OUTPUT_PP, 
		GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOD, &igpio);

	initGPIO(igpio, LORA_PIN_AUX, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOD, &igpio);

	/* Initialize the GPIOB peripheral. */
	initGPIO(igpio, LORA_PIN_TX | LORA_PIN_RX, GPIO_MODE_AF_PP, 
		GPIO_NOPULL, GPIO_AF8_UART5);
	HAL_GPIO_Init(GPIOB, &igpio);

	/* Initialize the UART5 peripheral. */
	initUART(huart5, UART5, 115200, UART_MODE_TX, UART_WORDLENGTH_8B, 
		UART_STOPBITS_1, UART_PARITY_NONE, UART_HWCONTROL_NONE, 
		UART_OVERSAMPLING_16);
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
	initGPIO(igpio, LED_PIN_HEARTBEAT | LED_PIN_ERROR | LED_PIN_DETECT, 
		GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOE, &igpio);
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
 
