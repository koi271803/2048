#include "Grid.h"
#include <cstdlib>
#include <ctime>   // Thư viện hỗ trợ lấy thời gian thực của hệ thống
#include <vector>

// ==============================================================================
// 1. CONSTRUCTOR (HÀM KHỞI TẠO)
// ==============================================================================
Grid::Grid(int size) {
    this->gridSize = size;
    this->score = 0;
    
    // Thiết lập hạt giống (seed) cho hàm random dựa trên thời gian thực.
    // Điều này đảm bảo mỗi lần chạy game, các số sinh ra sẽ hoàn toàn ngẫu nhiên.
    // Ép kiểu (unsigned int) để tránh cảnh báo mất mát dữ liệu của C++.
    srand((unsigned int)time(0)); 
    
    // Cấp phát bộ nhớ động cho mảng 2 chiều 'board' chứa các con trỏ Tile.
    board = new Tile**[gridSize];           // Tạo mảng các hàng (rows)
    for (int i = 0; i < gridSize; i++) {
        board[i] = new Tile*[gridSize];     // Tạo mảng các cột (cols) cho từng hàng
        for (int j = 0; j < gridSize; j++) {
            // Khởi tạo từng ô gạch (Tile) với giá trị ban đầu là 0 (ô trống)
            board[i][j] = new Tile(0, i, j); 
        }
    }
    
    // Luật game 2048: Khi bắt đầu game, trên bàn cờ luôn có sẵn 2 số
    spawnRandomTile();
    spawnRandomTile();
}

// ==============================================================================
// 2. DESTRUCTOR (HÀM HỦY)
// ==============================================================================
Grid::~Grid() {
    // Thu hồi bộ nhớ để chống rò rỉ (memory leak) khi thoát game
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            delete board[i][j]; // Xóa từng đối tượng Tile
        }
        delete[] board[i];      // Xóa từng mảng cột
    }
    delete[] board;             // Xóa mảng hàng chứa con trỏ tổng
}

// ==============================================================================
// 3. HÀM SINH SỐ NGẪU NHIÊN (2 hoặc 4)
// ==============================================================================
void Grid::spawnRandomTile() {
    // Dùng vector để lưu danh sách tọa độ (row, col) của tất cả các ô còn trống
    std::vector<std::pair<int, int>> emptyCells;

    // Quét toàn bộ bàn cờ để tìm ô trống (giá trị = 0)
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) {
                emptyCells.push_back({i, j}); // Thêm tọa độ ô trống vào danh sách
            }
        }
    }

    // Nếu bàn cờ đã đầy (không còn ô trống), thoát hàm không sinh thêm số
    if (emptyCells.empty()) return;

    // Chọn ngẫu nhiên 1 vị trí trong danh sách các ô trống
    int randomIndex = rand() % emptyCells.size();
    int row = emptyCells[randomIndex].first;
    int col = emptyCells[randomIndex].second;

    // Xác định xác suất sinh số: 90% ra số 2, 10% ra số 4
    int randomChance = rand() % 100;
    int newValue = (randomChance < 90) ? 2 : 4;
    
    // Gán giá trị mới cho ô vừa được chọn
    board[row][col]->setValue(newValue);
}

