#ifndef ELEVATORCONTROLSYSTEM_H
#define ELEVATORCONTROLSYSTEM_H

#include <QString>
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

#include "elevator.h"
#include "floor.h"
#include "buildingSafetyService.h"
#include "defs.h"

class ElevatorControlSystem: public QObject
{    

    public:
        ElevatorControlSystem(QLabel** elvFloorNums, QPushButton** floorButtons, QPushButton** elvLevelBtns, QLabel** displays, QCheckBox** ecsChecks, QPushButton** fireHelp, QSpinBox** cabFloorNumChosen, QCheckBox** helpResponses);

        bool elevatorRequest(int floorNumber, int direction); // requesting elevator to floor
        void addDestination(int destFloor, int carNumber); // adding passenger destination to elevator

        // safety
        void helpButton(int carNumber);
        void helpDisconnected(int carNumber);
        bool obstacleDetected(int carNumber);
        void fireButton(int carNumber); // from elevator
        void fireSignal(bool on); // from building
        void powerOutage(bool on); // from building
        bool overload(int carNumber);

        void openDoor(int carNumber, bool pressed);
        void closeDoor(int carNumber);

        void setFloor(int floorNumber);
        void setElevator(int carNumber);

    private:
           Elevator** elevators; // int-> current floor
           QVector<QVector<int>> destinations; // list of dest floors for every elevator, index is carNumber
           Floor** floors; // list of floors; index floorNumber-1
           BuildingSafetyService bss;

           QTimer* timer;

           QLabel** elvFloorNums;
           QPushButton** floorButtons;
           QPushButton** elvLevelBtns;
           QLabel** displays;
           QCheckBox** ecsChecks;
           QPushButton** fireHelp;
           QSpinBox** cabFloorNumChosen;

           static int floorLevel;
           static int carNumber;

           void moveElevator(); // moving an elevator to requested floor
           void addDestinationHelper(int destFloor, int carNumber);

           int getElevatorDirection(int carNumber);
           void setElvCurrFloor(int floorNumber, int carNumber);
};

#endif // ELEVATORCONTROLSYSTEM_H
