#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QTime>
#include <QMap>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "logger.h"

struct Profile{
    QString name;
    int carbRatio;
    int correctionFactor;
    int targetGlucose;
    double basalRate;
};

struct ActivitySettings{
    bool sleepEnabled;
    struct SleepSchedule{
        QList<bool> days;
        QTime startTime;
        QTime endTime;
    };
    QVector<SleepSchedule> sleepSchedules;
    bool exerciseEnabled;
    int exerciseDuration;
    int tempBasalRate;
};

struct ControlIqSettings{
    double weight;
    double dailyInsulin;
};

struct CgmReading{
    float value;
    bool wasConnected;
};

class Data : public QObject
{
    Q_OBJECT

public:
    enum ScreenType {
        OffScreenType,
        HomeScreenType,
        SettingsScreenType,
        BolusScreenType
    };
    const int MAX_CGM_READINGS = 72; // 5 minutes per read, 6 hours of data

private:
    Data();
    int batteryLevel;
    bool charging = false;

    bool timedBolusStatus = false;
    bool stopInsulin = false;
    bool controlIqEnabled = false;

    Profile currentProfile;

    float storedInsulinLevel;
    float insulinOnBoard;

    float CGMValue;
    bool CGMConnected = true;
    QVector<CgmReading> cgmReadings;

    float basalRate;
    float toDeliver;

    QDateTime simulatedDateTime;
    int graphHours = 1;

    static Data* instance;
    QMap<QString, Profile> profiles;
    QString current_profile_name;
    QMap<QString, QString> cgm_configs;
    ActivitySettings activitySettings;
    ControlIqSettings controlIqSettings;
    ScreenType currentScreen;
    ScreenType lastActiveScreen;
    QString pinCode = "1234";
    //float maxAllowedBG = 180.0f;
    float predictionWindowMinutes = 30.0f;
    float lastCGMValue = 0.0f;
    QDateTime lastCGMTime;
    //float maxAllowedBG = 180.0f;

    QTimer* sleepScheduleTimer;
    void checkSleepSchedules();
    bool isWithinSleepSchedule() const;

public:
    static Data& getInstance();

    //timed bolus status
    bool getTimedBolusStatus() {return timedBolusStatus;}
    void setTimedBolusStatus(bool val) {timedBolusStatus = val;}

    // stop insulin delivery
    bool getStopInsulin() const { return stopInsulin; }
    void setStopInsulin(bool stop) { stopInsulin = stop; }

    // enable/disable controlIQ
    bool getControlIqEnabled() const { return controlIqEnabled; }
    void setControlIqEnabled(bool enabled) { controlIqEnabled = enabled; }

    // battery level
    void setBatteryLevel(int level);
    int getBatteryLevel() const { return batteryLevel; }
    void setCharging(bool newCharging);
    bool isCharging() const { return charging; }

    // insulin level
    void setStoredInsulinLevel(float level);
    float getStoredInsulinLevel() const { return storedInsulinLevel; }
    float getInsulinOnBoard(){ return insulinOnBoard; }
    void setInsulinOnBoard(float units);
    void deliverBolus(float units);
    void deliverBolusOverTime(float units, int durationSeconds);

    // CGM value
    void setCgmValue(float value);
    float getCgmValue() const;
    void addValueToCgmReadings();
    const QVector<CgmReading>& getCgmReadings() const { return cgmReadings; }
    void setCgmConnected(bool connected);
    bool isCgmConnected() const { return CGMConnected; }

    //timed bolus delivery
    QTimer* bolusTimer = nullptr;
    float bolusAmountToDeliver = 0;
    float bolusDelivered = 0;
    int bolusSteps = 0;
    int bolusStep = 0;
    float bolusStepAmount = 0;

    // screen type
    void setCurrentScreen(ScreenType screen);
    ScreenType getCurrentScreen() const { return currentScreen; }
    ScreenType getLastActiveScreen() const { return lastActiveScreen; }

    // pin code
    void setPinCode(const QString &pin);
    QString getPinCode() const { return pinCode; }

    void addProfile(const Profile& profile);
    void updateProfile(const Profile &profile);
    void deleteProfile(const QString profileName);
    QStringList getProfileNames() const;
    Profile getProfile(const QString profileName) const;

    bool setCurrentProfile(const QString profileName);
    QString getCurrentProfile();
    void clearCgmConfigs();
    ActivitySettings getActivitySettings();
    void setActivitySettings(const ActivitySettings& settings);
    ControlIqSettings getControlIqSettings();
    void setControlIqSettings(const ControlIqSettings& settings);

    // ControlIQ
    void predictGlucose();
    float getPredictedBG() const;
    void handlePredictedGlucose(float predictedBG);

    // date and time
    QDateTime getSimulatedDateTime() const { return simulatedDateTime; }
    void advanceSimulatedTime(int minutes);

    // graph hours
    int getGraphHours() const { return graphHours; }
    void setGraphHours(int hours);



signals:
    void batteryLevelChanged(int newLevel);
    void usbChargerConnected(bool connected);
    void storedInsulinLevelChanged(int newLevel);
    void profilesChanged();
    void activitySettingsChanged();
    void controlIqSettingsChanged();
    void currentScreenChanged(ScreenType newScreen);
    void insulinOnBoardChanged(float newLevel);
    void cgmValueChanged(float newValue);
    void cgmConnectionChanged(bool connected);
    void predictedBGChanged(float predictedValue);
    void automaticCorrectionNeeded(float units);
    void simulatedTimeChanged(QDateTime newTime);
    void graphHoursChanged(int newHours);
};

#endif // DATA_H
