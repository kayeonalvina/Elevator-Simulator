#include "elevatorControlSystem.h"
#include <QDebug>


int ElevatorControlSystem::carNumber = -1;
int ElevatorControlSystem::floorLevel = 0;

ElevatorControlSystem::ElevatorControlSystem(QLabel** elvFloorNums, QPushButton** floorButtons, QPushButton** elvLevelBtns, QLabel** displays, QCheckBox** ecsChecks,
                                             QPushButton** fireHelp, QSpinBox** cabFloorNumChosen, QCheckBox** helpResponses): bss(displays[2], helpResponses)
{
    // initializing
    this->elvFloorNums = elvFloorNums;
    this->floorButtons = floorButtons;
    this->elvLevelBtns = elvLevelBtns;
    this->displays = displays;
    this->ecsChecks = ecsChecks;
    this->fireHelp = fireHelp;
    this->cabFloorNumChosen = cabFloorNumChosen;

    elevators = new Elevator*[ELEVATORNUM];
    floors = new Floor*[FLOORNUM];

    for(int i=0; i<ELEVATORNUM; i++) {
        elevators[i] = new Elevator(++carNumber);
        destinations.append(QVector<int>());
    }

    for(int i=0; i<FLOORNUM; i++) {
        floors[i] = new Floor(++floorLevel);
    }

    // have elevator take some seconds to move between floors
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ElevatorControlSystem::moveElevator);
    timer->start(MOVINGTOLEVEL);
}

bool ElevatorControlSystem::elevatorRequest(int floorNumber, int direction) {

    int closestElv;
    int closestNum;
    int difference;

    // update floor button state
    if(direction == 1)
        floors[floorNumber-1]->upBtn.setStatus(true);
    else
        floors[floorNumber-1]->downBtn.setStatus(true);

    // display floor
    setFloor(floorNumber);

    // loop until an elevator is found
    while(1) {
        closestElv = -1;
        closestNum = FLOORNUM+5;

        for(int i=0; i<ELEVATORNUM; i++) {
            // check for free elevator
            if(getElevatorDirection(i) == 0 && destinations[i].isEmpty()) {
                difference = floorNumber - elevators[i]->getCurrentFloor();

                // if negative, make it positive
                if(difference < 0)
                    difference *= -1;

                // if the difference is smaller, store
                if(difference < closestNum) {
                    closestElv = i;
                    closestNum = difference;
                }
            }
        }

        // if an elevator is found
        if(closestElv != -1) {
            // add floor to the chosen elevator
            destinations[closestElv].append(floorNumber+ECSCOMMAND);

            // specify direction
            if(elevators[closestElv]->getCurrentFloor() - floorNumber > 0)
                elevators[closestElv]->setDirection(DOWN);
            else if(elevators[closestElv]->getCurrentFloor() - floorNumber < 0)
                elevators[closestElv]->setDirection(UP);

            floors[floorNumber-1]->display.setDisplay("Elevator: " + QString::number(closestElv+1));
            moveElevator();
            return true;
        }
    }
    return false;
}

void ElevatorControlSystem::moveElevator() {

    int newDestination;

    for(int i=0; i<ELEVATORNUM; i++) {
        // when there is more than one destination
        if(!destinations[i].isEmpty()) {
            // move in the direction
            elvFloorNums[i]->setNum((elvFloorNums[i]->text()).toInt()+getElevatorDirection(i));

            setElvCurrFloor((elvFloorNums[i]->text()).toInt(), i);

            // for every new floor, ring
            elevators[i]->audioSystem.ring(i);

            // ouput fire & power outage message along with floor number and direction
            if(bss.fire.getStatus() || bss.powerOutage.getStatus()) {
                elevators[i]->display.setDisplay(elevators[i]->display.getDisplay().split("\n")[0] + "\n" + QString::number(elevators[i]->getCurrentFloor()) + ((getElevatorDirection(i) == 1) ? " up" : (getElevatorDirection(i) == -1) ? " down" : ""));
            } else {
                elevators[i]->display.setDisplay(QString::number(elevators[i]->getCurrentFloor()) + ((getElevatorDirection(i) == 1) ? " up" : (getElevatorDirection(i) == -1) ? " down" : ""));
            }

            newDestination = destinations[i][0];
            // if newDestination is bigger than FLOORNUM, it means that it was specified by the ECS (passenger didn't press that floor button)
            if(newDestination>FLOORNUM)
                newDestination -= ECSCOMMAND;

            // once elevator current floor and floor number are equal
            if(elevators[i]->getCurrentFloor() == newDestination) {
                // stop the elevator
                elevators[i]->setDirection(STOP);

                // deilluminate floor button
                if(floors[newDestination-1]->upBtn.getStatus()) {
                    floors[newDestination-1]->upBtn.setStatus(false);
                } else if (floors[newDestination-1]->downBtn.getStatus()){
                    floors[newDestination-1]->downBtn.setStatus(false);
                }

                // set display
                floors[newDestination-1]->display.setDisplay("");
                elevators[i]->display.setDisplay(QString::number(elevators[i]->getCurrentFloor()));

                // remove once arrived at floor
                destinations[i].pop_front();

                // if fire button was pressed, deilluminate
                if(elevators[i]->fireButton.getStatus()) {
                    elevators[i]->fireButton.setStatus(false);
                    setElevator(i);
                }

                qInfo() << "Opening floor and elevator doors";
            }
        }
    }

    // set floor & elevator panels
    setElevator(cabFloorNumChosen[0]->value()-1);
    setFloor(cabFloorNumChosen[1]->value());
}

