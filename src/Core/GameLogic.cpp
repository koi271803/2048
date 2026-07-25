#include "Grid.h"
#include <cstdlib>
#include <ctime>

// 1. Hàm khởi tạo
Grid::Grid(int size) {
    gridSize = size;
    score = 0;
    isGameOver = false;
    
    // Cấp phát mảng 2 chiều
    board.resize(gridSize, std::vector<Tile*>(gridSize, nullptr));
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            board[i][j] = new Tile(0, i, j);
        }
    }
    
    srand(time(0));
    spawnRandomTile();
    spawnRandomTile();
}

// 2. Hàm hủy (tránh rò rỉ bộ nhớ)
Grid::~Grid() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            delete board[i][j];
        }
    }
}

// 3. Hàm sinh số ngẫu nhiên (Giống randomtile trong ảnh)
void Grid::spawnRandomTile() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) {
                emptyCells.push_back({i, j});
            }
        }
    }
    if (emptyCells.empty()) return;

    int randomIndex = rand() % emptyCells.size();
    int row = emptyCells[randomIndex].first;
    int col = emptyCells[randomIndex].second;
    
    int newValue = (rand() % 10 < 9) ? 2 : 4;
    board[row][col]->setValue(newValue);
}

// 4. Hàm cốt lõi: Dồn và gộp 1 hàng/cột (Giống hàm push trong ảnh)
void Grid::pushLine(std::vector<int>& line) {
    // Bước 1: Dồn các số khác 0 sát vào nhau
    std::vector<int> temp;
    for (int val : line) {
        if (val != 0) temp.push_back(val);
    }

    // Bước 2: Gộp các số giống nhau
    for (size_t i = 0; i < temp.size(); i++) {
        if (i + 1 < temp.size() && temp[i] == temp[i + 1]) {
            temp[i] *= 2;
            score += temp[i]; // Cộng điểm
            temp[i + 1] = 0;
        }
    }

    // Bước 3: Dồn lại lần cuối và cập nhật mảng gốc
    line.assign(line.size(), 0);
    int insertPos = 0;
    for (int val : temp) {
        if (val != 0) {
            line[insertPos++] = val;
        }
    }
}

// 5. Thao tác trượt TRÁI
void Grid::shiftLeft() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        std::vector<int> line(gridSize);
        for (int j = 0; j < gridSize; j++) line[j] = board[i][j]->getValue();
        
        std::vector<int> original = line;
        pushLine(line);
        
        for (int j = 0; j < gridSize; j++) {
            if (original[j] != line[j]) moved = true;
            board[i][j]->setValue(line[j]);
        }
    }
    if (moved) spawnRandomTile();
}

// 6. Thao tác trượt PHẢI
void Grid::shiftRight() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        std::vector<int> line(gridSize);
        for (int j = 0; j < gridSize; j++) line[j] = board[i][gridSize - 1 - j]->getValue();
        
        std::vector<int> original = line;
        pushLine(line);
        
        for (int j = 0; j < gridSize; j++) {
            if (original[j] != line[j]) moved = true;
            board[i][gridSize - 1 - j]->setValue(line[j]);
        }
    }
    if (moved) spawnRandomTile();
}

// 7. Thao tác trượt LÊN
void Grid::shiftUp() {
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        std::vector<int> line(gridSize);
        for (int i = 0; i < gridSize; i++) line[i] = board[i][j]->getValue();
        
        std::vector<int> original = line;
        pushLine(line);
        
        for (int i = 0; i < gridSize; i++) {
            if (original[i] != line[i]) moved = true;
            board[i][j]->setValue(line[i]);
        }
    }
    if (moved) spawnRandomTile();
}

// 8. Thao tác trượt XUỐNG
void Grid::shiftDown() {
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        std::vector<int> line(gridSize);
        for (int i = 0; i < gridSize; i++) line[i] = board[gridSize - 1 - i][j]->getValue();
        
        std::vector<int> original = line;
        pushLine(line);
        
        for (int i = 0; i < gridSize; i++) {
            if (original[i] != line[i]) moved = true;
            board[gridSize - 1 - i][j]->setValue(line[i]);
        }
    }
    if (moved) spawnRandomTile();
}

// 9. Kiểm tra điều kiện thua
bool Grid::checkGameOver() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) return false;
            if (j < gridSize - 1 && board[i][j]->getValue() == board[i][j + 1]->getValue()) return false;
            if (i < gridSize - 1 && board[i][j]->getValue() == board[i + 1][j]->getValue()) return false;
        }
    }
    isGameOver = true;
    return true;
}

// 10. Các hàm Getter
int Grid::getScore() const { return score; }
int Grid::getTileValue(int row, int col) const { return board[row][col]->getValue(); }
int Grid::getSize() const { return gridSize; }
}
