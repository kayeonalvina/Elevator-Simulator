#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QString>
#include "audioSystem.h"
#include "defs.h"
#include "button.h"
#include "display.h"


class Elevator
{
    public:
        Elevator(int cabNumber);

        // getters
        int getCabNumber();
        int getDirection();
        int getCurrentFloor();

        // setters
        void setDirection(int direction);
        void setCurrentFloor(int floor);

        Button fireButton;
        Button helpButton;
        Button openDoor;
        Button closeDoor;
        AudioSystem audioSystem;
        Display display;

    private:
        int cabNumber;
        int direction;
        int currentFloor;
};

#endif // ELEVATOR_H
