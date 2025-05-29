CXX = g++
SRC_DIR = src
OBJ_DIR = libs
SRC_FILES = Game.cpp SDL_ext.cpp services.cpp
OBJMODULES = $(addprefix $(OBJ_DIR)/,$(SRC_FILES:.cpp=.o))
FLAGS = -Wall -g
LIBS_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf


build: main.cpp $(OBJMODULES)
	$(CXX) $(FLAGS) $^ $(LIBS_FLAGS) -o main

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o build main
