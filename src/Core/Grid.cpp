#include "Grid.h"
#include <cstdlib>
#include <ctime> // Thêm thư viện này để dùng hàm time(0)
#include <vector>

Grid::Grid(int size) {
    this->gridSize = size;
    this->score = 0;

    // Ép kiểu (unsigned int) để triệt tiêu cảnh báo C4244
    srand((unsigned int)time(0));

    // Cấp phát bộ nhớ
    board = new Tile * *[gridSize];
    for (int i = 0; i < gridSize; i++) {
        board[i] = new Tile * [gridSize];
        for (int j = 0; j < gridSize; j++) {
            board[i][j] = new Tile(0, i, j);
        }
    }

    spawnRandomTile();
    spawnRandomTile();
}

Grid::~Grid() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            delete board[i][j];
        }
        delete[] board[i];
    }
    delete[] board;
}

void Grid::spawnRandomTile() {
    std::vector<std::pair<int, int>> emptyCells;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) {
                emptyCells.push_back({ i, j });
            }
        }
    }

    if (emptyCells.empty()) return;

    int randomIndex = rand() % emptyCells.size();
    int row = emptyCells[randomIndex].first;
    int col = emptyCells[randomIndex].second;

    int randomChance = rand() % 100;
    int newValue = (randomChance < 90) ? 2 : 4;
    board[row][col]->setValue(newValue);
}

bool Grid::pushLine(Tile** line) {
    bool moved = false;
    int insertPos = 0;

    for (int i = 0; i < gridSize; i++) {
        if (line[i]->getValue() != 0 && line[i]->getValue() != -1) {
            if (i != insertPos) {
                if (line[insertPos]->getValue() == -1) {
                    insertPos++;
                    i--;
                    continue;
                }
                line[insertPos]->setValue(line[i]->getValue());
                line[i]->setValue(0);
                moved = true;
            }
            insertPos++;
        }
    }

    for (int i = 0; i < gridSize - 1; i++) {
        int currentVal = line[i]->getValue();
        int nextVal = line[i + 1]->getValue();

        if (currentVal != 0 && currentVal != -1 && currentVal == nextVal) {
            line[i]->setValue(currentVal * 2);
            score += currentVal * 2;
            line[i + 1]->setValue(0);
            moved = true;

            for (int j = i + 1; j < gridSize - 1; j++) {
                line[j]->setValue(line[j + 1]->getValue());
            }
            line[gridSize - 1]->setValue(0);
        }
    }
    return moved;
}

void Grid::shiftLeft() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile * [gridSize];
        for (int j = 0; j < gridSize; j++) {
            row[j] = board[i][j];
        }
        if (pushLine(row)) moved = true;
        delete[] row;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftRight() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile * [gridSize];
        for (int j = 0; j < gridSize; j++) {
            row[j] = board[i][gridSize - 1 - j];
        }
        if (pushLine(row)) moved = true;
        delete[] row;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftUp() {
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        Tile** col = new Tile * [gridSize];
        for (int i = 0; i < gridSize; i++) {
            col[i] = board[i][j];
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftDown() {
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        Tile** col = new Tile * [gridSize];
        for (int i = 0; i < gridSize; i++) {
            col[i] = board[gridSize - 1 - i][j];
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

bool Grid::checkGameOver() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) return false;
        }
    }

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 1; j++) {
            int current = board[i][j]->getValue();
            if (current != -1 && current == board[i][j + 1]->getValue()) return false;
            if (current != -1 && current == board[j][i]->getValue()) return false;
        }
    }

    return true;
}

// Giữ nguyên các hàm Getter với từ khóa const y hệt code gốc của bạn
int Grid::getScore() const { return score; }
int Grid::getTileValue(int row, int col) const { return board[row][col]->getValue(); }
int Grid::getSize() const { return gridSize; }
