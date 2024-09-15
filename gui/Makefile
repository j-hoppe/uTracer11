# Define the source directories
SRC_DIRS := ../gui ../common ../CSerialPort/src ../Pdp11BusCycleDisas
BIN_DIR := ./bin

# Define the source files for ../CSerialPort/src directory
CSerialPort_SRC := SerialPortInfo.cpp SerialPortInfoBase.cpp SerialPortInfoUnixBase.cpp \
                   SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp
CSerialPort_OBJS := $(addprefix $(BIN_DIR)/CSerialPort/,$(CSerialPort_SRC:.cpp=.o))

# Define the source files for other directories
GUI_SRC := $(wildcard ../gui/*.cpp)
COMMON_SRC := csvreader.cpp memoryimage.cpp messages.cpp utils.cpp
# COMMON_SRC := $(wildcard ../common/*.cpp)
PDP11_SRC := $(wildcard ../Pdp11BusCycleDisas/*.cpp)

# Define the object files for each source directory
GUI_OBJS := $(addprefix $(BIN_DIR)/guiobjs/,$(notdir $(GUI_SRC:.cpp=.o)))
COMMON_OBJS := $(addprefix $(BIN_DIR)/common/,$(notdir $(COMMON_SRC:.cpp=.o)))
PDP11_OBJS := $(addprefix $(BIN_DIR)/Pdp11BusCycleDisas/,$(notdir $(PDP11_SRC:.cpp=.o)))

# Combine all object files
OBJS := $(CSerialPort_OBJS) $(GUI_OBJS) $(COMMON_OBJS) $(PDP11_OBJS)

# Define the compiler
CXX := g++

# Define compiler flags
# -Wall: Enable all compiler's warning messages
# -O2: Optimize the code
# wx-config --cxxflags: Get the flags for wxWidgets
CXXFLAGS := -Wall -Wextra -g $(shell wx-config --cxxflags)
# -std=c++17

# Define include directories
# -I.: Include headers from the current directory
# -I../common: Include headers from ../common directory
# -I../CSerialPort/include: Include headers from ../CSerialPort/include directory
# -I../Pdp11BusCycleDisas: Include headers from ../Pdp11BusCycleDisas directory
INCLUDE_DIRS := -I. -I../common -I../CSerialPort/include -I../Pdp11BusCycleDisas -I../gui

# Define the linker flags
# wx-config --libs: Get the linker flags for wxWidgets
LDFLAGS := $(shell wx-config --libs)

# Default target
all: $(BIN_DIR)/gui

# Pattern rule to compile .cpp to .o for the ../gui directory
$(BIN_DIR)/guiobjs/%.o: ../gui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Pattern rule to compile .cpp to .o for the ../common directory
$(BIN_DIR)/common/%.o: ../common/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Pattern rule to compile .cpp to .o for the ../CSerialPort/src directory
$(BIN_DIR)/CSerialPort/%.o: ../CSerialPort/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Pattern rule to compile .cpp to .o for the ../Pdp11BusCycleDisas directory
$(BIN_DIR)/Pdp11BusCycleDisas/%.o: ../Pdp11BusCycleDisas/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Link object files to create the executable
# $^: All prerequisites
$(BIN_DIR)/gui: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Clean target
clean:
	rm -rf $(BIN_DIR)/guiobjs/*.o $(BIN_DIR)/common/*.o $(BIN_DIR)/CSerialPort/*.o $(BIN_DIR)/Pdp11BusCycleDisas/*.o $(BIN_DIR)/gui

.PHONY: all clean
