#include "Grid.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// 1. KHỞI TẠO / HỦY
Grid::Grid(int boardSize) : size(boardSize), score(0), moveCount(0),
undoCount(2), swapCount(1), deleteCount(0)
{
    srand(static_cast<unsigned int>(time(0)));

    // Cấp phát kích thước cho ma trận bàn cờ, khởi tạo toàn bộ bằng nullptr (ô trống)
    board.resize(size, std::vector<Tile*>(size, nullptr));

    // Bắt đầu game bằng cách khởi tạo bàn cờ
    initBoard();
}

Grid::~Grid() {
    // Dọn dẹp bộ nhớ khi hủy đối tượng Grid để tránh rò rỉ bộ nhớ
    cleanupBoard();
}

void Grid::cleanupBoard() {
    // Duyệt qua toàn bộ bàn cờ, nếu có đối tượng Tile (khác nullptr) thì giải phóng bộ nhớ
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j]) {
                delete board[i][j];
                board[i][j] = nullptr;
            }
}

void Grid::initBoard()
{
    // Dọn dẹp bàn cờ cũ nếu đang chơi lại (Restart)
    cleanupBoard();

    // Reset các chỉ số cơ bản
    score = 0;
    moveCount = 0;
    lastMergedValues.clear();
    history.clear();          // ← thêm dòng này: Xóa lịch sử cũ

    // Khởi tạo 2 ô đầu tiên khi bắt đầu game mới
    spawnTile();
    spawnTile();

    // Lưu lại trạng thái ban đầu này vào lịch sử
    saveState();
}

// 2. LƯU TRẠNG THÁI + SINH Ô
void Grid::saveState()
{
    BoardState state;
    state.score = score;
    // Tạo một ma trận số nguyên cùng kích thước với bàn cờ
    state.board.resize(size, std::vector<int>(size, 0));

    // Sao chép giá trị (value) của các ô Tile hiện tại vào ma trận số nguyên
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            state.board[i][j] = board[i][j] ? board[i][j]->getValue() : 0;

    // Đẩy trạng thái này vào danh sách lịch sử
    history.push_back(state);

    // Giới hạn history (tránh tốn bộ nhớ), ví dụ tối đa 10 bước
    // Nếu quá 10 bước, xóa bước cũ nhất (ở đầu mảng)
    if (history.size() > 10)
        history.erase(history.begin());
}

void Grid::spawnTile() {
    // Tìm tất cả các tọa độ của các ô trống trên bàn cờ
    std::vector<std::pair<int, int>> empty;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (!board[i][j]) empty.push_back({ i, j });

    // Nếu không còn ô trống thì không làm gì cả
    if (empty.empty()) return;

    // Xác định số lượng ô sẽ sinh: Nếu bàn cờ >= 5x5 thì sinh 2 ô, ngược lại sinh 1 ô
    int num = (size >= 5) ? 2 : 1;
    // Đảm bảo không sinh quá số lượng ô trống hiện có
    if (num > (int)empty.size()) num = (int)empty.size();

    for (int k = 0; k < num; ++k) {
        // Chọn ngẫu nhiên một tọa độ trống trong danh sách
        int idx = rand() % empty.size();

        // Mặc định sinh ra số 2
        int val = 2;
        // Nếu bàn cờ >= 6x6, có 10% tỷ lệ (rand() % 10 == 0) sinh ra số 4
        if (size >= 6 && rand() % 10 == 0) val = 4;

        // Tạo đối tượng Tile mới tại tọa độ đã chọn
        board[empty[idx].first][empty[idx].second] =
            new Tile(val, empty[idx].first, empty[idx].second);

        // Kích hoạt hiệu ứng "Pop" (phóng to/thu nhỏ) khi ô mới xuất hiện
        board[empty[idx].first][empty[idx].second]->triggerPop();

        // Xóa tọa độ vừa sinh khỏi danh sách ô trống để vòng lặp kế tiếp không bị trùng
        empty.erase(empty.begin() + idx);
    }
}

