CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -g -I.
LD := $(CXX)
LDFLAGS :=

BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

C_SRCS :=
CXX_SRCS := catshell.cpp readline/reader.cpp readline/parser.cpp utils/arts/banner.cpp utils/utils.cpp utils/exec.cpp commands/echo.cpp commands/exit.cpp commands/env.cpp
SRCS := $(C_SRCS) $(CXX_SRCS)
TARGET := catshell
TARGET_BIN := $(BIN_DIR)/$(TARGET)

OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS)) $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CXX_SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all dirs run clean

all: dirs $(TARGET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(dir $(OBJS))

$(TARGET_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET_BIN)

clean:
	rm -rf $(BUILD_DIR)
