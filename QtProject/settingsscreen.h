#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QWidget>
#include "loggerscreen.h"
namespace Ui {
class SettingsScreen;
}

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = nullptr);
    ~SettingsScreen();

private slots:
    void showMyPumpScreen();
    void showActivityScreen();
    void showMainOptions();
    void showProfileScreen();
    void showControlIqScreen();
    void showCGMScreen();
    void showHistoryScreen();
    void reloadAcitivySettings();
    void stopInsulinDelivery();

private:
    Ui::SettingsScreen *ui;
    LoggerScreen* loggerScreen;
};

#endif // SETTINGSSCREEN_H
