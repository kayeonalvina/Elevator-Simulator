#include "elevator.h"

Elevator::Elevator(int cabNumber)
{
    this->cabNumber = cabNumber;
    this->currentFloor = 1;
    this->direction = STOP;
}

int Elevator::getCabNumber() {
    return cabNumber;
}

int Elevator::getDirection() {
    return direction;
}

void Elevator::setCurrentFloor(int floor) {
    this->currentFloor = floor;
}

int Elevator::getCurrentFloor() {
    return currentFloor;
}

void Elevator::setDirection(int direction) {
    this->direction = direction;
}
