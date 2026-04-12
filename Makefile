
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g
LDFLAGS :=

BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

SRCS := catshell.c readline/cat_read_line.c commands/utils.c arts/banner.c readline/parser.c
TARGET := catshell
TARGET_BIN := $(BIN_DIR)/$(TARGET)

OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all dirs run clean

all: dirs $(TARGET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(dir $(OBJS))

$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile C source into obj folder and generate dependency files (.d)
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include generated dependency files if they exist
-include $(DEPS)

run: all
	./$(TARGET_BIN)

clean:
	rm -rf $(BUILD_DIR)
