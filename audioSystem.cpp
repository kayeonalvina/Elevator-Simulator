#include "audioSystem.h"

AudioSystem::AudioSystem(){}

void AudioSystem::ring(int carNumber) {
    qInfo() << "Elevator " << QString::number(carNumber+1) << ": ring! ";

}
void AudioSystem::longRing(int carNumber) {
    qInfo() << "Elevator " << QString::number(carNumber+1) << ": ringggggggggggggg";
}
void AudioSystem::playWarning(QString message, int carNumber) {
    qInfo() << "Elevator " << QString::number(carNumber+1) << message;
}
