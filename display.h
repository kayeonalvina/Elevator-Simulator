#ifndef DISPLAY_H
#define DISPLAY_H

#include <QString>

class Display
{
public:
    Display();

    QString getDisplay();
    void setDisplay(QString input);

private:
    QString display;
};

#endif // DISPLAY_H
