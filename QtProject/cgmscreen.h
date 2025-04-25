#ifndef CGMSCREEN_H
#define CGMSCREEN_H

#include <QWidget>
#include "data.h"
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QDebug>

namespace Ui {
class cgmscreen;
}

class cgmscreen : public QWidget
{
    Q_OBJECT

public:
    explicit cgmscreen(QWidget *parent = nullptr);
    ~cgmscreen();

private:
    Ui::cgmscreen *ui;
    Data& dataInstance;
};

#endif // CGMSCREEN_H
