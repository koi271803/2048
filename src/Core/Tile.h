#ifndef TILE_H
#define TILE_H

#include "IMoveable.h"

class Tile : public IMoveable {
private:
    int value; // Giá trị của ô số (2, 4, 8, 16...)
    int x;     // Vị trí hàng hiện tại trên bàn cờ
    int y;     // Vị trí cột hiện tại trên bàn cờ

    // --- BIẾN QUẢN LÝ HOẠT ẢNH NẢY ---
    float popScale = 1.0f;
    bool isPopping = false;

public:
    // Constructor
    Tile();
    Tile(int val, int startX, int startY);

    // Getter và Setter cho giá trị của ô
    int getValue() const;
    void setValue(int val);

    // Getter lấy vị trí tọa độ
    int getX() const;
    int getY() const;

    // Cài đặt phương thức di chuyển từ lớp IMoveable
    void move(int dx, int dy) override;

    // --- CÁC HÀM QUẢN LÝ HOẠT ẢNH NẢY ---
    float getScale() const { return popScale; }

    // Kích hoạt nảy phình to 130%
    void triggerPop() {
        popScale = 1.15f;
        isPopping = true;
    }

    // Gọi mỗi khung hình để thu nhỏ từ 130% về 100%
    void updateAnimation() {
        if (isPopping) {
            popScale -= 0.05f;
            if (popScale <= 1.0f) {
                popScale = 1.0f;
                isPopping = false;
            }
        }
    }
};

#endif // TILE_H
