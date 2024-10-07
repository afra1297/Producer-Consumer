# Compiler 
CXX = gcc

# Compiler flags
FLAGS = -g -pthread -Iinclude

# Executable file name
TARGET = ProdConsOut

# Source files
SRCS = src/main.c src/BoundedBuffer.c src/Producer.c src/Dispatcher.c src/UnboundedBuffer.c src/CoEditor.c src/ScreenManager.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build executable file
$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) -o $(TARGET) $(OBJS)

# Build object files
%.o: %.c
	$(CXX) $(FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# PHONY targets
.PHONY: clean