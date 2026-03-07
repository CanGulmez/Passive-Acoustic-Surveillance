/**
 ******************************************************************************
 * @file 	peripheral.h
 * @author 	Ahmet Can GULMEZ
 * @brief 	Peripheral headers of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Ahmet Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

/* Oscillator and Clock Definitions */

#define OSC_TYPE								RCC_OSCILLATORTYPE_HSE
#define OSC_HSE_STATE						RCC_HSE_ON
#define OSC_PLL_STATE						RCC_PLL_ON
#define OSC_PLL_SRC							RCC_PLLSOURCE_HSE
#define OSC_PLLM								5
#define OSC_PLLN								80
#define OSC_PLLP								4
#define OSC_PLLQ								8
#define OSC_PLLR								8
#define OSC_PLLRGE							RCC_PLL1VCIRANGE_2
#define OSC_PLLVCOSEL						RCC_PLL1VCOWIDE
#define OSC_PLLFRACN							0

#define CLOCK_TYPE							RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | \
													RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | \
													RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1
#define CLOCK_SOURCE							RCC_SYSCLKSOURCE_PLLCLK
#define CLOCK_SYS_DIV						RCC_SYSCLK_DIV1
#define CLOCK_AHB_DIV						RCC_HCLK_DIV1
#define CLOCK_APB1_DIV						RCC_APB1_DIV2
#define CLOCK_APB2_DIV						RCC_APB2_DIV2
#define CLOCK_APB3_DIV						RCC_APB3_DIV2
#define CLOCK_APB4_DIV						RCC_APB4_DIV2

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

/* Debug Port Definitions */

#define DEBUG_PIN_TX							GPIO_PIN_0	/* GPIOA - UART4 */
#define DEBUG_PIN_RX							GPIO_PIN_1	/* GPIOA - UART4 */
#define DEBUG_PORT							GPIOA
#define DEBUG_PULL							GPIO_NOPULL
#define DEBUG_GPIO_MODE						GPIO_MODE_AF_PP
#define DEBUG_ALTERNATE						GPIO_AF6_UART4
#define DEBUG_INSTANCE						UART4
#define DEBUG_UART_MODE						UART_MODE_RX | UART_MODE_TX								
#define DEBUG_BAUDRATE						115200
#define DEBUG_WORDLEN						UART_WORDLENGTH_8B
#define DEBUG_STOPBITS						UART_STOPBITS_1
#define DEBUG_PARITY							UART_PARITY_NONE
#define DEBUG_HWCONTROL						UART_HWCONTROL_NONE
#define DEBUG_SAMPLING						UART_OVERSAMPLING_16

/* LoRa Module Definitions */

#define LORA_PIN_TX							GPIO_PIN_13	/* GPIOB - UART5 */
#define LORA_PIN_RX							GPIO_PIN_12	/* GPIOB - UART5 */
#define LORA_PIN_AUX							GPIO_PIN_3	/* GPIOD - Input */
#define LORA_PIN_M0							GPIO_PIN_5	/* GPIOD - Output */
#define LORA_PIN_M1							GPIO_PIN_4	/* GPIOD - Output */
#define LORA_PORTB							GPIOB
#define LORA_PORTD							GPIOD
#define LORA_GPIO_MODE_O					GPIO_MODE_OUTPUT_PP
#define LORA_GPIO_MODE_I					GPIO_MODE_INPUT
#define LORA_GPIO_MODE_AF					GPIO_MODE_AF_PP
#define LORA_PULL								GPIO_NOPULL
#define LORA_ALTERNATE						GPIO_AF8_UART5
#define LORA_INSTANCE						UART5
#define LORA_MODE								UART_MODE_RX | UART_MODE_TX
#define LORA_BAUDRATE						115200
#define LORA_WORDLEN							UART_WORDLENGTH_8B
#define LORA_STOPBITS						UART_STOPBITS_1
#define LORA_PARITY							UART_PARITY_NONE
#define LORA_HWCONTROL						UART_HWCONTROL_NONE
#define LORA_SAMPLING						UART_OVERSAMPLING_16

/* GPS Module Definitions */

