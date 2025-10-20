# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I./include -pthread
LDFLAGS = -pthread 

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Target executable
TARGET = redis_server

# Source files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/RESP.cpp $(SRC_DIR)/client.cpp $(SRC_DIR)/server.cpp $(SRC_DIR)/ioFull.cpp

# Object files
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/RESP.o $(OBJ_DIR)/client.o $(OBJ_DIR)/server.o $(OBJ_DIR)/ioFull.o

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile main.cpp
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/RESP.h $(INCLUDE_DIR)/client.h $(INCLUDE_DIR)/server.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

# Compile RESP.cpp
$(OBJ_DIR)/RESP.o: $(SRC_DIR)/RESP.cpp $(INCLUDE_DIR)/RESP.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/RESP.cpp -o $(OBJ_DIR)/RESP.o

# Compile client.cpp
$(OBJ_DIR)/client.o: $(SRC_DIR)/client.cpp $(INCLUDE_DIR)/client.h $(INCLUDE_DIR)/RESP.h $(INCLUDE_DIR)/ioFull.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/client.cpp -o $(OBJ_DIR)/client.o

# Compile server.cpp
$(OBJ_DIR)/server.o: $(SRC_DIR)/server.cpp $(INCLUDE_DIR)/server.h $(INCLUDE_DIR)/RESP.h $(INCLUDE_DIR)/ioFull.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/server.cpp -o $(OBJ_DIR)/server.o

# Compile ioFull.cpp
$(OBJ_DIR)/ioFull.o: $(SRC_DIR)/ioFull.cpp $(INCLUDE_DIR)/ioFull.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ioFull.cpp -o $(OBJ_DIR)/ioFull.o

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run