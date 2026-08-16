# コンパイラとフラグ設定
CXX      := g++
# -MMD -MP でヘッダーの依存関係ファイル (.d) を自動生成
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -MMD -MP

# ディレクトリ設定
SRC_DIR  := src
BUILD_DIR:= build
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/simulation

# ソースファイル、オブジェクトファイル、依存関係ファイルの取得
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS     := $(OBJS:.o=.d)

# デフォルトターゲット
all: $(TARGET)

# 実行ファイルのリンク
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 各.cppのコンパイル
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# 自動生成された依存関係ファイルを読み込む
-include $(DEPS)

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