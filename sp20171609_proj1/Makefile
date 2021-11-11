CC = gcc
OBJS = 20171609.o
TARGET = 20171609.out

.SUFFIXES : .c .o

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -Wall -o $@ $(OBJS)

clean :
	rm -f $(OBJS) $(TARGET)

20171609.out : 20171609.c 20171609.h
