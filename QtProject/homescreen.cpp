#include "homescreen.h"
#include "ui_homescreen.h"
#include "data.h"
#include <QDateTime>

HomeScreen::HomeScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeScreen)
{
    ui->setupUi(this);

    // initialize the batteryBar
    ui->batteryBar->setMinimum(0);
    ui->batteryBar->setMaximum(100);
    ui->batteryBar->setValue(Data::getInstance().getBatteryLevel());
    connect(&Data::getInstance(), &Data::batteryLevelChanged, this, &HomeScreen::updateBatteryDisplay);
    connect(&Data::getInstance(), &Data::usbChargerConnected, this, &HomeScreen::updateBatteryDisplay);

    // initialize the insulinBar
    ui->insulinBar->setMinimum(0);
    ui->insulinBar->setMaximum(300);
    ui->insulinBar->setValue(Data::getInstance().getStoredInsulinLevel());
    connect(&Data::getInstance(), &Data::storedInsulinLevelChanged, this, &HomeScreen::updateInsulinDisplay);

    // initialize the date time display
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &HomeScreen::updateDateTimeDisplay);
    dateTimeTimer->start(1000);
    updateDateTimeDisplay();

    // initialize the options and bolus buttons
    connect(ui->optionsButton, &QPushButton::clicked, this, [this]() {
        Data::getInstance().setCurrentScreen(Data::SettingsScreenType);
    });
    connect(ui->bolusButton, &QPushButton::clicked, this, [this]() {
        Data::getInstance().setCurrentScreen(Data::BolusScreenType);
    });

    // initialize the insulin on board label
    ui->insulinOnBoardLabel->setText("Insulin on Board: " + QString::number(Data::getInstance().getInsulinOnBoard()) + " u");
    connect(&Data::getInstance(), &Data::insulinOnBoardChanged, this, &HomeScreen::updateInsulinOnBoardDisplay);

    // initialize the CGM label
    connect(&Data::getInstance(), &Data::cgmValueChanged, this, &HomeScreen::updateCgmDisplay);
    connect(&Data::getInstance(), &Data::cgmConnectionChanged, this, &HomeScreen::updateCgmDisplay);
    updateCgmDisplay();

    // connect the hour toggle button (goes between 1, 3, and 6 hours)
    connect(ui->hourButton, &QPushButton::clicked, this, [this]() {
        // change the text on the button to 1 -> 3 -> 6 -> 1
        QString currentText = ui->hourButton->text();
        if (currentText == "1h") {
            ui->hourButton->setText("3h");
            Data::getInstance().setGraphHours(3);
        } else if (currentText == "3h") {
            ui->hourButton->setText("6h");
            Data::getInstance().setGraphHours(6);
        } else {
            ui->hourButton->setText("1h");
            Data::getInstance().setGraphHours(1);
        }
        ui->graph->update();
    });

    //Set Graph to update and add new CGM value to the readings vector every second

    //Set Graph to Update every second a
    QTimer *graphTimer = new QTimer(this);
    connect(graphTimer, &QTimer::timeout, this, [this]() {
        if(!Data::getInstance().getTimedBolusStatus())
            Data::getInstance().predictGlucose();
        Data::getInstance().addValueToCgmReadings();
        double cgmValue = Data::getInstance().getCgmValue();
        // Check CGM values for alerts
        if (cgmValue >= 11.4 && !highGlucoseAlertShown) {
            PopupManager::showPopup("High Glucose Alert", 
                QString("Current glucose level is %1 mmol/L").arg(cgmValue));
            highGlucoseAlertShown = true;
            lowGlucoseAlertShown = false;  // Reset low alert
        } 
        else if (cgmValue <= 4.4 && !lowGlucoseAlertShown) {
            PopupManager::showPopup("Low Glucose Alert", 
                QString("Current glucose level is %1 mmol/L").arg(cgmValue));
            lowGlucoseAlertShown = true;
            highGlucoseAlertShown = false;  // Reset high alert
        }
        else if (cgmValue > 4.4 && cgmValue < 11.4) {
            // Reset both flags when glucose returns to normal range
            highGlucoseAlertShown = false;
            lowGlucoseAlertShown = false;
        }

        ui->graph->update();

    });
    graphTimer->start(1000);
}

HomeScreen::~HomeScreen()
{
    delete dateTimeTimer;
    delete ui;
}

void HomeScreen::updateBatteryDisplay()
{
    ui->batteryBar->setValue(Data::getInstance().getBatteryLevel());
    QString batteryText = QString::number(Data::getInstance().getBatteryLevel()) + " %";
    ui->batteryText->setText(batteryText);

    // make the bar black if discharging, green if charging
    if (Data::getInstance().isCharging()) {
        ui->batteryBar->setStyleSheet("QProgressBar::chunk { background-color: green; width: 10px; margin: 0.5px; }");
    } else {
        ui->batteryBar->setStyleSheet("QProgressBar::chunk { background-color: black; width: 10px; margin: 0.5px; }");
        if (Data::getInstance().getBatteryLevel() <= 20) {
            ui->batteryBar->setStyleSheet("QProgressBar::chunk { background-color: red; width: 10px; margin: 0.5px; }");
        }
    }
}

void HomeScreen::updateInsulinDisplay()
{
    ui->insulinBar->setValue(Data::getInstance().getStoredInsulinLevel());
    QString insulinText = QString::number(Data::getInstance().getStoredInsulinLevel()) + " u";
    ui->insulinText->setText(insulinText);
}

void HomeScreen::updateDateTimeDisplay()
{
    Data::getInstance().advanceSimulatedTime(5);
    QDateTime simulatedDateTime = Data::getInstance().getSimulatedDateTime(); // Get simulated time from Data
    QString dateTimeText = simulatedDateTime.toString("h:mm AP\nMMM dd");
    ui->dateTimeLabel->setText(dateTimeText);
}

void HomeScreen::updateInsulinOnBoardDisplay()
{
    QString insulinOnBoardText = "Insulin on Board: " + QString::number(Data::getInstance().getInsulinOnBoard()) + " u";
    ui->insulinOnBoardLabel->setText(insulinOnBoardText);
}

void HomeScreen::updateCgmDisplay()
{
    if (Data::getInstance().isCgmConnected()) {
        qDebug() << "CGM is connected, setting CGM value label to:" << Data::getInstance().getCgmValue();
        ui->CGMValueLabel->setText(QString::number(Data::getInstance().getCgmValue(), 'f', 2));
    } else {
        ui->CGMValueLabel->setText("X");
    }
}
