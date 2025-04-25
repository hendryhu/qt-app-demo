#ifndef ACTIVITYSCREEN_H
#define ACTIVITYSCREEN_H

#include <QWidget>
#include <QList>

namespace Ui {
class ActivityScreen;
}

class ActivityScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ActivityScreen(QWidget *parent = nullptr);
    ~ActivityScreen();
    void loadSettings();

private slots:
    void onSaveButtonClicked();
    void onSleepModeToggled(bool checked);
    void onExerciseModeToggled(bool checked);
    void onAddScheduleClicked();
    void onRemoveScheduleClicked();

private:
    Ui::ActivityScreen *ui;
    void setDefaultValues();
    void saveSettings();
    QList<bool> getDaysForSchedule(int scheduleNum) const;
    void setDaysForSchedule(int scheduleNum, const QList<bool>& days);
    void updateScheduleVisibility();
    void enableScheduleControls(int scheduleNum, bool enabled);
};

#endif // ACTIVITYSCREEN_H
