# Unit Tests

CC					:= gcc
CFLAGS			:= -Wall -std=c17 -O3 -march=native

DEPENDS			:= check sqlite3 gsl gtk4 libadwaita-1 shumate-1.0
CONFIG			:= -ldsp -L../../lib $(shell pkg-config --cflags --libs $(DEPENDS))

COMMON_SRC		:= $(filter-out ../../src/main.c, $(wildcard ../../src/*.c))

ANALYSIS_SRC	:= $(COMMON_SRC) ./test-analysis.c
ANALYSIS_OUT	:= ./test-analysis

.PHONY			: test-analysis

test-analysis:
	$(CC) $(ANALYSIS_SRC) -o $(ANALYSIS_OUT) $(CFLAGS) $(CONFIG)

