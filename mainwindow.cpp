#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defs.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // initialization of arrays to be sent to ECS
    elvFloorNums[0] = ui->elv1_floorNum;
    elvFloorNums[1] = ui->elv2_floorNum;
    elvFloorNums[2] = ui->elv3_floorNum;

    displays[0] = ui->elevator_display;
    displays[1] = ui->floor_display;
    displays[2] = ui->BSS_help_label;

    floorBtns[0] = ui->floor_up_btn;
    floorBtns[1] = ui->floor_down_btn;

    elvLevelbtns[0] = ui->elv_f1_btn;
    elvLevelbtns[1] = ui->elv_f2_btn;
    elvLevelbtns[2] = ui->elv_f3_btn;
    elvLevelbtns[3] = ui->elv_f4_btn;
    elvLevelbtns[4] = ui->elv_f5_btn;
    elvLevelbtns[5] = ui->elv_f6_btn;
    elvLevelbtns[6] = ui->elv_f7_btn;

    ecsCheck[0] = ui->elv1_door;
    ecsCheck[1] = ui->elv2_door;
    ecsCheck[2] = ui->elv3_door;
    ecsCheck[3] = ui->elv1_overload; // to get overload carNumber+3
    ecsCheck[4] = ui->elv2_overload;
    ecsCheck[5] = ui->elv3_overload;

    fireHelp[0] = ui->elv_fire_btn;
    fireHelp[1] = ui->elv_help_btn;

    cabFloorNumChosen[0] = ui->elvNum_box;
    cabFloorNumChosen[1] = ui->floorNum_box;

    helpResponses[0] = ui->building_response;
    helpResponses[1] = ui->passenger_response;
    helpResponses[2] = ui->disconnect_response;

    ecs = new ElevatorControlSystem(elvFloorNums, floorBtns, elvLevelbtns, displays, ecsCheck, fireHelp, cabFloorNumChosen, helpResponses);

    // start by disabling it since first floor is default
    ui->floor_down_btn->setDisabled(true);

    // start by disabling check boxes for help as the button is not pressed
    ui->building_response->setDisabled(true);
    ui->passenger_response->setDisabled(true);
    ui->disconnect_response->setDisabled(true);

    for(int i=0; i<3; i++)
        displays[i]->setWordWrap(true);

    // elevator frame
    for(int i=0; i<FLOORNUM; i++) {
        connect(elvLevelbtns[i], &QPushButton::clicked, this, [this, i](){MainWindow::elevatorFloorButton(i);});
    }
    connect(ui->elv_fire_btn, &QPushButton::clicked, this, &MainWindow::elevatorFireButton);
    connect(ui->elv_help_btn, &QPushButton::clicked, this, &MainWindow::elevatorHelpButton);
    connect(ui->elv_open_btn, &QPushButton::pressed, this, &MainWindow::elevatorOpenButtonPressed);
    connect(ui->elv_open_btn, &QPushButton::released, this, &MainWindow::elevatorOpenButtonReleased);
    connect(ui->elv_close_btn, &QPushButton::clicked, this, &MainWindow::elevatorCloseButton);
    connect(ui->elvNum_box, SIGNAL(valueChanged(int)), this, SLOT(elevatorNumberSet()));

    // floor frame
    connect(ui->floor_up_btn, SIGNAL(clicked()), this, SLOT(floorUpButton()));
    connect(ui->floor_down_btn, &QPushButton::clicked, this, &MainWindow::floorDownButton);
    connect(ui->floorNum_box, SIGNAL(valueChanged(int)), this, SLOT(floorNumberSet()));

    // BSS frame
    connect(ui->BSS_submit_btn, &QPushButton::clicked, this, &MainWindow::BSSSubmitButton);
    connect(ui->disconnect_response, &QCheckBox::clicked, this, &MainWindow::BSSDisconnectCheckBox);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::elevatorFloorButton(int i)
{
    // add the selected floor to elevator
    ecs->addDestination(i+1, ui->elvNum_box->value()-1);
}