void ElevatorControlSystem::addDestination(int destFloor, int carNumber) {
    // if the close door was pressed
    if (elevators[carNumber]->closeDoor.getStatus()) {
        // move immediately
        addDestinationHelper(destFloor, carNumber);
    } else {
        // move after 5 seconds
        QTimer* defaultTimer = new QTimer(this);
        connect(defaultTimer, &QTimer::timeout, this, [=]() {
            defaultTimer->stop();
            defaultTimer->deleteLater();

            // if door is open, delay
            if (elevators[carNumber]->openDoor.getStatus()) {
                // to be notified when open door is pressed
                QTimer* doorCheckTimer = new QTimer(this);
                connect(doorCheckTimer, &QTimer::timeout, this, [=]() {

                    // once door is not held open
                    if (!elevators[carNumber]->openDoor.getStatus()) {
                        doorCheckTimer->stop();
                        doorCheckTimer->deleteLater();

                        // move
                        addDestinationHelper(destFloor, carNumber);
                    }
                });

                // check every 100ms
                doorCheckTimer->start(100);
            } else {
                // move
                addDestinationHelper(destFloor, carNumber);
            }
        });

        // Set the default waiting time to 5 seconds
        defaultTimer->start(DEFAULTOPEN);
    }
}

void ElevatorControlSystem::addDestinationHelper(int destFloor, int carNumber) {
    // if there isn't an obstacle or an overload
    if (obstacleDetected(carNumber) && overload(carNumber)) {
        // if there is no destination specified already
        if (destinations[carNumber].isEmpty()) {
            destinations[carNumber].append(destFloor);

            // get direction
            int difference = elevators[carNumber]->getCurrentFloor() - destFloor;
            if (difference > 0)
                elevators[carNumber]->setDirection(DOWN);
            else if (difference < 0)
                elevators[carNumber]->setDirection(UP);

            // default closeDoor status to false
            elevators[carNumber]->closeDoor.setStatus(false);
            qInfo() << "Closing floor and elevator doors";
            elevators[carNumber]->audioSystem.ring(carNumber);
            moveElevator();
        }
    }
}

void ElevatorControlSystem::openDoor(int carNumber, bool pressed) {
    if(pressed) {
        elevators[carNumber]->openDoor.setStatus(true);
        qInfo() << "Open door pressed.";
    } else {
        elevators[carNumber]->openDoor.setStatus(false);
    }

}

void ElevatorControlSystem::closeDoor(int carNumber){
    elevators[carNumber]->closeDoor.setStatus(true);
    qInfo() << "Close door pressed.";
}

bool ElevatorControlSystem::obstacleDetected(int carNumber) {
    // if an obstacle is checked for the elevator
    if(ecsChecks[carNumber]->isChecked()) {
        // stop moving and provide text and display message
        elevators[carNumber]->setDirection(STOP);
        elevators[carNumber]->audioSystem.longRing(carNumber);
        elevators[carNumber]->display.setDisplay("Obstacle detected. Door stays opened.");

        return false;
    }

//    qInfo() << "No obstacle detected. Door closed.";
    return true;
}

bool ElevatorControlSystem::overload(int carNumber) {
    // if overload is checked for the elevator
    if(ecsChecks[carNumber+3]->isChecked()) {
        // stop moving and provide text and display message
        elevators[carNumber]->setDirection(STOP);
        elevators[carNumber]->audioSystem.longRing(carNumber);
        elevators[carNumber]->display.setDisplay("Overload. Door stays opened.");

        return false;
    }
    return true;
}

