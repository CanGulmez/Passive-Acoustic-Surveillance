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

RCC_OscInitTypeDef iosc = {0};				/* Oscillator */
RCC_ClkInitTypeDef iclk = {0};				/* Clock */

GPIO_InitTypeDef igpio = {0};				/* Generic IO */
UART_HandleTypeDef huart4 = {0};			/* Debug Port */

RCC_PeriphCLKInitTypeDef hdfsdm1clk = {0};
DFSDM_Channel_HandleTypeDef hdfsdm1c2 = {0};
DFSDM_Channel_HandleTypeDef hdfsdm1c3 = {0};
DFSDM_Channel_HandleTypeDef hdfsdm1c4 = {0};
DFSDM_Channel_HandleTypeDef hdfsdm1c7 = {0};
DFSDM_Filter_HandleTypeDef hdfsdm1f0 = {0};
DFSDM_Filter_HandleTypeDef hdfsdm1f1 = {0};
DFSDM_Filter_HandleTypeDef hdfsdm1f2 = {0};
DFSDM_Filter_HandleTypeDef hdfsdm1f3 = {0};
DMA_HandleTypeDef hdfsdm1dma0 = {0};
DMA_HandleTypeDef hdfsdm1dma1 = {0};
DMA_HandleTypeDef hdfsdm1dma2 = {0};
DMA_HandleTypeDef hdfsdm1dma3 = {0};

SPI_HandleTypeDef hspi1 = {0};				/* IMU Sensor */

SD_HandleTypeDef hsdmmc1 = {0};				/* SD Card */
MPU_Region_InitTypeDef hsdmmc1mpu = {0};	/* SD Card MPU */
RCC_PeriphCLKInitTypeDef hsdmmc1clk = {0};	/* SD Card Clock */


/**
 * Configurate the oscillator and clock sources.
 */
void configOscClk(void)
{
	/**
	 * PLL1_M: 5				(25MHz / 5 = 5MHz)
	 * PLL1_N: 80				(5MHz × 80 = 400MHz)
	 * PLL1_P: 4				(400MHz / 4 = 100MHz) -> SYSCLK
	 * PLL1_Q: 8				(400MHz / 8 = 50MHz)  -> For SDMMC, USB, RNG
	 * PLL1_R: 8				(400MHz / 8 = 50MHz)  -> For SPI, I2S
	 * 
	 * AHB Prescaler: 1			HCLK = 100MHz
	 * APB1 Prescaler: 2		PCLK1 = 50MHz
	 * APB2 Prescaler: 2		PCLK2 = 50MHz
	 * APB3 Prescaler: 2		PCLK3 = 50MHz
	 * APB4 Prescaler: 2		PCLK4 = 50MHz
	 */

	/* Initialize the system oscillator. */
	iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	iosc.HSEState = RCC_HSE_ON;
	iosc.PLL.PLLState = RCC_PLL_ON;
	iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	iosc.PLL.PLLM = 5;
	iosc.PLL.PLLN = 80;
	iosc.PLL.PLLP = 4;
	iosc.PLL.PLLQ = 8;
	iosc.PLL.PLLR = 8;
	iosc.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	iosc.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	iosc.PLL.PLLFRACN = 0;	
	HAL_RCC_OscConfig(&iosc);

	/* Initialize the system clock. */
	iclk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | 
					 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | 
					 RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1;
	iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	iclk.SYSCLKDivider = RCC_SYSCLK_DIV1;
	iclk.AHBCLKDivider = RCC_HCLK_DIV1;
	iclk.APB1CLKDivider = RCC_APB1_DIV2;
	iclk.APB2CLKDivider = RCC_APB2_DIV2;
	iclk.APB3CLKDivider = RCC_APB3_DIV2;
	iclk.APB4CLKDivider = RCC_APB4_DIV2;
	HAL_RCC_ClockConfig(&iclk, FLASH_LATENCY_3);

	hdfsdm1clk.PeriphClockSelection = RCC_PERIPHCLK_DFSDM1;
	hdfsdm1clk.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_D2PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&hdfsdm1clk);

	hsdmmc1clk.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
    hsdmmc1clk.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
	HAL_RCCEx_PeriphCLKConfig(&hsdmmc1clk);

	SystemCoreClock = HAL_RCC_GetSysClockFreq();
}