// 3. POWER-UPS
bool Grid::useUndo()
{
    // Kiểm tra điều kiện: Cần có lượt Undo, có lịch sử, và đã di chuyển ít nhất 1 bước
    if (undoCount <= 0 || history.empty() || moveCount == 0)
        return false;

    // Lấy trạng thái gần nhất ở cuối mảng lịch sử và xóa nó đi
    BoardState state = history.back();
    history.pop_back();

    // Dọn dẹp bàn cờ hiện tại
    cleanupBoard();
    // Phục hồi lại điểm số
    score = state.score;

    // Tái tạo lại các đối tượng Tile dựa trên mảng số nguyên đã lưu
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (state.board[i][j] > 0)
                board[i][j] = new Tile(state.board[i][j], i, j);

    // Trừ đi 1 lượt sử dụng Undo
    undoCount--;
    // Lùi số bước di chuyển lại 1 bước
    if (moveCount > 0) moveCount--;

    return true;
}

bool Grid::useSwap(int r1, int c1, int r2, int c2) {
    // Kiểm tra xem còn lượt Swap không
    if (swapCount <= 0) return false;
    // Không cho phép đổi chỗ nếu cả 2 vị trí đều là ô trống
    if (!board[r1][c1] && !board[r2][c2]) return false;

    // Đổi chỗ con trỏ trong ma trận
    std::swap(board[r1][c1], board[r2][c2]);

    // Cập nhật lại logic chuyển động bên trong đối tượng Tile cho vị trí 1
    if (board[r1][c1]) board[r1][c1]->move(r1 - board[r1][c1]->getX(), c1 - board[r1][c1]->getY());
    // Cập nhật lại logic chuyển động bên trong đối tượng Tile cho vị trí 2
    if (board[r2][c2]) board[r2][c2]->move(r2 - board[r2][c2]->getX(), c2 - board[r2][c2]->getY());

    swapCount--;
    return true;
}

bool Grid::useDelete(int r, int c) {
    // Kiểm tra lượt Delete và vị trí cần xóa phải có ô (khác nullptr)
    if (deleteCount <= 0 || !board[r][c]) return false;

    // Giải phóng bộ nhớ và gán lại thành ô trống
    delete board[r][c];
    board[r][c] = nullptr;

    deleteCount--;
    return true;
}

// 4. GETTERS
int Grid::getScore() const { return score; }
int Grid::getMoveCount() const { return moveCount; }
int Grid::getUndoCount() const { return undoCount; }
int Grid::getSwapCount() const { return swapCount; }
int Grid::getDeleteCount() const { return deleteCount; }

int Grid::getMaxTile() const {
    int mx = 0;
    // Duyệt toàn bộ bảng tìm giá trị ô lớn nhất hiện có
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j] && board[i][j]->getValue() > mx)
                mx = board[i][j]->getValue();
    return mx;
}

int Grid::getTileValue(int r, int c) const {
    // Kiểm tra giới hạn mảng để tránh lỗi truy xuất vùng nhớ
    if (r >= 0 && r < size && c >= 0 && c < size && board[r][c])
        return board[r][c]->getValue();
    return 0;
}

bool Grid::isGameOver() const {
    // Kiểm tra xem có còn nước đi hợp lệ nào không
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            // Nếu còn ít nhất 1 ô trống -> chưa thua
            if (!board[i][j]) return false;

            // Nếu có 2 ô liền kề theo chiều dọc có cùng giá trị -> có thể gộp -> chưa thua
            if (i + 1 < size && board[i + 1][j] && board[i][j]->getValue() == board[i + 1][j]->getValue()) return false;

            // Nếu có 2 ô liền kề theo chiều ngang có cùng giá trị -> có thể gộp -> chưa thua
            if (j + 1 < size && board[i][j + 1] && board[i][j]->getValue() == board[i][j + 1]->getValue()) return false;
        }
    // Kín bàn cờ và không thể gộp thêm -> Thua
    return true;
}

