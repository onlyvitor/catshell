CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -g -I.
LDFLAGS :=

BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

CXX_SRCS := catshell.cpp readline/reader.cpp readline/parser.cpp utils/arts/banner.cpp utils/utils.cpp utils/exec.cpp commands/echo.cpp commands/exit.cpp commands/env.cpp
TARGET := catshell
TARGET_BIN := $(BIN_DIR)/$(TARGET)

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CXX_SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all dirs run clean

all: dirs $(TARGET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(dir $(OBJS))

$(TARGET_BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET_BIN)

clean:
	rm -rf $(BUILD_DIR)
