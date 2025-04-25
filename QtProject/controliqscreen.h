#ifndef CONTROLIQSCREEN_H
#define CONTROLIQSCREEN_H

#include <QWidget>
#include "data.h"
#include <QMessageBox>

namespace Ui {
class ControlIqScreen;
}

class ControlIqScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ControlIqScreen(QWidget *parent = nullptr);
    ~ControlIqScreen();

private slots:
    void toggleControlIq(bool checked);
    void saveControlIqSetting();

private:
    Ui::ControlIqScreen *ui;
    Data& dataInstance;
};

#endif // CONTROLIQSCREEN_H
