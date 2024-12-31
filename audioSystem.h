#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QString>
#include <QDebug>

class AudioSystem
{
    public:
        AudioSystem();

        void ring(int carNumber); // when it  gets to floor
        void longRing(int carNumber); // safety warning
        void playWarning(QString message, int carNumber); // verbally send out warnings
};

#endif // AUDIOSYSTEM_H
