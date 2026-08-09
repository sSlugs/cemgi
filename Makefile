CC = gcc

# development (no Wall or Wextra bc the "unused include" is far too annoying)
#CFLAGS = -Iinclude -g -O0

# release
CFLAGS = -Wall -Wextra -Iinclude -g -O3

SRC_DIR = src
BUILD_DIR = build

# Find every .c file in src and subfolders
SRC = $(shell find $(SRC_DIR) -name "*.c")

# Convert src/file.c -> build/file.o
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = cemgi


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
