#include "Grid.h"
#include <cstdlib>
#include <ctime>   
#include <vector>

// ==============================================================================
// 1. CONSTRUCTOR (HÀM KHỞI TẠO)
// ==============================================================================
Grid::Grid(int size) {
    this->gridSize = size;
    this->score = 0;
    
    // Khởi tạo số lượng Power-ups mặc định khi New Game
    undoCount = 2; // Cho sẵn 2 lượt Undo lúc bắt đầu
    swapCount = 0;
    deleteCount = 0;
    canUndo = false;
    prevScore = 0;
    
    srand((unsigned int)time(0)); 
    
    // Cấp phát bộ nhớ cho Bàn cờ chính và Bàn cờ lưu trạng thái (Undo)
    board = new Tile**[gridSize];
    prevBoard = new int*[gridSize]; 
    
    for (int i = 0; i < gridSize; i++) {
        board[i] = new Tile*[gridSize];
        prevBoard[i] = new int[gridSize];
        for (int j = 0; j < gridSize; j++) {
            board[i][j] = new Tile(0, i, j); 
            prevBoard[i][j] = 0; // Khởi tạo mảng lưu trạng thái bằng 0
        }
    }
    
    spawnRandomTile();
    spawnRandomTile();
}

// ==============================================================================
// 2. DESTRUCTOR (HÀM HỦY)
// ==============================================================================
Grid::~Grid() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            delete board[i][j];
        }
        delete[] board[i];
        delete[] prevBoard[i]; // Giải phóng bộ nhớ mảng Undo
    }
    delete[] board;
    delete[] prevBoard;
}

// ==============================================================================
// 3. HÀM SINH SỐ NGẪU NHIÊN (2 hoặc 4)
// ==============================================================================
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

    int randomChance = rand() % 100;
    int newValue = (randomChance < 90) ? 2 : 4;
    board[row][col]->setValue(newValue);
}

// ==============================================================================
// 4. HÀM LƯU TRẠNG THÁI (DÙNG CHO UNDO)
// ==============================================================================
void Grid::saveState() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            // Lưu lại giá trị của toàn bộ bàn cờ hiện tại vào prevBoard
            prevBoard[i][j] = board[i][j]->getValue();
        }
    }
    prevScore = score;
    canUndo = true; // Bật cờ cho phép Undo
}

// ==============================================================================
// 5. HÀM LÕI: THUẬT TOÁN DỒN VÀ GỘP SỐ (KÈM NHẬN THƯỞNG POWER-UPS)
// ==============================================================================
bool Grid::pushLine(Tile** line) {
    bool moved = false;
    int insertPos = 0;
    
    // BƯỚC 1: DỒN SỐ
    for (int i = 0; i < gridSize; i++) {
        if (line[i]->getValue() != 0) {
            if (i != insertPos) {
                line[insertPos]->setValue(line[i]->getValue());
                line[i]->setValue(0);
                moved = true;
            }
            insertPos++;
        }
    }
    
    // BƯỚC 2: GỘP SỐ VÀ NHẬN THƯỞNG
    for (int i = 0; i < gridSize - 1; i++) {
        int currentVal = line[i]->getValue();
        int nextVal = line[i+1]->getValue();
        
        if (currentVal != 0 && currentVal == nextVal) {
            int mergedValue = currentVal * 2; // Sinh ra số mới
            
            line[i]->setValue(mergedValue);
            score += mergedValue;
            
            // --- LOGIC NHẬN THƯỞNG POWER-UPS TẠI ĐÂY ---
            if (mergedValue == 64) {
                undoCount++;
            } else if (mergedValue == 128) {
                swapCount++;
            } else if (mergedValue == 256) {
                deleteCount++;
            }
            // ------------------------------------------
            
            line[i+1]->setValue(0);
            moved = true;
            
            for (int j = i + 1; j < gridSize - 1; j++) {
                line[j]->setValue(line[j+1]->getValue());
            }
            line[gridSize - 1]->setValue(0);
        }
    }
    return moved; 
}

