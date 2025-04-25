#include "bolusscreen.h"
#include "ui_bolusscreen.h"
#include "logger.h"
#include <QMessageBox>

BolusScreen::BolusScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BolusScreen)
{
    //init data instance
    data = &(Data::getInstance());
    ui->setupUi(this);
    ui->bloodGlucose->setText(QString::number(data->getCgmValue()));
}

BolusScreen::~BolusScreen()
{
    delete ui;
}

//SetBloodGlucoseLevel
void BolusScreen::setBloodGlucoseLabel(float value)
{
    ui->bloodGlucose->setText(QString::number(value));
}

//Deliver Bolus over time
void BolusScreen::startTimedBolus(float units, int durationSeconds)
{




    if (bolusTimer && bolusTimer->isActive()) {
        bolusTimer->stop();
    }
    if (!bolusTimer) {
        bolusTimer = new QTimer(this);
        connect(bolusTimer, &QTimer::timeout, this, [this]() {
            if (!data->getStopInsulin() && bolusStep < bolusSteps && data->getStoredInsulinLevel() >= bolusStepAmount) {

                //Adjusting Insulin
                data->setStoredInsulinLevel(data->getStoredInsulinLevel() - bolusStepAmount);
                bolusDelivered += bolusStepAmount;
                Data::getInstance().setInsulinOnBoard(Data::getInstance().getInsulinOnBoard() + bolusStepAmount);

                //Adjusting Glucose

                Data::getInstance().setCgmValue(Data::getInstance().getCgmValue()-  glucoseChange);


                bolusStep++;
            }
            if (bolusStep >= bolusSteps || data->getStoredInsulinLevel() < bolusStepAmount) {
                // Deliver any remaining insulin due to rounding errors
                float remaining = bolusAmountToDeliver - bolusDelivered;
                if (remaining > 0 && data->getStoredInsulinLevel() >= remaining) {
                    data->setStoredInsulinLevel(data->getStoredInsulinLevel() - remaining);
                }
                bolusTimer->stop();
                bolusDelivered = 0;
                bolusStep = 0;
                bolusSteps = 0;
                bolusAmountToDeliver = 0;
                glucoseChange = 0;
                Data::getInstance().setTimedBolusStatus(false);
            }
        });
    }
    Data::getInstance().setTimedBolusStatus(true);
    QString s = data->getCurrentProfile();
    Profile p = data->getProfile(s);
    bolusAmountToDeliver = units;
    bolusDelivered = 0;
    bolusSteps = durationSeconds;
    glucoseChange = (Data::getInstance().getCgmValue() - p.targetGlucose) / bolusSteps;
    bolusStep = 0;
    bolusStepAmount = (durationSeconds > 0) ? units / durationSeconds : 0;
    if (bolusStepAmount > 0 && bolusSteps > 0) {
        bolusTimer->start(1000); // 1 second interval
    }
}


void BolusScreen::on_continue1_clicked()
{
    carbs = ui->carb->text().toFloat();
    bloodGlucose = ui->bloodGlucose->text().toFloat();
    ui->stackedWidget->setCurrentIndex(1);
    QString s = data->getCurrentProfile();
    Profile p = data->getProfile(s);

    if(p.targetGlucose > bloodGlucose) {
        QMessageBox msgBox;
        QString s = "ERROR: New Glucose value is lower than target glucose";
        msgBox.setText(s);
        msgBox.exec();
    }
    foodBolus = carbs/p.carbRatio;
    correctionBolus = (bloodGlucose - p.targetGlucose)/p.correctionFactor;
    ui->correctionValue->setText(QString::number(correctionBolus));
    ui->foodValue->setText(QString::number(foodBolus));
    ui->totalDose->setText("Dose: " + QString::number(correctionBolus + foodBolus - data->getInsulinOnBoard()) + "u");
}

//Back button on correction/food value screen
void BolusScreen::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//Continue button on correction/food value screen, values get updated here
void BolusScreen::on_pushButton_3_clicked()
{
        if (ui->extend->isChecked()) {

            ui->stackedWidget->setCurrentIndex(2);
            return;
        } else {
            QMessageBox msgBox;
            QString profile = data->getCurrentProfile();
            Profile p = data->getProfile(profile);

            float totalBolus = correctionBolus + foodBolus - data->getInsulinOnBoard();
            QString s = "Total Bolus = " + QString::number(correctionBolus + foodBolus ) +" - " +QString::number(data->getInsulinOnBoard()) + " = " +
                    QString::number(totalBolus) +"u";
            msgBox.setText(s);
            msgBox.exec();
            data->deliverBolus(correctionBolus + foodBolus - data->getInsulinOnBoard());
            data->setInsulinOnBoard(correctionBolus + foodBolus);
            data->setCgmValue(p.targetGlucose);
            Logger::info("Delivered instant bolus of " + QString::number(totalBolus, 'f', 2) + "u");
            Data::getInstance().setCurrentScreen(Data::HomeScreenType);
            return;
        }
    // Go to home screen


}

//Continue button on dial screen
void BolusScreen::on_pushButton_6_clicked()
{

    QString s = data->getCurrentProfile();
    Profile p = data->getProfile(s);

    //Calculate amount of Bolus to Deliver
    float totalBolus = correctionBolus + foodBolus - data->getInsulinOnBoard();
    float deliverNowPercentage = (float) ui->deliverNow->value() / 100.0f;
    float deliverLaterPercentage = (float) ui->deliverLater->value() / 100.0f;
    int duration = ui->duration->value();
    int durationSeconds = duration * 12;

    //Deliver Bolus
    data->deliverBolus(totalBolus * deliverNowPercentage);
    float currentCGMValue = Data::getInstance().getCgmValue() - (Data::getInstance().getCgmValue() - p.targetGlucose) * deliverNowPercentage;
    Data::getInstance().setCgmValue(currentCGMValue);
    startTimedBolus(totalBolus * deliverLaterPercentage, durationSeconds);

    //Return to Home Screen
    Logger::info("Delivered instant bolus of " + QString::number(totalBolus * deliverNowPercentage, 'f', 2) + "u");
    Logger::info("Delivered timed bolus of " + QString::number(totalBolus * deliverLaterPercentage, 'f', 2) + "u over " +
                 QString::number(duration) + " simulation hours (" + QString::number(duration * 12) + "real seconds)");
    Data::getInstance().setCurrentScreen(Data::HomeScreenType);
}

//Back button on dial screen
void BolusScreen::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}




void BolusScreen::on_deliverNow_valueChanged(int value)
{
    ui->deliverNowLabel->setText(QString::number(value) + "%");
    ui->deliverLaterLabel->setText(QString::number(100-value) + "%");
    ui->deliveredNow->setText(QString::number(value*(correctionBolus+foodBolus)/100)+"u");
    ui->deliveredLater->setText(QString::number((100-value)*(correctionBolus+foodBolus)/100)+"u");
    ui->deliverLater->setValue(100-value);

}


void BolusScreen::on_deliverLater_valueChanged(int value)
{
    ui->deliverNowLabel->setText(QString::number(100-value) + "%");
    ui->deliverLaterLabel->setText(QString::number(value) + "%");
    ui->deliveredLater->setText(QString::number(value*(correctionBolus+foodBolus)/100)+"u");
    ui->deliveredNow->setText(QString::number((100-value)*(correctionBolus+foodBolus)/100)+"u");
    ui->deliverNow->setValue(100-value);
}


void BolusScreen::on_duration_valueChanged(int value)
{
    ui->durationLabel->setText(QString::number(value) + " hrs");
}

