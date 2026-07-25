#include "Grid.h"

// Hàm xử lý khi người chơi trượt sang trái
void Grid::shiftLeft() {
    bool isMoved = false;

    for (int row = 0; row < gridSize; row++) {
        // Bước 1: Dồn các số về bên trái (loại bỏ khoảng trống)
        int insertPos = 0;
        for (int col = 0; col < gridSize; col++) {
            if (board[row][col]->getValue() > 0) {
                if (col != insertPos) {
                    // Chuyển giá trị sang ô trống bên trái
                    board[row][insertPos]->setValue(board[row][col]->getValue());
                    board[row][col]->setValue(0);
                    isMoved = true;
                }
                insertPos++;
            }
        }

        // Bước 2: Gộp các số giống nhau
        for (int col = 0; col < gridSize - 1; col++) {
            int currentVal = board[row][col]->getValue();
            int nextVal = board[row][col + 1]->getValue();

            // Bỏ qua ô trống hoặc chướng ngại vật (-1)
            if (currentVal > 0 && currentVal == nextVal) {
                // Nhân đôi ô hiện tại và xóa ô bên cạnh
                board[row][col]->setValue(currentVal * 2);
                board[row][col + 1]->setValue(0);
                
                // Cập nhật điểm số nội bộ (Đóng gói dữ liệu)
                score += currentVal * 2; 
                isMoved = true;
            }
        }

        // Bước 3: Dồn lại lần nữa sau khi gộp
        insertPos = 0;
        for (int col = 0; col < gridSize; col++) {
            if (board[row][col]->getValue() > 0) {
                if (col != insertPos) {
                    board[row][insertPos]->setValue(board[row][col]->getValue());
                    board[row][col]->setValue(0);
                }
                insertPos++;
            }
        }
    }

    // Nếu bàn cờ có sự thay đổi, tiến hành sinh số mới
    if (isMoved) {
        spawnRandomTile();
    }
}
