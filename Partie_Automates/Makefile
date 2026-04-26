CC = gcc
CFLAGS = -Wall -Wextra -fexec-charset=UTF-8

TARGET = Tamagoshi
OBJ = Tamagoshi.o Automate.o

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

Tamagoshi.o: Tamagoshi.c Automate.h
	$(CC) $(CFLAGS) -c Tamagoshi.c

Automate.o: Automate.c Automate.h
	$(CC) $(CFLAGS) -c Automate.c

clean:
	rm -f $(OBJ) $(TARGET)
