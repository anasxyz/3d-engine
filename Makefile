CXX = g++
CXXFLAGS = -std=c++17 -O2 -g -Iinclude
LDFLAGS = -lglfw -ldl -lGL

SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c)
OUT = app

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