#define GPS_PIN_RX							GPIO_PIN_7	/* GPIOE - UART7 */
#define GPS_PIN_TX							GPIO_PIN_8	/* GPIOE - UART7 */
#define GPS_PORT								GPIOE
#define GPS_GPIO_MODE						GPIO_MODE_AF_PP
#define GPS_PULL								GPIO_NOPULL
#define GPS_ALTERNATE						GPIO_AF7_UART7
#define GPS_INSTANCE							UART7
#define GPS_UART_MODE						UART_MODE_RX | UART_MODE_TX							
#define GPS_BAUDRATE							9600
#define GPS_WORDLEN							UART_WORDLENGTH_8B
#define GPS_STOPBITS							UART_STOPBITS_1
#define GPS_PARITY							UART_PARITY_NONE
#define GPS_HWCONTROL						UART_HWCONTROL_NONE
#define GPS_SAMPLING							UART_OVERSAMPLING_16

/* SD Card Definitions */

#define SD_PIN_CD								GPIO_PIN_8	/* GPIOA - Output */
#define SD_PIN_DAT0							GPIO_PIN_8	/* GPIOC - SDMMC1 */
#define SD_PIN_DAT1							GPIO_PIN_9	/* GPIOC - SDMMC1 */
#define SD_PIN_DAT2							GPIO_PIN_10	/* GPIOC - SDMMC1 */
#define SD_PIN_DAT3							GPIO_PIN_11	/* GPIOC - SDMMC1 */
#define SD_PIN_CMD							GPIO_PIN_2	/* GPIOD - SDMMC1 */
#define SD_PIN_CLK							GPIO_PIN_12	/* GPIOC - SDMMC1 */
#define SD_PORTA								GPIOA
#define SD_PORTC								GPIOC
#define SD_PORTD								GPIOD
#define SD_GPIO_MODE_AF						GPIO_MODE_AF_PP
#define SD_GPIO_MODE_O						GPIO_MODE_OUTPUT_PP
#define SD_PULL								GPIO_NOPULL
#define SD_ALTERNATE							GPIO_AF12_SDMMC1
#define SD_INSTANCE							SDMMC1
#define SD_BUS_WIDE							SDMMC_BUS_WIDE_4B
#define SD_CLOCK_EDGE						SDMMC_CLOCK_EDGE_RISING
#define SD_POWER_SAVE						SDMMC_CLOCK_POWER_SAVE_DISABLE
#define SD_CLOCKDIV							1
#define SD_HWCONTROL							SDMMC_HARDWARE_FLOW_CONTROL_DISABLE

/* IMU Sensor Definitions */

#define IMU_PIN_NSS							GPIO_PIN_4	/* GPIOA - Output */
#define IMU_PIN_SCK							GPIO_PIN_5	/* GPIOA - SPI1 */
#define IMU_PIN_MISO							GPIO_PIN_6	/* GPIOA - SPI1 */
#define IMU_PIN_MOSI							GPIO_PIN_7	/* GPIOA - SPI1 */
#define IMU_PIN_INT1							GPIO_PIN_2	/* GPIOB - Input */
#define IMU_PIN_INT2							GPIO_PIN_3	/* GPIOA - Input */
#define IMU_PORTA								GPIOA
#define IMU_PORTB								GPIOB
#define IMU_GPIO_MODE_I						GPIO_MODE_INPUT
#define IMU_GPIO_MODE_O						GPIO_MODE_OUTPUT_PP
#define IMU_GPIO_MODE_AF					GPIO_MODE_AF_PP
#define IMU_PULL								GPIO_NOPULL
#define IMU_ALTERNATE						GPIO_AF5_SPI1
#define IMU_INSTANCE							SPI1
#define IMU_SPI_MODE							SPI_MODE_MASTER
#define IMU_DIRECTION						SPI_DIRECTION_2LINES
#define IMU_DATASIZE							SPI_DATASIZE_8BIT
#define IMU_POLARITY							SPI_POLARITY_LOW
#define IMU_PHASE								SPI_PHASE_1EDGE
#define IMU_NSS								SPI_NSS_SOFT
#define IMU_PRESCALER						SPI_BAUDRATEPRESCALER_8
#define IMU_FIRSTBIT							SPI_FIRSTBIT_MSB
#define IMU_TIMODE							SPI_TIMODE_DISABLE

