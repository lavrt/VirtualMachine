# Переменные
CC = g++
CFLAGS = -I../Include -I../Stack -I. -Wall -g
SRC = asm_main.cpp asm_interpreter.cpp asm_labels.cpp ../Stack/stack.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = my_asm

# Правило по умолчанию
all: $(TARGET)

# Линковка
$(TARGET): $(OBJ)
	$(CC) -o $@ $^

# Компиляция объектов
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка скомпилированных файлов
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
