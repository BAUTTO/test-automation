# Testsuite target.
TARGET := testsuite

# Source directory.
SOURCE_DIR := ../source

# Source files - update this list as new source files are added to the system.
SOURCE_FILES := $(SOURCE_DIR)/arch/test/hw_platform.cpp \
                $(SOURCE_DIR)/driver/adc/atmega328p.cpp \
                $(SOURCE_DIR)/driver/eeprom/atmega328p.cpp \
                $(SOURCE_DIR)/driver/gpio/atmega328p.cpp \
                $(SOURCE_DIR)/driver/serial/atmega328p.cpp \
                $(SOURCE_DIR)/driver/tempsensor/smart.cpp \
                $(SOURCE_DIR)/driver/tempsensor/tmp36.cpp \
                $(SOURCE_DIR)/driver/timer/atmega328p.cpp \
                $(SOURCE_DIR)/driver/watchdog/atmega328p.cpp \
                $(SOURCE_DIR)/logic/logic.cpp \
                $(SOURCE_DIR)/ml/lin_reg/fixed.cpp \
                $(SOURCE_DIR)/utils/utils.cpp \

# Test files - update this list as new test files are added to the system.
TEST_FILES := driver/adc/atmega328p_test.cpp \
              driver/eeprom/atmega328p_test.cpp \
              driver/gpio/atmega328p_test.cpp \
              driver/serial/atmega328p_test.cpp \
              driver/tempsensor/smart_test.cpp \
              driver/tempsensor/tmp36_test.cpp \
              driver/timer/atmega328p_test.cpp \
              driver/watchdog/atmega328p_test.cpp \
              logic/logic_test.cpp \
              ml/lin_reg/fixed_test.cpp \
              testsuite.cpp \

# All files.
ALL_FILES := $(SOURCE_FILES) $(TEST_FILES)

# Main include directory.
INC_DIR := ../include

# Gtest directory.
GTEST_DIR := /usr/include

# C++ compiler.
CXX_COMPILER = g++

# C++ compiler flags.
CXX_FLAGS = -std=c++17 -Werror -Wall -I$(INC_DIR) -I$(GTEST_DIR) -DTESTSUITE -DLECTURE1

# Linked libraries.
LINK_LIBS = -lgtest -lgmock -lgtest_main -lpthread

# Build and run the test suite as default:
default: build run

# Build the test suite.
build:
	@$(CXX_COMPILER) $(ALL_FILES) -o $(TARGET) $(CXX_FLAGS) $(LINK_LIBS)

# Run the test suite.
run:
	@./$(TARGET)

# Clean the test suite.
clean:
	@rm -f $(TARGET)