// ==============================================================================
// 4. HÀM LÕI: THUẬT TOÁN DỒN VÀ GỘP SỐ TRÊN 1 ĐƯỜNG THẲNG (1D Array)
// ==============================================================================
bool Grid::pushLine(Tile** line) {
    bool moved = false;
    int insertPos = 0; // Vị trí chèn phần tử (con trỏ ảo trỏ đến ô trống gần nhất)
    
    // BƯỚC 1: DỒN CÁC SỐ KHÁC 0 VỀ ĐẦU MẢNG
    for (int i = 0; i < gridSize; i++) {
        if (line[i]->getValue() != 0) {
            // Nếu ô hiện tại có số và không nằm ở đúng vị trí chèn
            if (i != insertPos) {
                // Di chuyển số về vị trí chèn, ô cũ biến thành 0
                line[insertPos]->setValue(line[i]->getValue());
                line[i]->setValue(0);
                moved = true; // Ghi nhận là có sự di chuyển
            }
            insertPos++; // Dịch vị trí chèn lên 1 ô
        }
    }
    
    // BƯỚC 2: GỘP CÁC SỐ GIỐNG NHAU NẰM CẠNH NHAU
    for (int i = 0; i < gridSize - 1; i++) {
        int currentVal = line[i]->getValue();
        int nextVal = line[i+1]->getValue();
        
        // Nếu 2 ô liền kề có cùng giá trị (và không phải ô trống)
        if (currentVal != 0 && currentVal == nextVal) {
            // Ô hiện tại nhân đôi giá trị
            line[i]->setValue(currentVal * 2);
            score += currentVal * 2; // Cộng điểm bằng đúng giá trị vừa gộp được
            
            // Ô kế bên biến thành 0
            line[i+1]->setValue(0);
            moved = true;
            
            // Sau khi gộp, sẽ xuất hiện 1 ô trống ở giữa, 
            // Cần kéo toàn bộ các phần tử phía sau lên 1 nấc để lấp chỗ trống
            for (int j = i + 1; j < gridSize - 1; j++) {
                line[j]->setValue(line[j+1]->getValue());
            }
            // Gán ô cuối cùng bằng 0 vì đã bị kéo lên
            line[gridSize - 1]->setValue(0);
        }
    }
    // Trả về true nếu mảng có sự thay đổi (để biết có nên sinh thêm số hay không)
    return moved; 
}

// ==============================================================================
// 5. CÁC HÀM DI CHUYỂN BÀN CỜ
// Thuật toán: Trích xuất các Hàng/Cột thành các mảng 1 chiều (line) 
// rồi ném cho hàm pushLine() xử lý, sau đó tự động cập nhật lại bàn cờ.
// ==============================================================================

void Grid::shiftLeft() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile*[gridSize];
        for (int j = 0; j < gridSize; j++) {
            row[j] = board[i][j]; // Lấy dữ liệu từ trái sang phải
        }
        if (pushLine(row)) moved = true;
        delete[] row; // Xóa mảng tạm để tránh tràn ram
    }
    // Nếu bàn cờ có sự di chuyển, sinh ra một số ngẫu nhiên mới
    if (moved) spawnRandomTile();
}

void Grid::shiftRight() {
    bool moved = false;
    for (int i = 0; i < gridSize; i++) {
        Tile** row = new Tile*[gridSize];
        for (int j = 0; j < gridSize; j++) {
            // Lấy dữ liệu ĐẢO NGƯỢC từ phải sang trái
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
        Tile** col = new Tile*[gridSize];
        for (int i = 0; i < gridSize; i++) {
            col[i] = board[i][j]; // Lấy dữ liệu từ trên xuống dưới
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

void Grid::shiftDown() {
    bool moved = false;
    for (int j = 0; j < gridSize; j++) {
        Tile** col = new Tile*[gridSize];
        for (int i = 0; i < gridSize; i++) {
            // Lấy dữ liệu ĐẢO NGƯỢC từ dưới lên trên
            col[i] = board[gridSize - 1 - i][j];
        }
        if (pushLine(col)) moved = true;
        delete[] col;
    }
    if (moved) spawnRandomTile();
}

// ==============================================================================
// 6. HÀM KIỂM TRA THUA GAME
// ==============================================================================
bool Grid::checkGameOver() {
    // Điều kiện 1: Nếu vẫn còn ít nhất 1 ô trống -> Chưa thua
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (board[i][j]->getValue() == 0) return false;
        }
    }
    
    // Điều kiện 2: Bàn cờ đã đầy (không qua được return false ở trên)
    // Phải kiểm tra xem có 2 ô liền kề nào giống nhau để gộp không
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 1; j++) {
            int current = board[i][j]->getValue();
            
            // Kiểm tra hàng ngang (ô hiện tại với ô bên phải)
            if (current == board[i][j+1]->getValue()) return false;
            
            // Kiểm tra hàng dọc (ô hiện tại với ô bên dưới)
            if (current == board[j][i]->getValue()) return false;
        }
    }
    
    // Nếu qua được cả 2 vòng lặp mà không return false, nghĩa là đã Thua
    return true;
}

// ==============================================================================
// 7. CÁC HÀM GETTER (Bảo vệ tính đóng gói, UI chỉ được Đọc, không được Ghi)
// ==============================================================================
int Grid::getScore() const { return score; }
int Grid::getTileValue(int row, int col) const { return board[row][col]->getValue(); }
int Grid::getSize() const { return gridSize; }
