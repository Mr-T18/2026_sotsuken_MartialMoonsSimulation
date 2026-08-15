# コンパイラとフラグ設定
CXX      := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra

# ディレクトリ設定
SRC_DIR  := src
BUILD_DIR:= build
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/simulation

# ソースファイルとオブジェクトファイルの自動取得
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# デフォルトターゲット
all: $(TARGET)

# 実行ファイルのリンク
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 各.cppのコンパイル
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# ディレクトリ作成
$(BUILD_DIR) $(BIN_DIR):
	mkdir $@

# 生成物の削除
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) result.dat

# ビルドして実行
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run