#ifndef BOLUSSCREEN_H
#define BOLUSSCREEN_H

#include <QWidget>
#include "data.h"

namespace Ui {
class BolusScreen;
}

class BolusScreen : public QWidget
{
    Q_OBJECT

public:
    explicit BolusScreen(QWidget *parent = nullptr);
    void setBloodGlucoseLabel(float value);

    ~BolusScreen();



private slots:
    void on_continue1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_4_clicked();

    void on_deliverLater_valueChanged(int value);

    void on_duration_valueChanged(int value);

    void on_deliverNow_valueChanged(int value);

signals:
    void navigateToHomeScreen();
    void bolusRequested(float cgmValue);

private:
    float carbs;
    float bloodGlucose;
    float foodBolus;
    float correctionBolus;

    //Instance of data class
    Data* data;

    //Timed Bolus
    QTimer* bolusTimer = nullptr;
    float bolusAmountToDeliver = 0;
    float bolusDelivered = 0;
    int bolusSteps = 0;
    int bolusStep = 0;
    float bolusStepAmount = 0;

    float targetGlucose;
    float glucoseChange = 0;


    Ui::BolusScreen *ui;
    void startTimedBolus(float units, int durationSeconds);
};

#endif // BOLUSSCREEN_H
