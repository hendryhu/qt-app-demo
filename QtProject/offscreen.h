#ifndef OFFSCREEN_H
#define OFFSCREEN_H

#include <QWidget>
#include <QTimer>
#include <QIntValidator>
#include <QTimeLine>    
#include "logger.h"

namespace Ui {
class OffScreen;
}

class OffScreen : public QWidget
{
    Q_OBJECT

public:
    explicit OffScreen(QWidget *parent = nullptr);
    ~OffScreen();

private slots:
    void onBootScreenShown();
    void onBootTimerTimeout();
    void onPinOkButtonClicked();
    void onPinDigitButtonClicked();
    void onPinDeleteButtonClicked();

private:
    Ui::OffScreen *ui;
    QIntValidator *pinValidator;
    QTimeLine *bootTimeLine;
};

#endif // OFFSCREEN_H