/**
 * Configurate the serial line.
 */
void configSerialLine(void)
{
	/* Initialize the GPIOA peripheral. */
	initGPIO(&igpio, SERIAL_PIN_TX | SERIAL_PIN_RX, GPIO_MODE_AF_PP, 
		GPIO_NOPULL, GPIO_AF8_UART4);
	HAL_GPIO_Init(GPIOA, &igpio);

	/* Initialize the UART4 peripheral. */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart4);
}

/**
 * Configure the microphone sensors.
 */
void configMicSensors(void)
{
	HAL_StatusTypeDef status;

	/**
 	 * APB1 Clock:				50 MHz
 	 * DFSDM Clock:				50 MHz / DIVIDER = 1.5625 MHz
 	 * Filter Output Rate:		1.25 MHz / OVERSAMPLING = 48.828 kHz
 	 */

	/* Initialize the GPIO peripheral. */

	initGPIO(&igpio, GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF6_DFSDM1);
	HAL_GPIO_Init(GPIOB, &igpio);

	initGPIO(&igpio, GPIO_PIN_10, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF3_DFSDM1);
	HAL_GPIO_Init(GPIOE, &igpio);

	initGPIO(&igpio, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF4_DFSDM1);
	HAL_GPIO_Init(GPIOC, &igpio);

	initGPIO(&igpio, GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF3_DFSDM1);
	HAL_GPIO_Init(GPIOB, &igpio);

	/*************************************************************************/

	/* Initialize the DFSDM1 channels. */

    hdfsdm1c2.Instance = DFSDM1_Channel2;
    hdfsdm1c2.Init.OutputClock.Activation = ENABLE;
    hdfsdm1c2.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    hdfsdm1c2.Init.OutputClock.Divider = 32;
    hdfsdm1c2.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    hdfsdm1c2.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    hdfsdm1c2.Init.Input.Pins = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;
    hdfsdm1c2.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
    hdfsdm1c2.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hdfsdm1c2.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    hdfsdm1c2.Init.Awd.Oversampling = 1;
    hdfsdm1c2.Init.Offset = 0;
    hdfsdm1c2.Init.RightBitShift = 0;

    status = HAL_DFSDM_ChannelInit(&hdfsdm1c2);
    if (status != HAL_OK)
        printError(status, "Failed to initialize DFSDM1 channel!");

	hdfsdm1c3.Instance = DFSDM1_Channel3;
	hdfsdm1c3.Init.OutputClock.Activation = ENABLE;
	hdfsdm1c3.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
	hdfsdm1c3.Init.OutputClock.Divider = 32;
	hdfsdm1c3.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
	hdfsdm1c3.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
	hdfsdm1c3.Init.Input.Pins = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;
	hdfsdm1c3.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
	hdfsdm1c3.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
	hdfsdm1c3.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
	hdfsdm1c3.Init.Awd.Oversampling = 1;
	hdfsdm1c3.Init.Offset = 0;
	hdfsdm1c3.Init.RightBitShift = 0;

	status = HAL_DFSDM_ChannelInit(&hdfsdm1c3);
	if (status != HAL_OK)
	    printError(status, "Failed to initialize DFSDM1 channel 3!");

	hdfsdm1c4.Instance = DFSDM1_Channel4;
	hdfsdm1c4.Init.OutputClock.Activation = ENABLE;
	hdfsdm1c4.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
	hdfsdm1c4.Init.OutputClock.Divider = 32;
	hdfsdm1c4.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
	hdfsdm1c4.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
	hdfsdm1c4.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
	hdfsdm1c4.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
	hdfsdm1c4.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
	hdfsdm1c4.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
	hdfsdm1c4.Init.Awd.Oversampling = 1;
	hdfsdm1c4.Init.Offset = 0;
	hdfsdm1c4.Init.RightBitShift = 0;

	status = HAL_DFSDM_ChannelInit(&hdfsdm1c4);
	if (status != HAL_OK)
	    printError(status, "Failed to initialize DFSDM1 channel 4!");

	hdfsdm1c7.Instance = DFSDM1_Channel7;
	hdfsdm1c7.Init.OutputClock.Activation = ENABLE;
	hdfsdm1c7.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
	hdfsdm1c7.Init.OutputClock.Divider = 32;
	hdfsdm1c7.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
	hdfsdm1c7.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
	hdfsdm1c7.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
	hdfsdm1c7.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
	hdfsdm1c7.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
	hdfsdm1c7.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
	hdfsdm1c7.Init.Awd.Oversampling = 1;
	hdfsdm1c7.Init.Offset = 0;
	hdfsdm1c7.Init.RightBitShift = 0;

	status = HAL_DFSDM_ChannelInit(&hdfsdm1c7);
	if (status != HAL_OK)
	    printError(status, "Failed to initialize DFSDM1 channel 7!");

	/*************************************************************************/

	/* Initialize the DFSDM1 filters. */

	hdfsdm1f0.Instance = DFSDM1_Filter0;
	hdfsdm1f0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
	hdfsdm1f0.Init.RegularParam.FastMode = DISABLE;	
	hdfsdm1f0.Init.RegularParam.DmaMode = ENABLE;
	hdfsdm1f0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
	hdfsdm1f0.Init.FilterParam.Oversampling = 32;
	hdfsdm1f0.Init.FilterParam.IntOversampling = 1;

	status = HAL_DFSDM_FilterInit(&hdfsdm1f0);
	if (status != HAL_OK)
		printError(status, "Failed to initialize DFSDM1 filter 0!");

	hdfsdm1f1.Instance = DFSDM1_Filter1;
	hdfsdm1f1.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
	hdfsdm1f1.Init.RegularParam.FastMode = DISABLE;	
	hdfsdm1f1.Init.RegularParam.DmaMode = ENABLE;
	hdfsdm1f1.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
	hdfsdm1f1.Init.FilterParam.Oversampling = 32;
	hdfsdm1f1.Init.FilterParam.IntOversampling = 1;

	status = HAL_DFSDM_FilterInit(&hdfsdm1f1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize DFSDM1 filter 1!");

	hdfsdm1f2.Instance = DFSDM1_Filter2;
	hdfsdm1f2.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
	hdfsdm1f2.Init.RegularParam.FastMode = DISABLE;	
	hdfsdm1f2.Init.RegularParam.DmaMode = ENABLE;
	hdfsdm1f2.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
	hdfsdm1f2.Init.FilterParam.Oversampling = 32;
	hdfsdm1f2.Init.FilterParam.IntOversampling = 1;

	status = HAL_DFSDM_FilterInit(&hdfsdm1f2);
	if (status != HAL_OK)
		printError(status, "Failed to initialize DFSDM1 filter 2!");

	hdfsdm1f3.Instance = DFSDM1_Filter3;
	hdfsdm1f3.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
	hdfsdm1f3.Init.RegularParam.FastMode = DISABLE;	
	hdfsdm1f3.Init.RegularParam.DmaMode = ENABLE;
	hdfsdm1f3.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
	hdfsdm1f3.Init.FilterParam.Oversampling = 32;
	hdfsdm1f3.Init.FilterParam.IntOversampling = 1;

	status = HAL_DFSDM_FilterInit(&hdfsdm1f3);
	if (status != HAL_OK)
		printError(status, "Failed to initialize DFSDM1 filter 3!");

	/*************************************************************************/

	/* Assign the channels to the associated filters. */

	status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f0, DFSDM_CHANNEL_7, DFSDM_CONTINUOUS_CONV_ON);
	if (status != HAL_OK)
        printError(status, "Failed to assign the filter to the channel!");

	status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f1, DFSDM_CHANNEL_2, DFSDM_CONTINUOUS_CONV_ON);
	if (status != HAL_OK)
        printError(status, "Failed to assign the filter to the channel!");

	status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f2, DFSDM_CHANNEL_4, DFSDM_CONTINUOUS_CONV_ON);
	if (status != HAL_OK)
        printError(status, "Failed to assign the filter to the channel!");

	status = HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1f3, DFSDM_CHANNEL_3, DFSDM_CONTINUOUS_CONV_ON);
	if (status != HAL_OK)
        printError(status, "Failed to assign the filter to the channel!");

	/*************************************************************************/

	/* Initialize the DMA1_Stream0..3 for DFSDM1. */

	hdfsdm1dma0.Instance = DMA1_Stream0;
	hdfsdm1dma0.Init.Request = DMA_REQUEST_DFSDM1_FLT0;
	hdfsdm1dma0.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdfsdm1dma0.Init.PeriphInc = DMA_PINC_DISABLE;
	hdfsdm1dma0.Init.MemInc = DMA_MINC_ENABLE;	
	hdfsdm1dma0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdfsdm1dma0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdfsdm1dma0.Init.Mode = DMA_CIRCULAR;
	hdfsdm1dma0.Init.Priority = DMA_PRIORITY_HIGH;	
	hdfsdm1dma0.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	status = HAL_DMA_Init(&hdfsdm1dma0);
	if (status != HAL_OK)
		printError(status, "Failed to initialize the DMA1 Stream0 for DFSDM1!");

	hdfsdm1dma1.Instance = DMA1_Stream1;
	hdfsdm1dma1.Init.Request = DMA_REQUEST_DFSDM1_FLT1;
	hdfsdm1dma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdfsdm1dma1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdfsdm1dma1.Init.MemInc = DMA_MINC_ENABLE;	
	hdfsdm1dma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdfsdm1dma1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdfsdm1dma1.Init.Mode = DMA_CIRCULAR;
	hdfsdm1dma1.Init.Priority = DMA_PRIORITY_HIGH;	
	hdfsdm1dma1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	status = HAL_DMA_Init(&hdfsdm1dma1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize the DMA1 Stream1 for DFSDM1!");

	hdfsdm1dma2.Instance = DMA1_Stream2;
	hdfsdm1dma2.Init.Request = DMA_REQUEST_DFSDM1_FLT2;
	hdfsdm1dma2.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdfsdm1dma2.Init.PeriphInc = DMA_PINC_DISABLE;
	hdfsdm1dma2.Init.MemInc = DMA_MINC_ENABLE;	
	hdfsdm1dma2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdfsdm1dma2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdfsdm1dma2.Init.Mode = DMA_CIRCULAR;
	hdfsdm1dma2.Init.Priority = DMA_PRIORITY_HIGH;	
	hdfsdm1dma2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	status = HAL_DMA_Init(&hdfsdm1dma2);
	if (status != HAL_OK)
		printError(status, "Failed to initialize the DMA1 Stream2 for DFSDM1!");

	hdfsdm1dma3.Instance = DMA1_Stream3;
	hdfsdm1dma3.Init.Request = DMA_REQUEST_DFSDM1_FLT3;
	hdfsdm1dma3.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdfsdm1dma3.Init.PeriphInc = DMA_PINC_DISABLE;
	hdfsdm1dma3.Init.MemInc = DMA_MINC_ENABLE;	
	hdfsdm1dma3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdfsdm1dma3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdfsdm1dma3.Init.Mode = DMA_CIRCULAR;
	hdfsdm1dma3.Init.Priority = DMA_PRIORITY_HIGH;	
	hdfsdm1dma3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	status = HAL_DMA_Init(&hdfsdm1dma3);
	if (status != HAL_OK)
		printError(status, "Failed to initialize the DMA1 Stream3 for DFSDM1!");

	/* Link DMA1_Stream0 to DFSDM1 filter. */
	__HAL_LINKDMA(&hdfsdm1f0, hdmaReg, hdfsdm1dma0);
	__HAL_LINKDMA(&hdfsdm1f1, hdmaReg, hdfsdm1dma1);
	__HAL_LINKDMA(&hdfsdm1f2, hdmaReg, hdfsdm1dma2);
	__HAL_LINKDMA(&hdfsdm1f3, hdmaReg, hdfsdm1dma3);

	__HAL_LINKDMA(&hdfsdm1f0, hdmaInj, hdfsdm1dma0);
	__HAL_LINKDMA(&hdfsdm1f1, hdmaInj, hdfsdm1dma1);
	__HAL_LINKDMA(&hdfsdm1f2, hdmaInj, hdfsdm1dma2);
	__HAL_LINKDMA(&hdfsdm1f3, hdmaInj, hdfsdm1dma3);

	/* Configure the DMA1_Stream0-3 interrupts. */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, MIC_IRQ, 0);
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, MIC_IRQ, 0);
	HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, MIC_IRQ, 0);
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, MIC_IRQ, 0);

	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}
 