void MainWindow::elevatorHelpButton()
{
    // allow check boxes to be checked
    ui->building_response->setEnabled(true);
    ui->passenger_response->setEnabled(true);
    ui->disconnect_response->setEnabled(true);

    // disable from moving to another elevator
    ui->elvNum_box->setReadOnly(true);

    ecs->helpButton(ui->elvNum_box->value()-1);
}


void MainWindow::elevatorFireButton()
{
    ecs->fireButton(ui->elvNum_box->value()-1);

    // indicate to building that there is a fire
    ui->BSS_fire->setChecked(true);
}


void MainWindow::elevatorOpenButtonPressed()
{
    ui->elv_open_btn->setStyleSheet("* { background-color: rgb(179,179,179) }");
    ecs->openDoor(ui->elvNum_box->value()-1, true);

}


void MainWindow::elevatorOpenButtonReleased()
{
    ui->elv_open_btn->setStyleSheet("* { background-color: rgb(243,243,243) }");
    ecs->openDoor(ui->elvNum_box->value()-1, false);
}

void MainWindow::elevatorCloseButton() {
    ecs->closeDoor(ui->elvNum_box->value()-1);
}

void MainWindow::elevatorNumberSet()
{
    ui->elvNum_box->setMinimum(1);
    ui->elvNum_box->setMaximum(ELEVATORNUM);

    for(int i=0; i<FLOORNUM; i++) {
        elvLevelbtns[i]->setStyleSheet("* { background-color: rgb(243,243,243) }");
    }

    // set elevator panel after retreiving the cab number
    ecs->setElevator(ui->elvNum_box->value()-1);
}

void MainWindow::floorUpButton()
{
    // request for elevator
    ecs->elevatorRequest(ui->floorNum_box->value(), 1);
}

void MainWindow::floorDownButton()
{
    // request for elevator
    ecs->elevatorRequest(ui->floorNum_box->value(), -1);
}

void MainWindow::floorNumberSet()
{
    ui->floorNum_box->setMinimum(1);
    ui->floorNum_box->setMaximum(FLOORNUM);

    ui->floor_up_btn->setStyleSheet("* { background-color: rgb(243,243,243) }");
    ui->floor_down_btn->setStyleSheet("* { background-color: rgb(243,243,243) }");

    // if the highest floor, disable up button
    if(ui->floorNum_box->value() == FLOORNUM)
        ui->floor_up_btn->setDisabled(true);
    else
        ui->floor_up_btn->setEnabled(true);

    // if the lowest floor, disable down button
    if(ui->floorNum_box->value() == 1)
        ui->floor_down_btn->setDisabled(true);
    else
        ui->floor_down_btn->setEnabled(true);

    // set floor panel after retreiving the floor number
    ecs->setFloor(ui->floorNum_box->value());
}

void MainWindow::BSSSubmitButton()
{
    // if checked, notify fire
    if(ui->BSS_fire->isChecked()) {
        ecs->fireSignal(true);
    } else {
        ecs->fireSignal(false);
    }

    // if checked, notify power outage
    if(ui->BSS_powerOut->isChecked()) {
        ecs->powerOutage(true);
    } else {
        ecs->powerOutage(false);
    }
}

void MainWindow::BSSDisconnectCheckBox()
{
    // disconnect help
    ecs->helpDisconnected(ui->elvNum_box->value()-1);

    // uncheck
    for (int i = 0; i < 3; ++i) {
        if (helpResponses[i]->isChecked()) {
            helpResponses[i]->setChecked(false);
        }
    }

    // disable all
    helpResponses[0]->setDisabled(true);
    helpResponses[1]->setDisabled(true);
    helpResponses[2]->setDisabled(true);

    ui->BSS_help_label->setText("");

    // enable choosing another elevator
    ui->elvNum_box->setReadOnly(false);
}
