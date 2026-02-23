# Passive Acoustic Surveillance System

CC				:= gcc
PIO			:= pio
CFLAGS		:= -Wall -std=c17 -g3 -O3 -march=native

SRC			:= ./src/*.h ./src/*.c
DEPENDS		:= gtk4 libadwaita-1 shumate-1.0
CONFIG		:= $(shell pkg-config --cflags --libs $(DEPENDS)) -lm -lsqlite3 -ldsp -lgsl -L./lib
PROGRAM		:= SONAR

FIRMWARE		:= firmware.elf
FRM_DIR		:= ./firmware/.pio/build/genericSTM32H750VB

OPENOCD		:= openocd 
INTERFACE	:= interface/stlink.cfg
TARGET		:= target/stm32h7x.cfg
COMMAND		:= "program $(FIRMWARE) verify reset exit"

.PHONY: firmware station firmware_remove

# Building and flashing the firmware
firmware:
	@echo "Building embedded firmware..."
	@cd ./firmware ; $(PIO) run ; cd ..
	@echo " "
	@echo "Uploading embedded firmware..."
	@cd $(FRM_DIR) ; openocd -f $(INTERFACE) -f $(TARGET) -c $(COMMAND)
	
# Building and running the ground station program
station:
	@echo "Building ground station..."
	$(CC) $(SRC) -o ./$(PROGRAM) $(CFLAGS) $(CONFIG)
	@echo " "
	@echo "Running ground station..."
	@./$(PROGRAM)

# Remove the old firmware
firmware_remove:
	@echo "Removing the old firmware..."
	@cd ./firmware ; $(PIO) run -t clean ; cd ..
