CXX = g++
CXXFLAGS = -std=c++17 -O2 -g -Iinclude -Iexternal/imgui
LDFLAGS = -lglfw -ldl -lGL -lassimp

SRC_DIR = src
EXTERNAL_DIR = external/imgui
BUILD_DIR = build

# get all .cpp and .c files
SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c) $(wildcard $(EXTERNAL_DIR)/*.cpp)

OBJ = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC))

OUT = app

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# compile rule: build/ mirrors src/ and external/imgui/ structure
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(OUT)
	./$(OUT)

clean:
	rm -rf $(BUILD_DIR) $(OUT)
