CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lncurses
TARGET = traffic_sim
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) traffic_sim.log

run: all
	./$(TARGET)

.PHONY: all clean run