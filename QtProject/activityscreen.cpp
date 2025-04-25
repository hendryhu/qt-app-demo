#include "activityscreen.h"
#include "ui_activityscreen.h"
#include <QMessageBox>
#include "data.h"

ActivityScreen::ActivityScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActivityScreen)
{
    ui->setupUi(this);

    ui->tempratevalue->setEnabled(true);
    loadSettings();

    connect(ui->saveButton, &QPushButton::clicked, this, &ActivityScreen::onSaveButtonClicked);
    connect(ui->sleepon, &QPushButton::toggled, this, &ActivityScreen::onSleepModeToggled);
    connect(ui->exerciseon, &QPushButton::toggled, this, &ActivityScreen::onExerciseModeToggled);
    connect(ui->addScheduleBtn, &QPushButton::clicked, this, &ActivityScreen::onAddScheduleClicked);
    connect(ui->removeScheduleBtn, &QPushButton::clicked, this, &ActivityScreen::onRemoveScheduleClicked);
}

ActivityScreen::~ActivityScreen()
{
    delete ui;
}

void ActivityScreen::onSaveButtonClicked()
{
    saveSettings();
    QMessageBox::information(this, "Success", "Activity profile settings have been saved successfully.");
}

void ActivityScreen::onSleepModeToggled(bool checked)
{

    if (checked && ui->exerciseon->isChecked()) {
        // If trying to enable sleep mode while exercise is on, show warning and prevent
        QMessageBox::warning(this, "Mode Conflict", 
            "Exercise mode is currently active. Please disable it before enabling Sleep mode.");
        ui->sleepon->setChecked(false);
        return;
    }

    enableScheduleControls(1, checked);
    if (ui->schedule2Frame->isVisible()) {
        enableScheduleControls(2, checked);
    }
    ui->addScheduleBtn->setEnabled(checked);
}

void ActivityScreen::onExerciseModeToggled(bool checked)
{
    if (checked && ui->sleepon->isChecked()) {
        // If trying to enable exercise mode while sleep is on, show warning and prevent
        QMessageBox::warning(this, "Mode Conflict", 
            "Sleep mode is currently active. Please disable it before enabling Exercise mode.");
        ui->exerciseon->setChecked(false);
        return;
    }
   
    ui->exerciseduration->setEnabled(checked);

}

void ActivityScreen::onAddScheduleClicked()
{
    ui->schedule2Frame->setVisible(true);
    enableScheduleControls(2, true);
    ui->addScheduleBtn->setVisible(false);
}

void ActivityScreen::onRemoveScheduleClicked()
{
    ui->schedule2Frame->setVisible(false);
    enableScheduleControls(2, false);
    ui->addScheduleBtn->setVisible(true);
}

QList<bool> ActivityScreen::getDaysForSchedule(int scheduleNum) const
{
    QList<bool> days;
    if (scheduleNum == 1) {
        days.append(ui->mon1->isChecked());
        days.append(ui->tue1->isChecked());
        days.append(ui->wed1->isChecked());
        days.append(ui->thu1->isChecked());
        days.append(ui->fri1->isChecked());
        days.append(ui->sat1->isChecked());
        days.append(ui->sun1->isChecked());
    } else if (scheduleNum == 2) {
        days.append(ui->mon2->isChecked());
        days.append(ui->tue2->isChecked());
        days.append(ui->wed2->isChecked());
        days.append(ui->thu2->isChecked());
        days.append(ui->fri2->isChecked());
        days.append(ui->sat2->isChecked());
        days.append(ui->sun2->isChecked());
    }
    return days;
}

void ActivityScreen::setDaysForSchedule(int scheduleNum, const QList<bool>& days)
{
    if (days.size() != 7) return;

    if (scheduleNum == 1) {
        ui->mon1->setChecked(days[0]);
        ui->tue1->setChecked(days[1]);
        ui->wed1->setChecked(days[2]);
        ui->thu1->setChecked(days[3]);
        ui->fri1->setChecked(days[4]);
        ui->sat1->setChecked(days[5]);
        ui->sun1->setChecked(days[6]);
    } else if (scheduleNum == 2) {
        ui->mon2->setChecked(days[0]);
        ui->tue2->setChecked(days[1]);
        ui->wed2->setChecked(days[2]);
        ui->thu2->setChecked(days[3]);
        ui->fri2->setChecked(days[4]);
        ui->sat2->setChecked(days[5]);
        ui->sun2->setChecked(days[6]);
    }
}

