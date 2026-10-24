CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell

SRCS = main.c \
       basicreq.c \
       hop.c \
       input.c \
       reveal.c \
       logs.c \
       execut.c \
       othercom.c \
       proclore.c \
       seek.c \
       myshrc.c \
       io.c \
       pipes.c \
       activities.c \
       ping.c \
       fgandbg.c \
       iMan.c \
       neonate.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) programming myprogram test_shell