// 5. MOVE (LOGIC DI CHUYỂN & GỘP Ô)
bool Grid::moveLeft() {
    saveState(); // Lưu trạng thái trước khi vuốt
    lastMergedValues.clear();
    bool moved = false; // Cờ kiểm tra xem có sự thay đổi trên bàn cờ hay không

    // Xử lý từng hàng một
    for (int r = 0; r < size; ++r) {
        // Lưu lại mảng giá trị gốc của hàng này để tí nữa so sánh
        std::vector<int> original(size, 0);
        for (int c = 0; c < size; ++c)
            if (board[r][c]) original[c] = board[r][c]->getValue();

        // Thu thập các giá trị khác nullptr dồn sang trái, đồng thời xóa các ô cũ
        std::vector<int> vals;
        for (int c = 0; c < size; ++c) {
            if (board[r][c]) {
                vals.push_back(board[r][c]->getValue());
                delete board[r][c];
                board[r][c] = nullptr;
            }
        }

        // Xử lý logic gộp ô (Merge)
        std::vector<int> merged;
        std::vector<bool> isMerged; // Đánh dấu ô nào là kết quả của việc gộp để chạy hiệu ứng
        for (size_t i = 0; i < vals.size(); ++i) {
            // Nếu ô hiện tại và ô tiếp theo giống nhau -> Gộp lại
            if (i + 1 < vals.size() && vals[i] == vals[i + 1]) {
                int newVal = vals[i] * 2;
                merged.push_back(newVal);
                isMerged.push_back(true);
                score += newVal; // Cộng điểm
                lastMergedValues.push_back(newVal);
                ++i; // Bỏ qua ô tiếp theo (vì đã bị gộp)
            }
            else {
                // Không gộp được, giữ nguyên giá trị
                merged.push_back(vals[i]);
                isMerged.push_back(false);
            }
        }

        // Đổ dữ liệu đã gộp/dồn trở lại vào bàn cờ từ trái qua phải
        for (size_t c = 0; c < merged.size(); ++c) {
            board[r][c] = new Tile(merged[c], r, (int)c);
            if (isMerged[c]) board[r][c]->triggerPop(); // Chạy hiệu ứng cho ô vừa gộp
        }

        // So sánh hàng mới với hàng gốc xem có thực sự dịch chuyển không
        for (int c = 0; c < size; ++c) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[c] != nv) moved = true;
        }
    }

    // Nếu có sự thay đổi -> tính là 1 bước đi và sinh ô mới
    if (moved) { moveCount++; spawnTile(); }
    return moved;
}

bool Grid::moveRight() {
    saveState();
    lastMergedValues.clear();
    bool moved = false;

    for (int r = 0; r < size; ++r) {
        std::vector<int> original(size, 0);
        for (int c = 0; c < size; ++c)
            if (board[r][c]) original[c] = board[r][c]->getValue();

        // Duyệt từ phải qua trái để dồn các ô
        std::vector<int> vals;
        for (int c = size - 1; c >= 0; --c) {
            if (board[r][c]) {
                vals.push_back(board[r][c]->getValue());
                delete board[r][c];
                board[r][c] = nullptr;
            }
        }

        std::vector<int> merged;
        std::vector<bool> isMerged;
        // Logic gộp ô tương tự moveLeft
        for (size_t i = 0; i < vals.size(); ++i) {
            if (i + 1 < vals.size() && vals[i] == vals[i + 1]) {
                int newVal = vals[i] * 2;
                merged.push_back(newVal);
                isMerged.push_back(true);
                score += newVal;
                lastMergedValues.push_back(newVal);
                ++i;
            }
            else {
                merged.push_back(vals[i]);
                isMerged.push_back(false);
            }
        }

        // Đổ dữ liệu đã gộp trở lại bàn cờ, theo hướng từ phải qua trái
        for (size_t i = 0; i < merged.size(); ++i) {
            int c = size - 1 - (int)i; // Tính toán tọa độ cột tính từ lề phải
            board[r][c] = new Tile(merged[i], r, c);
            if (isMerged[i]) board[r][c]->triggerPop();
        }

        // Kiểm tra sự thay đổi
        for (int c = 0; c < size; ++c) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[c] != nv) moved = true;
        }
    }

    if (moved) { moveCount++; spawnTile(); }
    return moved;
}

