#include "profilescreen.h"
#include "ui_profilescreen.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QTextStream>
#include <QDebug>
#include "logger.h"

Profilescreen::Profilescreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Profilescreen),
    dataInstance(Data::getInstance())
{
    ui->setupUi(this);

    ui->profileEditPanel->setVisible(false);

//    connect(ui->profileAddBtn, &QPushButton::clicked, this, &Profilescreen::on_profileAddBtn_clicked);
//    connect(ui->profileEditBtn, &QPushButton::clicked, this, &Profilescreen::on_profileEditBtn_clicked);
//    connect(ui->profileDeleteBtn, &QPushButton::clicked, this, &Profilescreen::on_profileDeleteBtn_clicked);
//    connect(ui->saveProfileBtn, &QPushButton::clicked, this, &Profilescreen::on_saveProfileBtn_clicked);
//    connect(ui->cancelBtn, &QPushButton::clicked, this, &Profilescreen::on_cancelBtn_clicked);
//    connect(ui->addBasalRateBtn, &QPushButton::clicked, this, &Profilescreen::on_addBasalRateBtn_clicked);

    connect(&dataInstance, &Data::profilesChanged, this, &Profilescreen::refreshProfileList);

    refreshProfileList();

}

Profilescreen::~Profilescreen()
{
    delete ui;
}

void Profilescreen::refreshProfileList(){
    ui->profileList->clear();

    QStringList profileNames = dataInstance.getProfileNames();
    ui->profileList->addItems(profileNames);
    QString currentProfile = dataInstance.getCurrentProfile();
    QList<QListWidgetItem*> items = ui->profileList->findItems(currentProfile, Qt::MatchExactly);
    if (!items.isEmpty()) {
        QListWidgetItem* item = items.first();
        // Now you can work with the found item
        ui->profileList->setCurrentItem(item);
        item->setText("✔ " + currentProfile);

        QFont boldFont;
        boldFont.setBold(true);
        item->setFont(boldFont);

        //loadProfileDetails(currentProfile);
        QString message = "Current profile set to:" + dataInstance.getCurrentProfile();
        Logger::info(message);
    }


    return;
}

void Profilescreen::loadProfileDetails(const QString& profileName){
    Profile profile = dataInstance.getProfile(profileName);

    ui->profileNameLineEdit->setText(profile.name);
    ui->carbRatioSpin->setValue(profile.carbRatio);
    ui->correctionFactorSpin->setValue(profile.correctionFactor);
    ui->targetGlusoceSpin->setValue(profile.targetGlucose);
    ui->basalRateSpin->setValue(profile.basalRate);
}

void Profilescreen::on_profileAddBtn_clicked()
{
    ui->profileEditHeaderLabel->setText("Create New Profile");
    ui->profileNameLineEdit->clear();
    ui->profileEditPanel->setVisible(true);
    // ui->basalRateTable->setRowCount(0);
    ui->carbRatioSpin->setValue(0);
    ui->correctionFactorSpin->setValue(0);
    ui->targetGlusoceSpin->setValue(0);
    ui->basalRateSpin->setValue(0);
    // on_addBasalRateBtn_clicked();
    ui->profileSelectionPanel->setEnabled(false);
}

void Profilescreen::on_profileEditBtn_clicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "Selection Required", "Please select a profile to edit");
        return;
    }

    ui->profileEditHeaderLabel->setText("Edit Profile");

    QString profileName = selectedItem->text();
    if (profileName.startsWith("✔ ")) {
            profileName = profileName.mid(2);
    }

    ui->profileEditPanel->setVisible(true);
    ui->profileSelectionPanel->setEnabled(false);

    loadProfileDetails(profileName);

}


void Profilescreen::on_profileDeleteBtn_clicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "Selection Required", "Please select a profile to delete");
        return;
    }

     QString profileName = selectedItem->text();
    if (profileName.startsWith("✔ ")) {
            profileName = profileName.mid(2);
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion",
                                 "Are you sure you want to delete the profile '" + profileName + "'?",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
          dataInstance.deleteProfile(profileName);
        QMessageBox::information(this, "Profile Deleted", "The profile has been deleted successfully");
    }
    refreshProfileList();
}

void Profilescreen::on_saveProfileBtn_clicked()
{
    // Validate profile name
    QString profileName = ui->profileNameLineEdit->text().trimmed();
    if (profileName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a profile name");
        return;
    }

    // Validate correction factor
    double correctionFactor = ui->correctionFactorSpin->value();
    if (correctionFactor <= 0) {
        QMessageBox::warning(this, "Validation Error", "Correction factor must be greater than 0");
        return;
    }

    // Validate carb ratio
    double carbRatio = ui->carbRatioSpin->value();
    if (carbRatio <= 0) {
        QMessageBox::warning(this, "Validation Error", "Carb ratio must be greater than 0");
        return;
    }

    // Validate target glucose
    double targetGlucose = ui->targetGlusoceSpin->value();
    if (targetGlucose < 70 || targetGlucose > 180) {
        QMessageBox::warning(this, "Validation Error", "Target glucose must be between 70 and 180 mg/dL");
        return;
    }

    // Validate basal rate
    double basalRate = ui->basalRateSpin->value();
    if (basalRate < 0) {
        QMessageBox::warning(this, "Validation Error", "Basal rate cannot be negative");
        return;
    }

    Profile profile;
    profile.name = profileName;
    profile.correctionFactor = correctionFactor;
    profile.carbRatio = carbRatio;
    profile.targetGlucose = targetGlucose;
    profile.basalRate = basalRate;

    if (ui->profileEditHeaderLabel->text() == "Create New Profile") {
        dataInstance.addProfile(profile);
        QMessageBox::information(this, "Profile Created", "New profile created successfully");
    } else {
        dataInstance.updateProfile(profile);
        QMessageBox::information(this, "Profile Updated", "Profile updated successfully");
    }

    ui->profileEditPanel->setVisible(false);
    refreshProfileList();
    ui->profileSelectionPanel->setEnabled(true);
}

void Profilescreen::on_cancelBtn_clicked()
{
    ui->profileEditPanel->setVisible(false);
    ui->profileSelectionPanel->setEnabled(true);

}


void Profilescreen::on_currentProfileBtn_clicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "Selection Required", "Please select a profile to edit");
        return;
    }

    // Clear previous indicators from all items
    for (int i = 0; i < ui->profileList->count(); ++i) {
        QListWidgetItem *item = ui->profileList->item(i);
        QString text = item->text();
        // Remove prefix if it exists
        if (text.startsWith("✔ ")) {
            item->setText(text.mid(2));
        }
        item->setFont(QFont());  // reset font
    }

    // Mark the selected item visually
    QString profileName = selectedItem->text();
    if(dataInstance.setCurrentProfile(profileName)){
        selectedItem->setText("✔ " + profileName);

        QFont boldFont;
        boldFont.setBold(true);
        selectedItem->setFont(boldFont);

        //loadProfileDetails(profileName);
        QString message = "Current profile set to:" + dataInstance.getCurrentProfile();
        Logger::info(message);

        return;
    };
    QString message = "Profile not found" + profileName;
    Logger::error(message);

}
