#include "data.h"

Data* Data::instance = nullptr;

Data::Data()
{
    batteryLevel = 100;
    storedInsulinLevel = 300.0;
    insulinOnBoard = 5;
    currentScreen = OffScreenType;
    lastActiveScreen = HomeScreenType;
    pinCode = "1234";

    Profile exercise;
    exercise.name = "Exercise Mode";
    exercise.carbRatio = 1;
    exercise.correctionFactor = 1;
    exercise.targetGlucose = 5;
    exercise.basalRate = 0.8;  // Changed from append to direct assignment
    profiles[exercise.name] = exercise;

    Profile sleep;
    sleep.name = "Sleep ProfileS";
    sleep.carbRatio = 8;
    sleep.correctionFactor = 40;
    sleep.targetGlucose = 110;
    exercise.basalRate = 0.5;  // Changed from append to direct assignment
    profiles[sleep.name] = sleep;

    setCurrentProfile("Exercise Mode");

    activitySettings.sleepEnabled = false;
    activitySettings.exerciseEnabled = false;
    activitySettings.exerciseDuration = 60;
    activitySettings.tempBasalRate = 100;

    CGMValue = 7.5;
    simulatedDateTime = QDateTime::currentDateTime();

    cgmReadings.reserve(MAX_CGM_READINGS);
    for (int i = 0; i < 20; ++i) {
        float value = 5.3 + (7.5 - 5.3) * i / 19.0;
        cgmReadings.append({value, true});
    }
    CGMValue = 5.5;
    lastCGMValue = 5.5;

    //Set Basal Rate
    basalRate = getProfile(getCurrentProfile()).basalRate;

    sleepScheduleTimer = new QTimer(this);
    connect(sleepScheduleTimer, &QTimer::timeout, this, &Data::checkSleepSchedules);
    sleepScheduleTimer->start(60000); 
}

void Data::setBatteryLevel(int level)
{
    if (level >= 0 && level <= 100) {
        batteryLevel = level;
        emit batteryLevelChanged(batteryLevel);
    }
}

void Data::setCharging(bool newCharging)
{
    if (charging != newCharging) {
        charging = newCharging;
        emit usbChargerConnected(charging);
    }
}

void Data::setStoredInsulinLevel(float level)
{
    if (level >= 0 && level <= 300) {
        storedInsulinLevel = level;
        emit storedInsulinLevelChanged(storedInsulinLevel);
    }
}

void Data::setInsulinOnBoard(float units)
{
    if (units >= 0 && units <= 300) {
        insulinOnBoard = units;
        emit insulinOnBoardChanged(insulinOnBoard);
    }
}

void Data::setPinCode(const QString &pin)
{
    if (pin.length() == 4) {
        pinCode = pin;
    }
}

void Data::setCgmValue(float value)
{
    if (value > 0) {
        float previousValue = CGMValue;
        QDateTime currentTime = QDateTime::currentDateTime();

        CGMValue = value;
        lastCGMValue = previousValue;
        lastCGMTime = currentTime;


    }
    emit cgmValueChanged(CGMValue);
}