#define IMU_REG_FUNC_CFG_ACCESS			0x01
#define IMU_REG_SENSOR_SYNC_TIME			0x04
#define IMU_REG_SENSOR_SYNC_RES			0x05
#define IMU_REG_FIFO_CTRL1					0x06
#define IMU_REG_FIFO_CTRL2					0x07
#define IMU_REG_FIFO_CTRL3					0x08
#define IMU_REG_FIFO_CTRL4					0x09
#define IMU_REG_FIFO_CTRL5					0x0A
#define IMU_REG_DYDY_PULSE_CFG_G			0x0B
#define IMU_REG_INT1_CTRL					0x0D
#define IMU_REG_INT2_CTRL					0x0E
#define IMU_REG_WHO_AM_I					0x0F
#define IMU_REG_CTRL1_XL					0x10
#define IMU_REG_CTRL2_G						0x11
#define IMU_REG_CTRL3_C						0x12
#define IMU_REG_CTRL4_C						0x13
#define IMU_REG_CTRL5_C						0x14
#define IMU_REG_CTRL6_C						0x15
#define IMU_REG_CTRL7_G						0x15
#define IMU_REG_CTRL8_XL					0x17
#define IMU_REG_CTR9_XL						0x18
#define IMU_REG_CTRL10_C					0x19
#define IMU_REG_MASTER_CONFIG				0x1A
#define IMU_REG_WAKE_UP_SRC				0x1B
#define IMU_REG_TAP_SRC						0x1C
#define IMU_REG_D6D_SRC						0x1D
#define IMU_REG_STATUS_REG					0x1E	
#define IMU_REG_OUT_TEMP_L					0x20	
#define IMU_REG_OUT_TEMP_H					0x21
#define IMU_REG_OUTX_L_G					0x22
#define IMU_REG_OUTX_H_G					0x23
#define IMU_REG_OUTY_L_G					0x24
#define IMU_REG_OUTY_H_G					0x25
#define IMU_REG_OUTZ_L_G					0x26
#define IMU_REG_OUTZ_H_G					0x27
#define IMU_REG_OUTX_L_XL					0x28
#define IMU_REG_OUTX_H_XL					0x29
#define IMU_REG_OUTY_L_XL					0x2A
#define IMU_REG_OUTY_H_XL					0x2B
#define IMU_REG_OUTZ_L_XL					0x2C
#define IMU_REG_OUTZ_H_XL					0x2D
#define IMU_REG_SENSORHUB1_REG			0x2E
#define IMU_REG_SENSORHUB2_REG			0x2F
#define IMU_REG_SENSORHUB3_REG			0x30
#define IMU_REG_SENSORHUB4_REG			0x31
#define IMU_REG_SENSORHUB5_REG			0x32
#define IMU_REG_SENSORHUB6_REG			0x33
#define IMU_REG_SENSORHUB7_REG			0x34
#define IMU_REG_SENSORHUB8_REG			0x35
#define IMU_REG_SENSORHUB9_REG			0x36
#define IMU_REG_SENSORHUB10_REG			0x37
#define IMU_REG_SENSORHUB11_REG			0x38
#define IMU_REG_SENSORHUB12_REG			0x39
#define IMU_REG_FIFO_STATUS1				0x3A
#define IMU_REG_FIFO_STATUS2				0x3B
#define IMU_REG_FIFO_STATUS3				0x3C
#define IMU_REG_FIFO_STATUS4				0x3D
#define IMU_REG_FIFO_DATA_OUT_L			0x3E
#define IMU_REG_FIFO_DATA_OUT_H			0x3F
#define IMU_REG_TIMESTAMP0_REG			0x40
#define IMU_REG_TIMESTAMP1_REG			0x41
#define IMU_REG_TIMESTAMP2_REG			0x42
#define IMU_REG_STEP_TIMESTAMP_L			0x49
#define IMU_REG_STEP_TIMESTAMP_H			0x4A
#define IMU_REG_STEP_COUNTER_L			0x4B
#define IMU_REG_STEP_COUNTER_H			0x4C
#define IMU_REG_SENSORHUB13_REG			0x4D
#define IMU_REG_SENSORHUB14_REG			0x4E
#define IMU_REG_SENSORHUB15_REG			0x4F
#define IMU_REG_SENSORHUB16_REG			0x50
#define IMU_REG_SENSORHUB17_REG			0x51
#define IMU_REG_SENSORHUB18_REG			0x52
#define IMU_REG_FUNC_SRC1					0x53
#define IMU_REG_FUNC_SRC2					0x54
#define IMU_REG_WRIST_TILT_IA				0x55
#define IMU_REG_TAP_CFG						0x58
#define IMU_REG_TAP_THS_6D					0x59
#define IMU_REG_INT_DUR2					0x5A
#define IMU_REG_WAKE_UP_THS				0x5B
#define IMU_REG_WAKE_UP_DUR				0x5C
#define IMU_REG_FREE_FALL					0x5D
#define IMU_REG_MD1_CFG						0x5E
#define IMU_REG_MD2_CFG						0x5F
#define IMU_REG_MASTER_CMD_CODE			0x60
#define IMU_REG_SENS_SYNC_SPI_ERROR		0x61
#define IMU_REG_OUT_MAG_RAW_X_L			0x66
#define IMU_REG_OUT_MAG_RAW_X_H			0x67
#define IMU_REG_OUT_MAG_RAW_Y_L			0x68
#define IMU_REG_OUT_MAG_RAW_Y_H			0x69
#define IMU_REG_OUT_MAG_RAW_Z_L			0x6A
#define IMU_REG_OUT_MAG_RAW_Z_H			0x6B
#define IMU_REG_X_OFS_USR					0x73
#define IMU_REG_Y_OFS_USR					0x74
#define IMU_REG_Z_OFS_USR					0x75