/**
 * Configure the IMU sensor.
 */
void configIMUSensor(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIO peripheral. */

	initGPIO(&igpio, IMU_PIN_NSS, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);
	IMU_NSS_HIGH();

	initGPIO(&igpio, IMU_PIN_INT2, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);

	initGPIO(&igpio, IMU_PIN_SCK | IMU_PIN_MISO | IMU_PIN_MOSI, GPIO_MODE_AF_PP,
		GPIO_NOPULL, GPIO_AF5_SPI1);
	HAL_GPIO_Init(GPIOA, &igpio);

	initGPIO(&igpio, IMU_PIN_INT1, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOB, &igpio);

	/* Initialize the SPI1 peripheral. */

	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;

	status = HAL_SPI_Init(&hspi1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SPI1 peripheral!");
}

/**
 * Configure the SD card writing.
 */
void configSDWriting(void)
{
	HAL_StatusTypeDef status;

	/* Initialize the GPIOA peripheral. */
	initGPIO(&igpio, SD_PIN_CD, GPIO_MODE_INPUT, GPIO_NOPULL, -1);
	HAL_GPIO_Init(GPIOA, &igpio);

	/* Initialize the GPIOC peripheral. */
	initGPIO(&igpio, SD_PIN_DAT0 | SD_PIN_DAT1 | SD_PIN_DAT2 | SD_PIN_DAT3 | 
		SD_PIN_CLK, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF12_SDMMC1);
	HAL_GPIO_Init(GPIOC, &igpio);

	/* Initialize the GPIOD peripheral. */
	initGPIO(&igpio, SD_PIN_CMD, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF12_SDMMC1);
	HAL_GPIO_Init(GPIOD, &igpio);

	/* Initialize the SDMMC1 peripheral. */
	hsdmmc1.Instance = SDMMC1;
	hsdmmc1.Init.BusWide = SDMMC_BUS_WIDE_1B;
	hsdmmc1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	hsdmmc1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	hsdmmc1.Init.ClockDiv = 1;
	hsdmmc1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;

	status = HAL_SD_Init(&hsdmmc1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SDMMC1 peripheral!");

	status = HAL_SD_ConfigWideBusOperation(&hsdmmc1, SDMMC_BUS_WIDE_4B);
	if (status != HAL_OK)
		printError(status, "Failed to configure SDMMC1 wide bus operation!");

	/* SDMMC1 internally use the DMA stream... */
	HAL_NVIC_SetPriority(SDMMC1_IRQn, SD_IRQ, 0);
	HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

	/*************************************************************************/

	/* Disable the cache for SDMMC1 for AXI SRAM access. */
	// HAL_MPU_Disable();

	// hsdmmc1mpu.Enable = MPU_REGION_ENABLE;
	// hsdmmc1mpu.Number = MPU_REGION_NUMBER0;
	// hsdmmc1mpu.BaseAddress = 0x24000000;
	// hsdmmc1mpu.Size = MPU_REGION_SIZE_512KB;
	// hsdmmc1mpu.SubRegionDisable = 0x0;
	// hsdmmc1mpu.TypeExtField = MPU_TEX_LEVEL1;	/* no-cachable */
	// hsdmmc1mpu.AccessPermission = MPU_REGION_FULL_ACCESS;
	// hsdmmc1mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	// hsdmmc1mpu.IsShareable = MPU_ACCESS_SHAREABLE;
	// hsdmmc1mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	// hsdmmc1mpu.IsBufferable = MPU_ACCESS_BUFFERABLE;
	// HAL_MPU_ConfigRegion(&hsdmmc1mpu);

	// HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
