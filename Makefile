# Passive Acoustic Surveillance System

CC				:= gcc
PIO			:= pio
VALGRIND		:= valgrind
CPPCHECK		:= cppcheck

CFLAGS		:= -Wall -std=c17 -g -O0 -march=native

SRC			:= ./src/*.h ./src/*.c
DEPENDS		:= sqlite3 gsl gtk4 libadwaita-1 shumate-1.0
CONFIG		:= -ldsp -L./lib $(shell pkg-config --cflags --libs $(DEPENDS))

VAL_CONFIG	:= --leak-check=full --track-origins=yes --show-leak-kinds=all \
				   --suppressions=./valgrind.supp
VAL_OUTPUT	:= ./logs/valgrind.log

CPP_CONFIG	:= --enable=warning,style,portability --std=c23 --addon=misra
CPP_OUTPUT	:= ./logs/cppcheck.log

PROGRAM		:= SONAR

FIRMWARE		:= firmware.elf
FRM_DIR		:= ./firmware/.pio/build/genericSTM32H750VB

OPENOCD		:= openocd 
INTERFACE	:= interface/stlink.cfg
TARGET		:= target/stm32h7x.cfg
COMMAND		:= "program $(FIRMWARE) verify reset exit"

.PHONY: firmware station memcheck analysis

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

# Run the valgrind to check for memory leaks
memcheck:
	@echo "Checking for memory leaks...\n"
	@$(VALGRIND) $(VAL_CONFIG) --log-file=$(VAL_OUTPUT) ./$(PROGRAM)
	@echo "Completed the memory leak check, look at $(VAL_OUTPUT)"

# Analyze the program codebase with cppcheck
analysis:
	@echo "Running the static code analysis..."
	@$(CPPCHECK) $(CPP_CONFIG) --output-file=$(CPP_OUTPUT) ./src
	@echo "Completed the static code analysis, look at $(CPP_OUTPUT)"