/* Microhone Sensor Definitions */

/**
 * System Clock:								100 MHz
 * DFSDM Clock:								100 MHz / DIVIDER = 3.125 MHz
 * Filter Output Rate:						3.125 MHz / OVERSAMPLING = 48.828 kHz
 * Actual Audio Rate:						48.828 kHz / (ORDER + 1) = 12.207 kHz
 */

#define MIC_PIN_DATAIN0						GPIO_PIN_1	/* GPIOC - DFSDM1 */ 
#define MIC_PIN_DATAIN1						GPIO_PIN_1	/* GPIOB - DFSDM1 */
#define MIC_PIN_DATAIN2						GPIO_PIN_5	/* GPIOC - DFSDM1 */
#define MIC_PIN_DATAIN3						GPIO_PIN_7	/* GPIOC - DFSDM1 */
#define MIC_PIN_CKOUT						GPIO_PIN_0	/* GPIOB - DFSDM1 */
#define MIC_PORTB								GPIOB
#define MIC_PORTC								GPIOC
#define MIC_GPIO_MODE						GPIO_MODE_AF_PP
#define MIC_PULL								GPIO_NOPULL
#define MIC_ALTERNATE						GPIO_AF3_DFSDM1
#define MIC_DIVIDER							32
#define MIC_FILTER_TYPE						DFSDM_CHANNEL_FASTSINC_ORDER
#define MIC_OFFSET							0
#define MIC_RIGHT_SHIFT						0
#define MIC_SINC_ORDER						DFSDM_FILTER_SINC3_ORDER
#define MIC_OVERSAMPLING					64

/* LED Definitions */

#define LED_PIN_HEARTBEAT					GPIO_PIN_0	/* GPIOE - Output */
#define LED_PIN_ERROR						GPIO_PIN_1	/* GPIOE - Output */
#define LED_PIN_DETECT						GPIO_PIN_9	/* GPIOE - Output */
#define LED_PORT								GPIOE
#define LED_MODE								GPIO_MODE_OUTPUT_PP
#define LED_PULL								GPIO_NOPULL

/* Servo Motors Definitions */

#define SERVO_PIN_1							GPIO_PIN_6	/* GPIOB - TIM4 */
#define SERVO_PIN_2							GPIO_PIN_7	/* GPIOB - TIM4 */
#define SERVO_PIN_3							GPIO_PIN_8	/* GPIOB - TIM4 */
