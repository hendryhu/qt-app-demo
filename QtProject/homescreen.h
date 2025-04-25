#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QTimer>
#include "popupmanager.h"

namespace Ui {
class HomeScreen;
}

class HomeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreen(QWidget *parent = nullptr);
    ~HomeScreen();

public slots:
    void updateBatteryDisplay();
    void updateInsulinDisplay();
    void updateInsulinOnBoardDisplay();
    void updateCgmDisplay();

private slots:
    void updateDateTimeDisplay();

private:
    Ui::HomeScreen *ui;
    QTimer *dateTimeTimer;
    bool highGlucoseAlertShown = false;
    bool lowGlucoseAlertShown = false;
};

#endif // HOMESCREEN_H
