#include "controliqscreen.h"
#include "ui_controliqscreen.h"

ControlIqScreen::ControlIqScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlIqScreen),
    dataInstance(Data::getInstance())
{
    ui->setupUi(this);
    ui->controliqon->setCheckable(true);
    ui->weight->setEnabled(false);
    ui->weightvalue->setMinimum(25);
    ui->weightvalue->setMaximum(140);
    ui->dailyinsulin->setEnabled(false);
    ui->totaldailyinsulinvalue->setMinimum(10);
    ui->totaldailyinsulinvalue->setMaximum(100);
    
    connect(ui->controliqon, &QPushButton::toggled, this, &ControlIqScreen::toggleControlIq);
    connect(ui->saveButton, &QPushButton::clicked, this, &ControlIqScreen::saveControlIqSetting);
}

void ControlIqScreen::toggleControlIq(bool checked)
{
    if (checked) {
        // Code to turn on Control IQ
        ui->controliqon->setText("Disable Control IQ");
        ui->weight->setEnabled(true);
        ui->dailyinsulin->setEnabled(true);
        dataInstance.setControlIqEnabled(true);
    } else {
        // Code to turn off Control IQ
       
        ui->controliqon->setText("Enable Control IQ");
        ui->weight->setEnabled(false);
        ui->dailyinsulin->setEnabled(false);
        dataInstance.setControlIqEnabled(false);
    }
}

void ControlIqScreen::saveControlIqSetting(){
    // Code to save Control IQ settings
    bool controlIqEnabled = ui->controliqon->isChecked();
    double weight = ui->weightvalue->value();
    double dailyInsulin = ui->totaldailyinsulinvalue->value();
    ControlIqSettings settings;
    settings.weight = weight;
    settings.dailyInsulin = dailyInsulin;
    if (!controlIqEnabled) {
        return;  // Don't save settings if ControlIQ is disabled
    }
    // Assuming Data class has a method to save these settings
    Data::getInstance().setControlIqSettings(settings);
    qDebug() << "Control IQ settings saved: " << "Weight:" << weight << ", Daily Insulin:" << dailyInsulin;
    QMessageBox::information(this, "Success", "Control IQ settings have been saved successfully.");
}



ControlIqScreen::~ControlIqScreen()
{
    delete ui;
}