void ActivityScreen::enableScheduleControls(int scheduleNum, bool enabled)
{
    if (scheduleNum == 1) {
        ui->mon1->setEnabled(enabled);
        ui->tue1->setEnabled(enabled);
        ui->wed1->setEnabled(enabled);
        ui->thu1->setEnabled(enabled);
        ui->fri1->setEnabled(enabled);
        ui->sat1->setEnabled(enabled);
        ui->sun1->setEnabled(enabled);
        ui->sleep1start->setEnabled(enabled);
        ui->sleep1end->setEnabled(enabled);
    } else if (scheduleNum == 2) {
        ui->mon2->setEnabled(enabled);
        ui->tue2->setEnabled(enabled);
        ui->wed2->setEnabled(enabled);
        ui->thu2->setEnabled(enabled);
        ui->fri2->setEnabled(enabled);
        ui->sat2->setEnabled(enabled);
        ui->sun2->setEnabled(enabled);
        ui->sleep2start->setEnabled(enabled);
        ui->sleep2end->setEnabled(enabled);
    }
}

void ActivityScreen::saveSettings()
{
    if (ui->sleepon->isChecked()) {

        if (ui->sleep1start->time() == ui->sleep1end->time()) {
            QMessageBox::warning(this, "Invalid Input", "Sleep schedule 1 start and end times cannot be the same.");
            return;
        }

        QList<bool> schedule1Days = getDaysForSchedule(1);
        if (!schedule1Days.contains(true)) {
            QMessageBox::warning(this, "Invalid Input", "Please select at least one day for Sleep Schedule 1.");
            return;
        }

        if (ui->schedule2Frame->isVisible()) {
            if (ui->sleep2start->time() == ui->sleep2end->time()) {
                QMessageBox::warning(this, "Invalid Input", "Sleep schedule 2 start and end times cannot be the same.");
                return;
            }

            QList<bool> schedule2Days = getDaysForSchedule(2);
            if (!schedule2Days.contains(true)) {
                QMessageBox::warning(this, "Invalid Input", "Please select at least one day for Sleep Schedule 2.");
                return;
            }
        }
    }

    ActivitySettings settings;
    settings.sleepEnabled = ui->sleepon->isChecked();
    if(settings.sleepEnabled){
        ActivitySettings::SleepSchedule schedule1;
        schedule1.startTime = ui->sleep1start->time();
        schedule1.endTime = ui->sleep1end->time();
        schedule1.days = getDaysForSchedule(1);
        settings.sleepSchedules.append(schedule1);
        if(ui->schedule2Frame->isVisible()) {
            ActivitySettings::SleepSchedule schedule2;
            schedule2.startTime = ui->sleep2start->time();
            schedule2.endTime = ui->sleep2end->time();
            schedule2.days = getDaysForSchedule(2);
            settings.sleepSchedules.append(schedule2);
        }
    }
    settings.exerciseEnabled = ui->exerciseon->isChecked();
    settings.exerciseDuration = ui->exerciseduration->value();
    settings.tempBasalRate = ui->tempratevalue->value();
    Data::getInstance().setActivitySettings(settings);
}

void ActivityScreen::setDefaultValues() {

    ui->sleep1start->setTime(QTime(22, 0));
    ui->sleep1end->setTime(QTime(6, 0));
    ui->sleep2start->setTime(QTime(14, 0));
    ui->sleep2end->setTime(QTime(16, 0));

    QList<bool> emptyDays = {false, false, false, false, false, false, false};
    setDaysForSchedule(1, emptyDays);
    setDaysForSchedule(2, emptyDays);

    ui->schedule2Frame->setVisible(false);
    ui->addScheduleBtn->setVisible(true);
}

void ActivityScreen::loadSettings(){
    ActivitySettings settings = Data::getInstance().getActivitySettings();

    setDefaultValues();

    ui->sleepon->setChecked(settings.sleepEnabled);
    ui->exerciseon->setChecked(settings.exerciseEnabled);
    ui->exerciseduration->setValue(settings.exerciseDuration);
    ui->tempratevalue->setValue(settings.tempBasalRate);

  if (settings.sleepEnabled && !settings.sleepSchedules.isEmpty()) {
        const ActivitySettings::SleepSchedule& schedule1 = settings.sleepSchedules[0];
        ui->sleep1start->setTime(schedule1.startTime);
        ui->sleep1end->setTime(schedule1.endTime);
        setDaysForSchedule(1, schedule1.days);

        if (settings.sleepSchedules.size() > 1) {
            const ActivitySettings::SleepSchedule& schedule2 = settings.sleepSchedules[1];
            ui->schedule2Frame->setVisible(true);
            ui->addScheduleBtn->setVisible(false);
            ui->sleep2start->setTime(schedule2.startTime);
            ui->sleep2end->setTime(schedule2.endTime);
            setDaysForSchedule(2, schedule2.days);
        }
    }

    onSleepModeToggled(settings.sleepEnabled);
    onExerciseModeToggled(settings.exerciseEnabled);
}
