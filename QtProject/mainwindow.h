#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QTimer>
#include <QDoubleValidator>
#include "data.h"
#include "popupmanager.h"
#include "logger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void checkBatteryLevelPopup();

private slots:
    void handleCurrentScreenChanged(Data::ScreenType newScreen);
    void onPowerButtonPressed();
    void onPowerButtonReleased();
    void onPowerButtonHeldTimeout();
    void onBatteryTimerTimeout();

private:
    Ui::MainWindow *ui;
    QIntValidator *batteryLevelValidator;
    QDoubleValidator *storedInsulinLevelValidator;
    QDoubleValidator *cgmLevelValidator;
    QTimer *powerHoldTimer;
    QTimer *batteryTimer;
    bool isPowerButtonHeld = false;
};
#endif // MAINWINDOW_H