bool Grid::moveUp() {
    saveState();
    lastMergedValues.clear();
    bool moved = false;

    // Xử lý từng CỘT thay vì từng hàng
    for (int c = 0; c < size; ++c) {
        std::vector<int> original(size, 0);
        for (int r = 0; r < size; ++r)
            if (board[r][c]) original[r] = board[r][c]->getValue();

        // Gom các giá trị từ trên xuống dưới
        std::vector<int> vals;
        for (int r = 0; r < size; ++r) {
            if (board[r][c]) {
                vals.push_back(board[r][c]->getValue());
                delete board[r][c];
                board[r][c] = nullptr;
            }
        }

        std::vector<int> merged;
        std::vector<bool> isMerged;
        // Logic gộp ô
        for (size_t i = 0; i < vals.size(); ++i) {
            if (i + 1 < vals.size() && vals[i] == vals[i + 1]) {
                int newVal = vals[i] * 2;
                merged.push_back(newVal);
                isMerged.push_back(true);
                score += newVal;
                lastMergedValues.push_back(newVal);
                ++i;
            }
            else {
                merged.push_back(vals[i]);
                isMerged.push_back(false);
            }
        }

        // Đổ dữ liệu từ trên xuống dưới vào bàn cờ
        for (size_t r = 0; r < merged.size(); ++r) {
            board[r][c] = new Tile(merged[r], (int)r, c);
            if (isMerged[r]) board[r][c]->triggerPop();
        }

        // Kiểm tra sự thay đổi theo chiều dọc
        for (int r = 0; r < size; ++r) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[r] != nv) moved = true;
        }
    }

    if (moved) { moveCount++; spawnTile(); }
    return moved;
}

bool Grid::moveDown() {
    saveState();
    lastMergedValues.clear();
    bool moved = false;

    // Xử lý từng CỘT
    for (int c = 0; c < size; ++c) {
        std::vector<int> original(size, 0);
        for (int r = 0; r < size; ++r)
            if (board[r][c]) original[r] = board[r][c]->getValue();

        // Gom các giá trị từ dưới lên trên
        std::vector<int> vals;
        for (int r = size - 1; r >= 0; --r) {
            if (board[r][c]) {
                vals.push_back(board[r][c]->getValue());
                delete board[r][c];
                board[r][c] = nullptr;
            }
        }

        std::vector<int> merged;
        std::vector<bool> isMerged;
        // Logic gộp ô
        for (size_t i = 0; i < vals.size(); ++i) {
            if (i + 1 < vals.size() && vals[i] == vals[i + 1]) {
                int newVal = vals[i] * 2;
                merged.push_back(newVal);
                isMerged.push_back(true);
                score += newVal;
                lastMergedValues.push_back(newVal);
                ++i;
            }
            else {
                merged.push_back(vals[i]);
                isMerged.push_back(false);
            }
        }

        // Đổ dữ liệu từ dưới lên trên (tính toán tọa độ r đảo ngược)
        for (size_t i = 0; i < merged.size(); ++i) {
            int r = size - 1 - (int)i;
            board[r][c] = new Tile(merged[i], r, c);
            if (isMerged[i]) board[r][c]->triggerPop();
        }

        // Kiểm tra sự thay đổi theo chiều dọc
        for (int r = 0; r < size; ++r) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[r] != nv) moved = true;
        }
    }

    if (moved) { moveCount++; spawnTile(); }
    return moved;
}
// 6. ANIMATIONS & CẬP NHẬT POWER-UPS
void Grid::updateAnimations() {
    // Duyệt qua toàn bộ và gọi hàm update hiệu ứng của từng ô
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j]) board[i][j]->updateAnimation();
}

float Grid::getTileScale(int r, int c) const {
    // Trả về mức độ phóng to/thu nhỏ hiện tại của một ô 
    if (r >= 0 && r < size && c >= 0 && c < size && board[r][c])
        return board[r][c]->getScale();
    return 1.0f;
}

// Giới hạn tối đa mọi power (kỹ năng đặc biệt)
static const int MAX_POWER = 5;

// Các hàm cộng thêm số lượng quyền năng (có chặn trên bởi MAX_POWER)
void Grid::addUndo(int amount)
{
    undoCount = std::min(undoCount + amount, MAX_POWER);
}

void Grid::addSwap(int amount)
{
    swapCount = std::min(swapCount + amount, MAX_POWER);
}

void Grid::addDelete(int amount)
{
    deleteCount = std::min(deleteCount + amount, MAX_POWER);
}

// Set cứng số lượng quyền năng cho từng loại
void Grid::setPowerCounts(int undo, int swap, int del)
{
    undoCount = std::min(undo, MAX_POWER);
    swapCount = std::min(swap, MAX_POWER);
    deleteCount = std::min(del, MAX_POWER);
}