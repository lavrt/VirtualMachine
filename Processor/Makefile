# Переменные
CC = g++
CFLAGS = -I../Include -I. -I../Stack -Wall -g
SRC = main.cpp processor.cpp ../Stack/stack.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = my_processor

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
