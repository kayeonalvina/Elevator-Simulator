#include "button.h"

Button::Button()
{
    this->status = false;
}

bool Button::getStatus() {
    return status;
}

void Button::setStatus(bool status) {
    this->status = status;
}
