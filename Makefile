# Directories
SRC_DIR = ./src
IMGUI_DIR = ./lib/imgui
OBJ_DIR = obj

# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -O3

# Source files
SRC = $(SRC_DIR)/main.cpp \
      $(IMGUI_DIR)/imgui.cpp \
      $(IMGUI_DIR)/imgui_draw.cpp \
      $(IMGUI_DIR)/imgui_widgets.cpp \
      $(IMGUI_DIR)/imgui_tables.cpp \
      $(IMGUI_DIR)/imgui_demo.cpp \
      $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
      $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Object files in the obj directory
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Output executable
TARGET = main
LIBS = -lglfw -lGL -lGLEW

# Default target (build and run the project)
all: $(TARGET)
	@echo "Build successful. Running the program..."
	@./$(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

# Rule to build object files in the obj directory
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf $(OBJ_DIR)

# Phony targets (not real files)
.PHONY: all clean
