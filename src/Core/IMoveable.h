#ifndef IMOVEABLE_H
#define IMOVEABLE_H

// Interface thể hiện tính Trừu tượng hóa (Abstraction)
class IMoveable {
public:
    virtual ~IMoveable() = default;
    
    // Phương thức thuần ảo bắt buộc các lớp con phải ghi đè
    virtual void move(int dx, int dy) = 0;
};

#endif // IMOVEABLE_H
