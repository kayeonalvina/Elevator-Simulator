#ifndef FLOOR_H
#define FLOOR_H

#include <QString>
#include "button.h"
#include "display.h"

class Floor
{
    public:
        Floor(int floorNumber);

        Button upBtn;
        Button downBtn;
        Display display;

    private:
        int floorNumber;
};

#endif // FLOOR_H
