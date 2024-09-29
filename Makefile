CC = gcc
CFLAGS = -lncurses -lm  # Link ncurses and math libraries
TARGET = earth

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o $(CFLAGS)

$(TARGET).o: $(TARGET).c
	$(CC) -c $(TARGET).c

clean:
	rm -f $(TARGET).o $(TARGET)
