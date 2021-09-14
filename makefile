OUTPUT = run
CC = gcc
CFLAGS = -g -Wall
SOURCES = $(shell find . -name '*.c' | sort -k 1nr | cut -f2-)
OBJECT_FILES = $(SOURCES:%.c=%.o)
DIRS = Configuration Handler Neighbor Node Parser Protocol Reactor Relay Route
INCLUDES = $(patsubst %, -I%, $(DIRS))


.PHONY: all clean
all: output

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

output: $(OBJECT_FILES)
	$(CC) $^ -o $(OUTPUT)

clean:
	rm ${OUTPUT}
	find . -name '*.o' -delete