#ifndef PROFILESCREEN_H
#define PROFILESCREEN_H

#include "data.h"
#include <QWidget>
#include <QListWidgetItem>
#include <QTableWidgetItem>

namespace Ui {
class Profilescreen;
}

class Profilescreen : public QWidget
{
    Q_OBJECT

public:
    explicit Profilescreen(QWidget *parent = nullptr);
    ~Profilescreen();

private slots:
    void on_profileAddBtn_clicked();
    void on_profileEditBtn_clicked();
    void on_profileDeleteBtn_clicked();
    void on_saveProfileBtn_clicked();
    void on_cancelBtn_clicked();
    // void on_addBasalRateBtn_clicked();
    void on_currentProfileBtn_clicked();

private:
    Ui::Profilescreen *ui;
    void refreshProfileList();
    void loadProfileDetails(const QString& profileName);
    Data& dataInstance;
};

#endif // PROFILESCREEN_H