// ==============================================================================
// 6. CÁC HÀM DI CHUYỂN BÀN CỜ
// ==============================================================================
void Grid::shiftLeft() {
    saveState(); // LƯU TRẠNG THÁI TRƯỚC KHI DI CHUYỂN
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile*[gridSize];
        for (int j = 0; j < gridSize; j++) {
            row[j] = board[i][j];
        }
        if (pushLine(row)) moved = true;
        delete[] row;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftRight() {
    saveState(); // LƯU TRẠNG THÁI TRƯỚC KHI DI CHUYỂN
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile*[gridSize];
        for (int j = 0; j < gridSize; j++) {
            row[j] = board[i][gridSize - 1 - j];
        }
        if (pushLine(row)) moved = true;
        delete[] row;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftUp() {
    saveState(); // LƯU TRẠNG THÁI TRƯỚC KHI DI CHUYỂN
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        Tile** col = new Tile*[gridSize];
        for (int i = 0; i < gridSize; i++) {
            col[i] = board[i][j];
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftDown() {
    saveState(); // LƯU TRẠNG THÁI TRƯỚC KHI DI CHUYỂN
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        Tile** col = new Tile*[gridSize];
        for (int i = 0; i < gridSize; i++) {
            col[i] = board[gridSize - 1 - i][j];
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

// ==============================================================================
// 7. CÁC HÀM XỬ LÝ POWER-UPS
// ==============================================================================

// Chức năng 1: Hoàn tác nước đi
bool Grid::useUndo() {
    if (undoCount > 0 && canUndo) {
        // Phục hồi lại dữ liệu từ mảng prevBoard
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                board[i][j]->setValue(prevBoard[i][j]);
            }
        }
        score = prevScore;
        undoCount--;
        canUndo = false; // Ngăn chặn việc Undo nhiều lần liên tiếp
        return true;
    }
    return false;
}

// Chức năng 2: Đổi chỗ 2 ô bất kỳ
bool Grid::useSwap(int r1, int c1, int r2, int c2) {
    if (swapCount > 0) {
        // Kiểm tra tọa độ có nằm trong bàn cờ không
        if (r1 >= 0 && r1 < gridSize && c1 >= 0 && c1 < gridSize &&
            r2 >= 0 && r2 < gridSize && c2 >= 0 && c2 < gridSize) {
            
            // Đổi giá trị 2 ô
            int temp = board[r1][c1]->getValue();
            board[r1][c1]->setValue(board[r2][c2]->getValue());
            board[r2][c2]->setValue(temp);
            
            swapCount--;
            return true;
        }
    }
    return false;
}

// Chức năng 3: Xóa 1 ô bất kỳ
bool Grid::useDelete(int r, int c) {
    if (deleteCount > 0) {
        // Kiểm tra tọa độ hợp lệ
        if (r >= 0 && r < gridSize && c >= 0 && c < gridSize) {
            // Không xóa ô vốn đã trống
            if (board[r][c]->getValue() != 0) {
                board[r][c]->setValue(0); // Đưa giá trị ô về 0
                deleteCount--;
                return true;
            }
        }
    }
    return false;
}

// ==============================================================================
// 8. HÀM KIỂM TRA THUA GAME
// ==============================================================================
bool Grid::checkGameOver() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) return false;
        }
    }
    
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 1; j++) {
            int current = board[i][j]->getValue();
            if (current == board[i][j+1]->getValue()) return false;
            if (current == board[j][i]->getValue()) return false;
        }
    }
    
    return true;
}

// ==============================================================================
// 9. CÁC HÀM GETTER
// ==============================================================================
int Grid::getScore() const { return score; }
int Grid::getTileValue(int row, int col) const { return board[row][col]->getValue(); }
int Grid::getSize() const { return gridSize; }
int Grid::getUndoCount() const { return undoCount; }
int Grid::getSwapCount() const { return swapCount; }
int Grid::getDeleteCount() const { return deleteCount; }
