#ifndef MYPUMPSCREEN_H
#define MYPUMPSCREEN_H

#include <QWidget>

namespace Ui {
class MyPumpScreen;
}

class MyPumpScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MyPumpScreen(QWidget *parent = nullptr);
    ~MyPumpScreen();

signals:
    void showProfileScreen();
    void showControlIqScreen();

private slots:
    void onProfilesButtonClicked();
    void onControlIqButtonClicked();

private:
    Ui::MyPumpScreen *ui;
};

#endif // MYPUMPSCREEN_H
