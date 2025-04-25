#include "mypumpscreen.h"
#include "ui_mypumpscreen.h"

MyPumpScreen::MyPumpScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyPumpScreen)
{
    ui->setupUi(this);
    connect(ui->profilesbutton, &QPushButton::clicked, this, &MyPumpScreen::onProfilesButtonClicked);
    connect(ui->controliqbutton, &QPushButton::clicked, this, &MyPumpScreen::onControlIqButtonClicked);
}

MyPumpScreen::~MyPumpScreen()
{
    delete ui;
}

void MyPumpScreen::onProfilesButtonClicked()
{
    emit showProfileScreen();
}

void MyPumpScreen::onControlIqButtonClicked()
{
    emit showControlIqScreen();
}
