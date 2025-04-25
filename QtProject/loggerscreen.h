#ifndef LoggerScreen_H
#define LoggerScreen_H

#include <QWidget>

namespace Ui {
class LoggerScreen;
}

class LoggerScreen : public QWidget
{
    Q_OBJECT

public:
    explicit LoggerScreen(QWidget *parent = nullptr);
    ~LoggerScreen();

public slots:
    void refreshLog();
    void clearLog();
    //void addTestLogEntry();
    void populateLogFiles();

private:
    Ui::LoggerScreen *ui;
};

#endif // LoggerScreen_H
