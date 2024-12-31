#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <QCheckBox>
#include <QSpinBox>
#include "elevatorControlSystem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ElevatorControlSystem* ecs;

private slots:
    void elevatorFloorButton(int);
    void elevatorHelpButton();
    void elevatorFireButton();
    void elevatorOpenButtonPressed();
    void elevatorOpenButtonReleased();
    void elevatorCloseButton();
    void elevatorNumberSet();

    void floorUpButton();
    void floorDownButton();
    void floorNumberSet();

    void BSSSubmitButton();
    void BSSDisconnectCheckBox();


private:
    Ui::MainWindow *ui;
    QLabel* elvFloorNums[ELEVATORNUM];
    QLabel* displays[3];
    QPushButton* floorBtns[2];
    QPushButton* elvLevelbtns[7];
    QCheckBox* ecsCheck[6];
    QPushButton* fireHelp[2];
    QSpinBox* cabFloorNumChosen[2];
    QCheckBox* helpResponses[3];
};
#endif // MAINWINDOW_H
