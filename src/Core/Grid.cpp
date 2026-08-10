#include "Grid.h"
#include <cstdlib>
#include <ctime>

// 1. KHỞI TẠO / HỦY
Grid::Grid(int boardSize) : size(boardSize), score(0), previousScore(0), moveCount(0),
undoCount(2), swapCount(1), deleteCount(0)
{
    srand(static_cast<unsigned int>(time(0)));
    board.resize(size, std::vector<Tile*>(size, nullptr));
    previousBoard.resize(size, std::vector<int>(size, 0));
    initBoard();
}

Grid::~Grid() {
    cleanupBoard();
}

void Grid::cleanupBoard() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j]) {
                delete board[i][j];
                board[i][j] = nullptr;
            }
}

void Grid::initBoard() {
    cleanupBoard();
    score = 0;
    moveCount = 0;
    lastMergedValues.clear();

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            previousBoard[i][j] = 0;

    spawnTile();
    spawnTile();
    saveState();
}

// 2. LƯU TRẠNG THÁI + SINH Ô
void Grid::saveState() {
    previousScore = score;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            previousBoard[i][j] = board[i][j] ? board[i][j]->getValue() : 0;
}

void Grid::spawnTile() {
    std::vector<std::pair<int, int>> empty;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (!board[i][j]) empty.push_back({ i, j });

    if (empty.empty()) return;

    int num = (size >= 5) ? 2 : 1;
    if (num > (int)empty.size()) num = (int)empty.size();

    for (int k = 0; k < num; ++k) {
        int idx = rand() % empty.size();
        int val = 2;
        if (size >= 6 && rand() % 10 == 0) val = 4;

        board[empty[idx].first][empty[idx].second] =
            new Tile(val, empty[idx].first, empty[idx].second);
        board[empty[idx].first][empty[idx].second]->triggerPop();
        empty.erase(empty.begin() + idx);
    }
}

// 3. POWER-UPS
bool Grid::useUndo() {
    if (undoCount <= 0 || moveCount == 0) return false;
    cleanupBoard();
    score = previousScore;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (previousBoard[i][j])
                board[i][j] = new Tile(previousBoard[i][j], i, j);
    undoCount--;
    return true;
}

bool Grid::useSwap(int r1, int c1, int r2, int c2) {
    if (swapCount <= 0) return false;
    if (!board[r1][c1] && !board[r2][c2]) return false;

    std::swap(board[r1][c1], board[r2][c2]);
    if (board[r1][c1]) board[r1][c1]->move(r1 - board[r1][c1]->getX(), c1 - board[r1][c1]->getY());
    if (board[r2][c2]) board[r2][c2]->move(r2 - board[r2][c2]->getX(), c2 - board[r2][c2]->getY());
    swapCount--;
    return true;
}

bool Grid::useDelete(int r, int c) {
    if (deleteCount <= 0 || !board[r][c]) return false;
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
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j] && board[i][j]->getValue() > mx)
                mx = board[i][j]->getValue();
    return mx;
}

int Grid::getTileValue(int r, int c) const {
    if (r >= 0 && r < size && c >= 0 && c < size && board[r][c])
        return board[r][c]->getValue();
    return 0;
}

bool Grid::isGameOver() const {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            if (!board[i][j]) return false;
            if (i + 1 < size && board[i + 1][j] && board[i][j]->getValue() == board[i + 1][j]->getValue()) return false;
            if (j + 1 < size && board[i][j + 1] && board[i][j]->getValue() == board[i][j + 1]->getValue()) return false;
        }
    return true;
}

// 5. MOVE
bool Grid::moveLeft() {
    saveState();
    lastMergedValues.clear();
    bool moved = false;

    for (int r = 0; r < size; ++r) {
        std::vector<int> original(size, 0);
        for (int c = 0; c < size; ++c)
            if (board[r][c]) original[c] = board[r][c]->getValue();

        std::vector<int> vals;
        for (int c = 0; c < size; ++c) {
            if (board[r][c]) {
                vals.push_back(board[r][c]->getValue());
                delete board[r][c];
                board[r][c] = nullptr;
            }
        }

        std::vector<int> merged;
        std::vector<bool> isMerged;
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

        for (size_t c = 0; c < merged.size(); ++c) {
            board[r][c] = new Tile(merged[c], r, (int)c);
            if (isMerged[c]) board[r][c]->triggerPop();
        }

        for (int c = 0; c < size; ++c) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[c] != nv) moved = true;
        }
    }

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

        for (size_t i = 0; i < merged.size(); ++i) {
            int c = size - 1 - (int)i;
            board[r][c] = new Tile(merged[i], r, c);
            if (isMerged[i]) board[r][c]->triggerPop();
        }

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

    for (int c = 0; c < size; ++c) {
        std::vector<int> original(size, 0);
        for (int r = 0; r < size; ++r)
            if (board[r][c]) original[r] = board[r][c]->getValue();

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

        for (size_t r = 0; r < merged.size(); ++r) {
            board[r][c] = new Tile(merged[r], (int)r, c);
            if (isMerged[r]) board[r][c]->triggerPop();
        }

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

    for (int c = 0; c < size; ++c) {
        std::vector<int> original(size, 0);
        for (int r = 0; r < size; ++r)
            if (board[r][c]) original[r] = board[r][c]->getValue();

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

        for (size_t i = 0; i < merged.size(); ++i) {
            int r = size - 1 - (int)i;
            board[r][c] = new Tile(merged[i], r, c);
            if (isMerged[i]) board[r][c]->triggerPop();
        }

        for (int r = 0; r < size; ++r) {
            int nv = board[r][c] ? board[r][c]->getValue() : 0;
            if (original[r] != nv) moved = true;
        }
    }

    if (moved) { moveCount++; spawnTile(); }
    return moved;
}

// 6. ANIMATIONS
void Grid::updateAnimations() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j]) board[i][j]->updateAnimation();
}

float Grid::getTileScale(int r, int c) const {
    if (r >= 0 && r < size && c >= 0 && c < size && board[r][c])
        return board[r][c]->getScale();
    return 1.0f;
}
