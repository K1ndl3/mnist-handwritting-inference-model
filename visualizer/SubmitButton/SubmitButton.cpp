#include "SubmitButton.hpp"
#include <raylib.h>
#include <iostream>

void SubmitButton::draw(int offsetX, int offsetY, int padding) {
    setPosX(offsetX + padding);
    setPosY(offsetY);
    setWidth(300);
    setHeight(100);
    DrawRectangle(_posX, _posY, _width, _height, GREEN);
    DrawText("Submit", offsetX + padding, offsetY, 95, BLACK);
}

bool SubmitButton::submit() {
    int left = _posX;
    int right = _posX + _width;
    int top = _posY;
    int bottom = _posY + _height;
    Vector2 mousePos = GetMousePosition();
    int mouseX = mousePos.x;
    int mouseY = mousePos.y; 
    if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _isSubmit = true;
            std::cout << "submitted\n";
            return true;
        } else {
            _isSubmit = false;
            return false;
        }
    }
    return false;
}