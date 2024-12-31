#ifndef BUILDINGSAFETYSERVICE_H
#define BUILDINGSAFETYSERVICE_H

#include <QString>
#include <QTimer>
#include <iostream>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

#include "button.h"
#include "defs.h"

class BuildingSafetyService: public QObject
{
    public:
        BuildingSafetyService(QLabel* label, QCheckBox** helpResponses);

        void enableVoiceConnection(int carNumber); // voice connection with ECS and elevator
        void connected(); // after connection

        Button powerOutage; // true if there's a power out
        Button fire; // ture if fire alarm signaled

    private:
        QLabel* label;
        QCheckBox** helpResponses;
        QPushButton* helpButton;
};

#endif // BUILDINGSAFETYSERVICE_H
