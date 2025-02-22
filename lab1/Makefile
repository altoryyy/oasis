CC=gcc
CFLAGS=-std=c11 -pedantic -Wall

TARGET=dirwalk

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f $(TARGET)