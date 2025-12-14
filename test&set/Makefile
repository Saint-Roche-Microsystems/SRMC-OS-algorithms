# Makefile para Test & Set - Exclusión Mutua

CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
TARGET = test_and_set

all: $(TARGET)

$(TARGET): test_and_set.c
	$(CC) $(CFLAGS) -o $(TARGET) test_and_set.c

# Compilar con información de debug
debug: test_and_set.c
	$(CC) $(CFLAGS) -g -O0 -o $(TARGET)_debug test_and_set.c

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(TARGET)_debug

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

.PHONY: all debug clean run

