CC = gcc
CFLAGS = -Wall -g

TARGET = shell

SRCS = main.c scan_input.c commands.c linked_list.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)