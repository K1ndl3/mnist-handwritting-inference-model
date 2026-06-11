#pragma once

#include <random>
class SubmitButton {

private:
    bool _isSubmit;
    int _height;
    int _width;
    int _posX;
    int _posY;
public:
    SubmitButton() {
        _isSubmit = false;
    }

    inline void setPosX(int posX) { _posX = posX; };
    inline void setPosY(int posY) { _posY = posY; };
    inline void setHeight(int height) {_height = height; };
    inline void setWidth(int width) {_width = width; };

    void draw(int offsetX, int offsetY, int padding);
    void submit();
};