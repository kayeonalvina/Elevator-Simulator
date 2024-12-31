#include "buildingSafetyService.h"
#include <QDebug>

BuildingSafetyService::BuildingSafetyService(QLabel* label, QCheckBox** helpResponses)
    : label(label), helpResponses(helpResponses){}

void BuildingSafetyService::enableVoiceConnection(int carNumber) {
    label->setText("Enabling voice connection...");

    qInfo() << "To indicate presence, please check \"building\" within 5 seconds.";

    // to wait for building side to respond in 5 seconds
    QTimer* buildingTimer = new QTimer(this);
    connect(buildingTimer, &QTimer::timeout, this, [=]() {
        buildingTimer->stop();
        buildingTimer->deleteLater();

        // if "building" is checked
        if(helpResponses[0]->isChecked()) {
            qInfo() << "To indicate presence, please check \"passenger\" within 5 seconds.";

            // to wait for passenger side to respond in 5 seconds
            QTimer* passengerTimer = new QTimer(this);
            connect(passengerTimer, &QTimer::timeout, this, [=]() {
                passengerTimer->stop();
                passengerTimer->deleteLater();

                // if "passenger" is checked
                if(helpResponses[1]->isChecked()) {
                    label->setText("Connected to Elevator " + QString::number(carNumber+1));
                    connected();
                } else {
                    label->setText("No response from passenger. Calling 911...");

                    // calling time
                    QTimer* callingTimer = new QTimer(this);
                    connect(callingTimer, &QTimer::timeout, this, [=]() {
                        callingTimer->stop();
                        callingTimer->deleteLater();

                        label->setText("Connected to 911");
                        connected();
                    });
                    callingTimer->start(CALLING);
                }

            });
            // wait for passenger to respond
            passengerTimer->start(RESPONSE);
        } else {
            label->setText("No response from building. Calling 911...");

            // calling time
            QTimer* callingTimer = new QTimer(this);
            connect(callingTimer, &QTimer::timeout, this, [=]() {
                callingTimer->stop();
                callingTimer->deleteLater();

                label->setText("Connected to 911");
                connected();
            });
            callingTimer->start(CALLING);
        }
    });

    // wait for building to respond for 5 seconds
    buildingTimer->start(RESPONSE);
}

void BuildingSafetyService::connected() {
    qInfo() << "To disable voice connection, please check \"disconnect\".";

    // update when "disconnect" is checked
    QTimer* waitingTimer = new QTimer(this);
    connect(waitingTimer, &QTimer::timeout, this, [=]() {
        waitingTimer->stop();
        waitingTimer->deleteLater();
    });
    waitingTimer->start(100);
}
