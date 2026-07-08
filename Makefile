CXX = g++
WINDRES = windres
.SILENT:

# Flags for compilation (C++17 standard, SFML include path)
CXXFLAGS = -std=c++17 -Wall -I"SFML-3.0.2/include" -I"include"

# Flags for linking (SFML library path and modules)
LDFLAGS = -L"SFML-3.0.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

# Files
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, bin/%.o, $(SRC)) bin/resource.o
EXEC = bin/fizz-rush.exe

# Main rule (runs when you just type `make`)
all: $(EXEC)

# Build and run the game
run: all
	./$(EXEC)

# Link the final .exe
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compile any .cpp to .o
bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile resource.o from resource.rc (icon)
bin/resource.o: src/resource.rc
	$(WINDRES) $< -O coff -o $@

# Clean compiled files (run: make clean)
clean:
	del /Q bin\*.o bin\*.exe
