#include "cgmscreen.h"
#include "ui_cgmscreen.h"


cgmscreen::cgmscreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cgmscreen),
    dataInstance(Data::getInstance())
{
    ui->setupUi(this);
    QStringList cgmTypes = {"Dexcom G6", "Dexcom G7"};
    ui->sensortype->addItems(cgmTypes);


    // Make button checkable
    ui->startsensor->setCheckable(true);
    
    // Creating a lambda function to set the boolean value to be something that vinoved making antoher 
    connect(ui->startsensor, &QPushButton::toggled, this, [this]() {
        QString currentText = ui->startsensor->text();
        if (currentText == "Turn Off Sensor") {
            ui->startsensor->setText("Start Sensor");
            dataInstance.clearCgmConfigs();
        } else {
            ui->startsensor->setText("Turn Off Sensor");
            // Adding the different cgm configs
            QString cgmType = ui->sensortype->currentText();
            if (ui->transmitterid->toPlainText().isEmpty() || ui->pairingcode->toPlainText().isEmpty()) {
                qDebug() << "Transmitter ID or Pairing Code is empty";
                return;
            }
            
        }
    });
}

cgmscreen::~cgmscreen()
{
    delete ui;
}
