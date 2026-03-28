# Embedded Firmware of Acoustic Surveillance System

# Toolchain definitions
CC					:= arm-none-eabi-gcc
OBJCOPY			:= arm-none-eabi-objcopy
SIZE				:= arm-none-eabi-size
RM					:= rm
FILE				:= file
AR					:= ar rcs

# Microcontroller-specific definitions
DEVICE_FAMILY	:= STM32H7xx
DEVICE_MODEL	:= STM32H750xx
DEVICE_VARIANT	:= STM32H750VBTx

# Compiler and linker flags
CORTEX_FLAGS	:= -mthumb -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard
COMMON_FLAGS	:= -O0 -Wall -ffunction-sections -fdata-sections
AS_FLAGS			:= -x assembler-with-cpp

# Include paths
MAIN_INC			:= ./src
CMSIS_INC		:= ./driver/CMSIS/Include 
CMSIS_DEV_INC	:= ./driver/CMSIS/Device/ST/STM32H7xx/Include
HAL_INC			:= ./driver/STM32H7xx_HAL_Driver/Inc
RTOS_INC			:= ./driver/FreeRTOS/include 
RTOS_GCC_INC	:= ./driver/FreeRTOS/portable/GCC/ARM_CM7

# Source files
MAIN_SRC			:= $(wildcard ./src/*.c)
HAL_SRC			:= $(wildcard ./driver/STM32H7xx_HAL_Driver/Src/*.c)
SYSTEM_SRC		:= ./driver/CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c
RTOS_SRC			:= $(wildcard ./driver/FreeRTOS/*.c) 
RTOS_GCC_SRC	:= $(wildcard ./driver/FreeRTOS/portable/GCC/ARM_CM7/*.c) 
RTOS_MEM_SRC	:= $(wildcard ./driver/FreeRTOS/portable/MemMang/*.c) 

# Startup file and linker script 
STARTUP_CODE	:= ./driver/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h750xx.S
LINKER_SCRIPT	:= ./driver/STM32H750VBTX_FLASH.ld

# Sorthand and build definititions
INCLUDES			:= -I$(MAIN_INC) -I$(CMSIS_DEV_INC) -I$(CMSIS_INC) -I$(HAL_INC) \
						-I$(RTOS_INC) -I$(RTOS_GCC_INC)
DEFINES			:= -D$(DEVICE_FAMILY) -D$(DEVICE_MODEL) -D$(DEVICE_VARIANT) \
						-DUSE_HAL_DRIVER
DRIVER_SRCS		:= $(HAL_SRC) $(SYSTEM_SRC) $(RTOS_SRC) $(RTOS_GCC_SRC) $(RTOS_MEM_SRC)
DRIVER_OBJS		:= $(notdir $(patsubst %.c,%.o,$(DRIVER_SRCS))) startup_stm32h750xx.o
MAIN_OBJS		:= $(notdir $(patsubst %.c,%.o,$(MAIN_SRC)))

CFLAGS			:= $(CORTEX_FLAGS) $(COMMON_FLAGS) $(DEFINES) $(INCLUDES)
AFLAGS			:= $(CORTEX_FLAGS) $(AS_FLAGS)
LDFLAGS			:= $(CORTEX_FLAGS) -T $(LINKER_SCRIPT) \
						-Wl,--gc-sections,--relax --specs=nano.specs --specs=nosys.specs \
				   	-Wl,--start-group -lc -lm -Wl,--end-group

# Output definitions
FIRMWARE_ELF	:= ./bin/firmware.elf
FIRMWARE_BIN	:= ./bin/firmware.bin
LIBRARY			:= ./lib/lib$(DEVICE_FAMILY).a

.PHONY: lib firmware

lib:
	@echo "Compiling the source files..."
	$(CC) $(AFLAGS) -c $(STARTUP_CODE)
	$(CC) $(CFLAGS) -c $(DRIVER_SRCS)

	@echo "\nArchiving the object files..."
	$(AR) $(LIBRARY) $(DRIVER_OBJS)

	@echo "\nRemoving the object files..."
	$(RM) $(DRIVER_OBJS)

firmware:
	@echo "Compiling the source files..."
	$(CC) -g3 $(CFLAGS) -c $(MAIN_SRC)

	@echo "\nLinking the object files..."
	$(CC) $(LDFLAGS) $(MAIN_OBJS) -o $(FIRMWARE_ELF) -l$(DEVICE_FAMILY) -L./lib
	$(OBJCOPY) -O binary $(FIRMWARE_ELF) $(FIRMWARE_BIN)

	@echo "\nRemoving the object files..."
	$(RM) $(MAIN_OBJS)

	@echo "\nThe firmware section sizes:"
	@$(SIZE) $(FIRMWARE_ELF)

	@echo "\nThe firmware binary format:"
	@$(FILE) $(FIRMWARE_ELF)