EXEC=prog
CXX = g++
SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp | wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CPPFLAGS = -Wall -O3 -pedantic -Wfatal-errors -Wconversion -Wredundant-decls -Wshadow -Wall -Wextra
# CPPFLAGS = -Wall -pedantic -Wfatal-errors -Wconversion -Wredundant-decls -Wshadow -Wall -Wextra # without -O3
CXXFLAGS := -std=c++11
.PHONY = clean

all: aan_reach

run: aan_reach
	./aan_reach

#main: $(OBJ_FILES)
#	$(CXX) $(LDFLAGS) -o $@ $^

aan_reach: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o aan_reach $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm obj/*/*.o obj/*.o ./aan_reach