void Data::predictGlucose() {
    // Need at least 2 readings for prediction
    if (cgmReadings.size() < 2) return;

    // Get the most recent readings to calculate trend
    int numReadingsToUse = qMin(6, cgmReadings.size()); // Use up to 6 recent readings
    QVector<float> recentValues;

    // Extract the recent values
    for (int i = cgmReadings.size() - numReadingsToUse; i < cgmReadings.size(); i++) {
        recentValues.append(cgmReadings[i].value);
    }

    // Calculate rate of change using linear regression
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    float n = recentValues.size();

    for (int i = 0; i < recentValues.size(); i++) {
        float x = i; // Time index
        float y = recentValues[i]; // Glucose value

        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    // Calculate slope (rate of change per reading)
    float slope = 0;
    if ((n * sumX2 - sumX * sumX) != 0) {
        slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    }

    // Convert to rate of change per minute (assuming readings are ~1 minute apart)
    float rateOfChange = slope;

    // Calculate predicted BG for 30 minutes ahead
    float currentBG = cgmReadings.last().value;
    float predictedBG = currentBG + (rateOfChange * predictionWindowMinutes);

    qDebug() << "Current BG:" << currentBG << "Rate of change:" << rateOfChange
             << "Predicted BG in" << predictionWindowMinutes << "minutes:" << predictedBG;

    handlePredictedGlucose(predictedBG);
    emit predictedBGChanged(predictedBG);
}

void Data::handlePredictedGlucose(float predictedBG) {
    QString s = getCurrentProfile();
    Profile p = getProfile(s);
    basalRate = p.basalRate;
    float tempBasalRate;
    static float previousTempBasalRate = -1.0f;

    if (!controlIqEnabled) {
        if (previousTempBasalRate != basalRate) {
            Logger::info("Control IQ not enabled, no automated adjustment made");
            previousTempBasalRate = basalRate;
        }
        return;
    }

    if (stopInsulin) {
        tempBasalRate = 0;
        return;
    }
     // Handle Sleep Mode
     if (activitySettings.sleepEnabled) {
        if (!isWithinSleepSchedule()) {
            activitySettings.sleepEnabled = false;
            emit activitySettingsChanged();
            Logger::info("Sleep mode disabled - outside of scheduled hours");
            return;
        }

        if (predictedBG <= 3.9f) {
            tempBasalRate = basalRate * 0;  // Suspend insulin
            Logger::info(QString("Sleep Mode: Suspended insulin delivery (BG: %1 mmol/L)").arg(predictedBG));
        } else if (predictedBG <= 6.25f) {
            tempBasalRate = basalRate * 0.75;  // Decrease insulin
            Logger::info(QString("Sleep Mode: Decreased insulin by 25% (BG: %1 mmol/L)").arg(predictedBG));
        } else if (predictedBG >= 6.7f) {
            tempBasalRate = basalRate * 1.333;  // Increase insulin
            Logger::info(QString("Sleep Mode: Increased insulin by 33% (BG: %1 mmol/L)").arg(predictedBG));
        } else {
            tempBasalRate = basalRate;  // Normal delivery
            Logger::info(QString("Sleep Mode: Increased insulin by 33% (BG: %1 mmol/L)").arg(predictedBG));
        }
        // No automatic correction boluses in sleep mode
        deliverBolus(tempBasalRate);
    }
    // Handle Exercise Mode
    else if (activitySettings.exerciseEnabled) {
        if (predictedBG <= 4.4f) {
            tempBasalRate = basalRate * 0;  // Suspend insulin
            Logger::info(QString("Exercise Mode: Suspended insulin delivery (BG: %1 mmol/L)").arg(predictedBG));
        } else if (predictedBG <= 7.8f) {
            tempBasalRate = basalRate * 0.75;  // Decrease insulin
            if (previousTempBasalRate != tempBasalRate) {
                QString logMessage = QString("Exercise Mode: Predicted BG low (%1 mmol/L) - decreasing basal rate by 25% (from %2 to %3 U/hr)")
                    .arg(predictedBG, 0, 'f', 1)
                    .arg(basalRate, 0, 'f', 2)
                    .arg(tempBasalRate, 0, 'f', 2);
                Logger::info(logMessage);
            }
        } else if (predictedBG >= 8.9f) {
            tempBasalRate = basalRate * 1.333;  // Increase insulin
            Logger::info(QString("Exercise Mode: Increased insulin by 33% (BG: %1 mmol/L)").arg(predictedBG));
        } else {
            tempBasalRate = basalRate;  // Normal delivery
            Logger::info(QString("Exercise Mode: Normal insulin delivery (BG: %1 mmol/L)").arg(predictedBG));
        }
        deliverBolus(tempBasalRate);
    }
    // Normal Control-IQ mode (existing code)
    else {
        if (predictedBG < 4.0f) {
            tempBasalRate = basalRate * 0;
        } else if (predictedBG >= 4.0f && predictedBG <= 6.25f) {
            tempBasalRate = basalRate * 0.75;
            if (previousTempBasalRate != tempBasalRate) {
                QString logMessage = QString("Predicted BG low (%1 mmol/L) - decreasing basal rate by 25% (from %2 to %3 U/hr)")
                    .arg(predictedBG, 0, 'f', 1)
                    .arg(basalRate, 0, 'f', 2)
                    .arg(tempBasalRate, 0, 'f', 2);
                Logger::info(logMessage);
            }
        } else if (predictedBG > 6.25f && predictedBG <= 8.9f) {
            tempBasalRate = basalRate;
            if (previousTempBasalRate != tempBasalRate) {
                QString logMessage = QString("Predicted BG in target range (%1 mmol/L) - maintaining standard basal rate at %2 U/hr")
                    .arg(predictedBG, 0, 'f', 1)
                    .arg(basalRate, 0, 'f', 2);
                Logger::info(logMessage);
            }
        } else if (predictedBG > 8.9f && predictedBG < 10.0f) {
            tempBasalRate = basalRate * 1.333;
            if (previousTempBasalRate != tempBasalRate) {
                QString logMessage = QString("Predicted BG elevated (%1 mmol/L) - increasing basal rate by 33% (from %2 to %3 U/hr)")
                    .arg(predictedBG, 0, 'f', 1)
                    .arg(basalRate, 0, 'f', 2)
                    .arg(tempBasalRate, 0, 'f', 2);
                Logger::info(logMessage);
            }
        } else {
            tempBasalRate = basalRate * 2;
            if (previousTempBasalRate != tempBasalRate) {
                QString logMessage = QString("Predicted BG high (%1 mmol/L) - doubling basal rate (from %2 to %3 U/hr)")
                    .arg(predictedBG, 0, 'f', 1)
                    .arg(basalRate, 0, 'f', 2)
                    .arg(tempBasalRate, 0, 'f', 2);
                Logger::info(logMessage);
            }
        }
        deliverBolus(tempBasalRate);
    }
    setCgmValue(getCgmValue() + (basalRate-tempBasalRate)/(basalRate*5));
}


void Data::setCgmConnected(bool connected)
{
    CGMConnected = connected;
    emit cgmConnectionChanged(CGMConnected);
}

//This is for bolus screen to remove
void Data::deliverBolus(float units)
{
    if (!stopInsulin && units > 0 && storedInsulinLevel >= units) {
        setStoredInsulinLevel(qMax((float)0, storedInsulinLevel - units));
    }
    //TODO: do some error handling here
}

void Data::setCurrentScreen(ScreenType screen)
{
    if (currentScreen == screen) {
        return;
    }
    if (currentScreen != OffScreenType) {
        lastActiveScreen = currentScreen;
    }
    currentScreen = screen;
    emit currentScreenChanged(currentScreen);
}

Data& Data::getInstance()
{
    if (!instance) {
        instance = new Data();
    }
    return *instance;
}

void Data::addProfile(const Profile &profile){
    profiles[profile.name] = profile;
    emit profilesChanged();
}

void Data::updateProfile(const Profile &profile){
    if(profiles.contains(profile.name)){
        profiles[profile.name] = profile;

        emit profilesChanged();
    }
}

void Data::deleteProfile(const QString profileName){
    if(profiles.contains(profileName)){
        profiles.remove(profileName);
        emit profilesChanged();
    }
}

QStringList Data::getProfileNames() const{
    return profiles.keys();
}

Profile Data::getProfile(const QString profileName) const{
    return profiles[profileName];
}

bool Data::setCurrentProfile(const QString profileName){
    if(profiles.contains(profileName)){
        current_profile_name = profileName;
        return true;
    }
    return false;
}

QString Data::getCurrentProfile(){
    return current_profile_name;
}

void Data::clearCgmConfigs(){
    cgm_configs.clear();
}

ActivitySettings Data::getActivitySettings(){
    return activitySettings;
}

void Data::setActivitySettings(const ActivitySettings& settings){
    activitySettings = settings;
}

void Data::setControlIqSettings(const ControlIqSettings& settings){
    controlIqSettings = settings;
}

ControlIqSettings Data::getControlIqSettings(){
    return controlIqSettings;
}



void Data::advanceSimulatedTime(int minutes)
{
    simulatedDateTime = simulatedDateTime.addSecs(minutes * 60);
    emit simulatedTimeChanged(simulatedDateTime);
}

void Data::setGraphHours(int hours)
{
    graphHours = hours;
    emit graphHoursChanged(graphHours);
}

void Data::addValueToCgmReadings()
{
    // if the CGM isn't connected, then just add the last value again
    if (!isCgmConnected()) {
        float lastValue = cgmReadings.isEmpty() ? 0 : cgmReadings.last().value;
        cgmReadings.append({lastValue, false});

    } else {
        cgmReadings.append({CGMValue, isCgmConnected()});
        if (cgmReadings.size() > MAX_CGM_READINGS) {
            cgmReadings.removeFirst();
        }
    }
}

float Data::getCgmValue() const {
    // if CGM is connected, return the CGM value. Otherwise, return the last value in the readings
    if (isCgmConnected()) {
        return CGMValue;
    } else if (!cgmReadings.isEmpty()) {
        return cgmReadings.last().value;
    } else {
        return 0.0;
    }
}


bool Data::isWithinSleepSchedule() const {
    if (!controlIqEnabled || activitySettings.sleepSchedules.isEmpty()) {
        return false;
    }

    int currentDay = simulatedDateTime.date().dayOfWeek() - 1; // 0-6 for Mon-Sun
    QTime currentTime = simulatedDateTime.time();

    for (const auto& schedule : activitySettings.sleepSchedules) {
        if (!schedule.days[currentDay]) {
            continue;
        }

        QTime startTime = schedule.startTime;
        QTime endTime = schedule.endTime;

        // Handle overnight schedules (e.g. 22:00 to 06:00)
        if (endTime < startTime) {
            if (currentTime >= startTime || currentTime <= endTime) {
                return true;
            }
        } else {
            if (currentTime >= startTime && currentTime <= endTime) {
                return true;
            }
        }
    }
    return false;
}

void Data::checkSleepSchedules() {
    if (!controlIqEnabled) return;

    bool shouldBeSleeping = isWithinSleepSchedule();
    
    if (shouldBeSleeping && !activitySettings.sleepEnabled) {
        // Enter sleep mode
        activitySettings.sleepEnabled = true;
        activitySettings.exerciseEnabled = false;  // Disable exercise mode
        emit activitySettingsChanged();
        Logger::info("Sleep mode automatically enabled based on schedule");
    } else if (!shouldBeSleeping && activitySettings.sleepEnabled) {
        // Exit sleep mode
        activitySettings.sleepEnabled = false;
        emit activitySettingsChanged();
        Logger::info("Sleep mode automatically disabled based on schedule");
    }
}

