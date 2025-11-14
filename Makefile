CXX = g++
CXXFLAGS = -std=c++17 -O2 -g -Iinclude -Iexternal/imgui -Iexternal/glm -Iexternal/stb

SRC_DIR = src
EXTERNAL_DIR = external/imgui
BUILD_DIR = build
DIST_DIR = dist

# gather all source files
SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c) $(wildcard $(EXTERNAL_DIR)/*.cpp)

# linux build
LINUX_BUILD_DIR = $(BUILD_DIR)/linux
LINUX_OBJ = $(patsubst %.cpp,$(LINUX_BUILD_DIR)/%.o,$(SRC))
LINUX_LDFLAGS = -lglfw -ldl -lGL
LINUX_OUT = $(LINUX_BUILD_DIR)/app

linux: $(LINUX_OUT)

$(LINUX_OUT): $(LINUX_OBJ)
	$(CXX) $(LINUX_OBJ) -o $@ $(LINUX_LDFLAGS)

$(LINUX_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# windows cross compile
WIN_CXX = x86_64-w64-mingw32-g++
WIN_CXXFLAGS = -std=c++17 -O2 -g -Iinclude -Iexternal/imgui -Iexternal/glm -Iexternal/stb
WIN_LDFLAGS = -Llib/win -lglfw3 -lopengl32 -lgdi32 -luser32

WIN_BUILD_DIR = $(BUILD_DIR)/win
WIN_OBJ = $(patsubst %.cpp,$(WIN_BUILD_DIR)/%.o,$(SRC))
WIN_OUT = $(WIN_BUILD_DIR)/app.exe

win: $(WIN_OUT)

$(WIN_OUT): $(WIN_OBJ)
	$(WIN_CXX) $(WIN_OBJ) -o $@ $(WIN_LDFLAGS)

$(WIN_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(WIN_CXX) $(WIN_CXXFLAGS) -c $< -o $@

# distribution
DIST_LINUX = $(DIST_DIR)/linux
DIST_WIN = $(DIST_DIR)/win

dist-linux: linux
	@mkdir -p $(DIST_LINUX)
	cp $(LINUX_OUT) $(DIST_LINUX)/
	cp -r assets $(DIST_LINUX)/
	cp -r shaders $(DIST_LINUX)/

dist-win: win
	@mkdir -p $(DIST_WIN)
	cp $(WIN_OUT) $(DIST_WIN)/
	cp -r assets $(DIST_WIN)/
	cp -r shaders $(DIST_WIN)/

# combined dist target
dist: dist-linux dist-win

# run linux build as default
run: linux
	$(LINUX_OUT)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