void ElevatorControlSystem::fireSignal(bool on) {

    // if it's off
    if(on == false) {
        bss.fire.setStatus(false);
        return;
    }

    for(int i=0; i<ELEVATORNUM; i++) {
        // erase all destinations and add the safe floor
        destinations[i].clear();
        destinations[i].append(1+ECSCOMMAND);

        // indicate direction
        if(elevators[i]->getCurrentFloor() == 1)
            elevators[i]->setDirection(STOP);
        else
            elevators[i]->setDirection(DOWN);

        QString text = "Fire detected. Moving to safe floor. Once reached, please evacuate immediately.";

        // display and AS
        elevators[i]->display.setDisplay(text);
        elevators[i]->audioSystem.playWarning(text, i);
        bss.fire.setStatus(true);
    }
}

void ElevatorControlSystem::fireButton(int carNumber) {
    // set fire button on
    elevators[carNumber]->fireButton.setStatus(true);
    fireSignal(true);
}

void ElevatorControlSystem::powerOutage(bool on) {

    // if it's off
    if(on == false) {
        bss.powerOutage.setStatus(false);
        return;
    }

    for(int i=0; i<ELEVATORNUM; i++) {
        // erase all destinations and add the safe floor
        destinations[i].clear();
        destinations[i].append(1+ECSCOMMAND);

        // indicate direction
        if(elevators[i]->getCurrentFloor() == 1)
            elevators[i]->setDirection(STOP);
        else
            elevators[i]->setDirection(DOWN);

        QString text = "Power outage. Moving to safe floor. Once reached, please evacuate.";

        // display and AS
        elevators[i]->display.setDisplay(text);
        elevators[i]->audioSystem.playWarning(text, i);
        bss.powerOutage.setStatus(true);
    }
}

void ElevatorControlSystem::helpButton(int carNumber) {
    // set help button on
    elevators[carNumber]->helpButton.setStatus(true);
    setElevator(carNumber);

    // connect to building safety service
    bss.enableVoiceConnection(carNumber);
}

void ElevatorControlSystem::helpDisconnected(int carNumber) {
    // set hlep button off
    elevators[carNumber]->helpButton.setStatus(false);
}

void ElevatorControlSystem::setElevator(int carNumber) {

    // set display
    displays[0]->setText(elevators[carNumber]->display.getDisplay());

    // set buttons
    for(int i=0; i<FLOORNUM; i++) {
        // floor buttons pressed
        if(!destinations[carNumber].isEmpty() && destinations[carNumber][0] == i+1){
            elvLevelBtns[i]->setStyleSheet("* { background-color: rgb(179,179,179) }");
        } else {
            elvLevelBtns[i]->setStyleSheet("* { background-color: rgb(243,243,243) }");
        }
    }

    // for fire button
    if (elevators[carNumber]->fireButton.getStatus()) {
        fireHelp[0]->setStyleSheet("* { background-color: rgb(255,57,77) }");
    } else {
        fireHelp[0]->setStyleSheet("* { background-color: rgb(243,243,243) }");
    }

    // for help button
    if (elevators[carNumber]->helpButton.getStatus()) {
        fireHelp[1]->setStyleSheet("* { background-color: rgb(255,255,0) }");
    } else {
        fireHelp[1]->setStyleSheet("* { background-color: rgb(243,243,243) }");
    }

}

void ElevatorControlSystem::setFloor(int floorNumber) {
    // set display
    displays[1]->setText(floors[floorNumber-1]->display.getDisplay());

    // set buttons
    if(floors[floorNumber-1]->upBtn.getStatus())
        floorButtons[0]->setStyleSheet("* { background-color: rgb(179,179,179) }");
    else
        floorButtons[0]->setStyleSheet("* { background-color: rgb(243,243,243) }");

    if(floors[floorNumber-1]->downBtn.getStatus())
        floorButtons[1]->setStyleSheet("* { background-color: rgb(179,179,179) }");
    else
        floorButtons[1]->setStyleSheet("* { background-color: rgb(243,243,243) }");
}

void ElevatorControlSystem::setElvCurrFloor(int floorNumber, int carNumber) {
    elevators[carNumber]->setCurrentFloor(floorNumber);
}

int ElevatorControlSystem::getElevatorDirection(int carNumber) {
    return elevators[carNumber]->getDirection();
}
