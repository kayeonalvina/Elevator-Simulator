#include "display.h"

Display::Display()
{
    this->display = "";
}
QString Display::getDisplay() {
    return display;
}

void Display::setDisplay(QString input) {
    display = input;
}
