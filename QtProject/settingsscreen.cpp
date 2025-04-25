#include "settingsscreen.h"
#include "ui_settingsscreen.h"

SettingsScreen::SettingsScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScreen)
{
    ui->setupUi(this);
    ui->frame_2->setVisible(false);

    // Create and add LoggerScreen to stackedWidget
    LoggerScreen* loggerScreen = new LoggerScreen(this);
    ui->stackedWidget->addWidget(loggerScreen);
    
    // Store a pointer to the LoggerScreen
    this->loggerScreen = loggerScreen;
    
    // Connect buttons to slots
    connect(ui->mypump, &QPushButton::clicked, this, &SettingsScreen::showMyPumpScreen);
    connect(ui->activity, &QPushButton::clicked, this, &SettingsScreen::showActivityScreen);
    connect(ui->mycgm, &QPushButton::clicked, this, &SettingsScreen::showCGMScreen);
    connect(ui->back_btn, &QPushButton::clicked, this, &SettingsScreen::showMainOptions);
    connect(ui->history, &QPushButton::clicked, this, &SettingsScreen::showHistoryScreen);
    connect(ui->stopinsulin, &QPushButton::clicked, this, &SettingsScreen::stopInsulinDelivery);

    // Connect MyPumpScreen signals
    connect(ui->mypumpscreen, &MyPumpScreen::showProfileScreen, this, &SettingsScreen::showProfileScreen);
    connect(ui->mypumpscreen, &MyPumpScreen::showControlIqScreen, this, &SettingsScreen::showControlIqScreen);
    connect(ui->mypumpscreen, &MyPumpScreen::showControlIqScreen, this, &SettingsScreen::showControlIqScreen);
    
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
}

void SettingsScreen::stopInsulinDelivery()
{
    Data::getInstance().setStopInsulin(!Data::getInstance().getStopInsulin());
    if (Data::getInstance().getStopInsulin()) {
        ui->stopinsulin->setText("Resume Insulin Delivery");
        Data::getInstance().setStopInsulin(true); 
    } else {
        ui->stopinsulin->setText("Stop Insulin Delivery");
        Data::getInstance().setStopInsulin(false); 
    }
}

void SettingsScreen::showMyPumpScreen()
{
    ui->frame->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->mypumpscreen);
    ui->frame_2->setVisible(true);
}

void SettingsScreen::showActivityScreen()
{
    ui->frame->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->activityscreen);
    ui->frame_2->setVisible(true);

    reloadAcitivySettings();
}

void SettingsScreen::showMainOptions()
{
    ui->frame->setVisible(true);
    ui->frame_2->setVisible(false);
}

void SettingsScreen::showProfileScreen()
{
    ui->stackedWidget->setCurrentWidget(ui->profilescreen);
}

void SettingsScreen::showControlIqScreen()
{
    ui->stackedWidget->setCurrentWidget(ui->controliqscreen);
}

void SettingsScreen::showCGMScreen()
{
    ui->frame->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->cgm);
    ui->frame_2->setVisible(true);
}

void SettingsScreen::showHistoryScreen()
{
    ui->frame->setVisible(false);
    ui->stackedWidget->setCurrentWidget(loggerScreen);
    ui->frame_2->setVisible(true);
}

void SettingsScreen::reloadAcitivySettings()
{
    ui->activityscreen->loadSettings();
}
