#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
   , ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   // connect the data-driven screen changes
   connect(&Data::getInstance(), &Data::currentScreenChanged, this, &MainWindow::handleCurrentScreenChanged);

   PopupManager::registerOverlayParent(ui->popupOverlayWidget); // Register the overlay parent for popups
   ui->popupOverlayWidget->setVisible(false); // Hide the popup overlay by default

   // connecting the debug screen buttons
   connect(ui->debugOffScreenButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCurrentScreen(Data::OffScreenType);
   });
   connect(ui->debugHomeButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCurrentScreen(Data::HomeScreenType);
   });
   connect(ui->debugBolusButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCurrentScreen(Data::BolusScreenType);
   });
   connect(ui->debugSettingsButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCurrentScreen(Data::SettingsScreenType);
   });


   // connect the physical home button
   connect(ui->physicalHomeButton, &QPushButton::clicked, this, [this]() {
      if (Data::getInstance().getCurrentScreen() != Data::OffScreenType) {
         Data::getInstance().setCurrentScreen(Data::HomeScreenType);
      }
   });

   // connecting battery level stuff
   batteryLevelValidator = new QIntValidator(0, 100, this);
   ui->batteryLevelInput->setValidator(batteryLevelValidator);
   connect(ui->setBatteryLevelButton, &QPushButton::clicked, this, [this]() {
      QString batteryText = ui->batteryLevelInput->text();
      int batteryValue = batteryText.toInt();
      Data::getInstance().setBatteryLevel(batteryValue);
      qDebug() << "Battery level set to:" << Data::getInstance().getBatteryLevel();
      if (Data::getInstance().getBatteryLevel() <= 20) {
         checkBatteryLevelPopup();
      }
   });
   connect(ui->batteryLevelInput, &QLineEdit::returnPressed, this, [this]() {
      ui->setBatteryLevelButton->click();
   });

   // additional checks to ensure the battery level is between 0 and 100
   connect(ui->batteryLevelInput, &QLineEdit::textChanged, this, [this](const QString &text) {
      bool ok;
      int value = text.toInt(&ok);

      if (ok) {
         if (value >= 0 && value <= 100) {
         } else {
            ui->batteryLevelInput->setText(QString::number(Data::getInstance().getBatteryLevel()));
         }
      } else {
         if (!text.isEmpty()) {
            ui->batteryLevelInput->clear();
         }
      }
   });

   // battery timer
   batteryTimer = new QTimer(this);
   connect(batteryTimer, &QTimer::timeout, this, &MainWindow::onBatteryTimerTimeout);
   batteryTimer->start(3000);

   // connect the charging button
   connect(ui->usbChargerButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCharging(!Data::getInstance().isCharging());
      if (Data::getInstance().isCharging()) {
         ui->usbChargerButton->setText("Disconnect USB Charger");
         Data::getInstance().setCharging(true);
         Logger::info("USB Charger Connected, charging...");
      } else {
         ui->usbChargerButton->setText("Connect USB Charger");
         Data::getInstance().setCharging(false);
         checkBatteryLevelPopup();
         Logger::info("USB Charger Disconnected, discharging...");
      }
   });
   
   // connecting the stored insulin level stuff
   storedInsulinLevelValidator = new QDoubleValidator(0, 300.0, 2, this);
   ui->storedInsulinInput->setValidator(storedInsulinLevelValidator);
   connect(ui->setStoredInsulinButton, &QPushButton::clicked, this, [this]() {
      QString insulinText = ui->storedInsulinInput->text();
      float insulinValue = insulinText.toFloat();
      Data::getInstance().setStoredInsulinLevel(insulinValue);
      Logger::info("Stored insulin level set to: " + QString::number(Data::getInstance().getStoredInsulinLevel()));
   });
   connect(ui->storedInsulinInput, &QLineEdit::returnPressed, this, [this]() {
      ui->setStoredInsulinButton->click();
   });
   connect(ui->storedInsulinInput, &QLineEdit::textChanged, this, [this](const QString &text) {
      bool ok;
      int value = text.toInt(&ok);

      if (ok) {
         if (value >= 0 && value <= 300) {
         } else {
            ui->storedInsulinInput->setText(QString::number(Data::getInstance().getStoredInsulinLevel()));
         }
      } else {
         if (!text.isEmpty()) {
            ui->storedInsulinInput->clear();
         }
      }
   });

   // connecting the CGM stuff
   cgmLevelValidator = new QDoubleValidator(0, 20.0, 2, this);
   ui->cgmLevelInput->setValidator(cgmLevelValidator);
   connect(ui->setCgmLevelButton, &QPushButton::clicked, this, [this]() {
      QString cgmText = ui->cgmLevelInput->text();
      float cgmValue = cgmText.toFloat();
      Data::getInstance().setCgmValue(cgmValue);
      qDebug() << "CGM level set to:" << Data::getInstance().getCgmValue();
   });
   connect(ui->cgmLevelInput, &QLineEdit::returnPressed, this, [this]() {
      ui->setCgmLevelButton->click();
   });
   connect(ui->cgmLevelInput, &QLineEdit::textChanged, this, [this](const QString &text) {
      bool ok;
      int value = text.toFloat(&ok);

      if (ok) {
         if (value >= 0.0 && value <= 20.0) {
         } else {
            ui->cgmLevelInput->setText(QString::number(Data::getInstance().getCgmValue()));
         }
      } else {
         if (!text.isEmpty()) {
            ui->cgmLevelInput->clear();
         }
      }
   });
   connect(ui->cgmConnectButton, &QPushButton::clicked, this, [this]() {
      Data::getInstance().setCgmConnected(!Data::getInstance().isCgmConnected());
      if (Data::getInstance().isCgmConnected()) {
         ui->cgmConnectButton->setText("Disconnect CGM");
         Data::getInstance().setCgmConnected(true);
         Logger::info("CGM Connected");
      } else {
         ui->cgmConnectButton->setText("Connect CGM");
         Data::getInstance().setCgmConnected(false);
         Logger::error("CGM Disconnected");

         // popup if CGM is disconnected
         PopupManager::showPopup("CGM Disconnected!", "Please reconnect the CGM device.");
      }
   });

   // connecting the power button and hold timer
   powerHoldTimer = new QTimer(this);
   powerHoldTimer->setSingleShot(true);
   connect(ui->powerButton, &QPushButton::pressed, this, &MainWindow::onPowerButtonPressed);
   connect(ui->powerButton, &QPushButton::released, this, &MainWindow::onPowerButtonReleased);
   connect(powerHoldTimer, &QTimer::timeout, this, &MainWindow::onPowerButtonHeldTimeout);
}

void MainWindow::handleCurrentScreenChanged(Data::ScreenType newScreen)
{
   qDebug() << "Current screen changed to:" << newScreen;

   // hide popup first when changing screens
   PopupManager::hidePopup();

   switch (newScreen)
   {
   case Data::OffScreenType:
      ui->screenStack->setCurrentWidget(ui->offScreen);
      break;
   case Data::HomeScreenType:
      ui->screenStack->setCurrentWidget(ui->homeScreen);
      break;
   case Data::BolusScreenType: 
   {
      ui->bolusScreen->setBloodGlucoseLabel(Data::getInstance().getCgmValue());
      ui->screenStack->setCurrentWidget(ui->bolusScreen);
      auto bolusWidget = ui->bolusScreen->findChild<QStackedWidget*>("stackedWidget");
      if (bolusWidget)
         bolusWidget->setCurrentIndex(0);  // Or use setCurrentWidget(ui->page_2) if accessible
      break;

   }
   case Data::SettingsScreenType:
      ui->screenStack->setCurrentWidget(ui->settingsScreen);
      break;
   default:
      qDebug() << "Warning: Unknown ScreenType in handleCurrentScreenChanged:" << newScreen;
      break;
   }

   // if we didn't change to the offscreen, throw a battery popup
   if (newScreen != Data::OffScreenType) {
      checkBatteryLevelPopup();
   }
}

void MainWindow::onPowerButtonPressed()
{
   qDebug() << "Power button pressed";
   isPowerButtonHeld = true;
   powerHoldTimer->start(1000);
}

void MainWindow::onPowerButtonReleased()
{
   qDebug() << "Power button released";
   if (powerHoldTimer->isActive()) {
      powerHoldTimer->stop();
      qDebug() << "Short press detected";

      // if not powered on yet (offScreenStack at index 0, do nothing)
      auto offScreenStack = ui->offScreen->findChild<QStackedWidget*>("offScreenStack");
      if (offScreenStack && offScreenStack->currentIndex() == 0 && Data::getInstance().getCurrentScreen() == Data::OffScreenType) {
         qDebug() << "Power button released while off screen, ignoring short press";
         return;
      }

      Data& data = Data::getInstance();
      if (data.getCurrentScreen() == Data::OffScreenType && offScreenStack->currentIndex() != 3) {
         qDebug() << "Waking up (short press)";
         Logger::info("Waking up from standby.");
         if (offScreenStack) {
            offScreenStack->setCurrentIndex(3);  // index 3 is the PIN screen
         }
      } else {
         qDebug() << "Going to Standby (short press)";
         Logger::info("Going to standby.");
         data.setCurrentScreen(Data::OffScreenType);
         if (offScreenStack) {
            offScreenStack->setCurrentIndex(2);  // index 0 is the standby screen
         }
      }
   }
   isPowerButtonHeld = false;
}

void MainWindow::onPowerButtonHeldTimeout()
{
   qDebug() << "Power button held timeout (long press)";

   // if the battery level is 0, do nothing
   if (Data::getInstance().getBatteryLevel() == 0) {
      Logger::error("No battery power, cannot power on.");
      qDebug() << "Battery is drained! The device cannot be powered on.";
      return;
   }

   if (isPowerButtonHeld) {
      Data& data = Data::getInstance();
      if (data.getCurrentScreen() == Data::OffScreenType) {
         qDebug() << "Powering On (long press)";
         Logger::info("Powering on the device.");
         auto offScreenStack = ui->offScreen->findChild<QStackedWidget*>("offScreenStack");
         if (offScreenStack) {
            offScreenStack->setCurrentIndex(1); // index 1 is the boot screen
         }
      }
   }
}

void MainWindow::onBatteryTimerTimeout()
{
   int currentBatteryLevel = Data::getInstance().getBatteryLevel();
   if(Data::getInstance().isCharging()) {
      currentBatteryLevel += 1;
      if (currentBatteryLevel > 100) {
         currentBatteryLevel = 100;
      }
      Data::getInstance().setBatteryLevel(currentBatteryLevel);
   } else {
      // don't do any discharging if we are in the offscreen
      if (Data::getInstance().getCurrentScreen() == Data::OffScreenType) {
         return;
      }
      currentBatteryLevel -= 1;
      if (currentBatteryLevel < 0) {
         currentBatteryLevel = 0;
      }
      Data::getInstance().setBatteryLevel(currentBatteryLevel);
      if (currentBatteryLevel == 20) {
         checkBatteryLevelPopup();
      }
   }

   // if the battery level is 0, set the current screen to OffScreenType index 0
   if (currentBatteryLevel == 0) {
      Data::getInstance().setCurrentScreen(Data::OffScreenType);
      Logger::warning("Battery is drained. Turning off the device.");
      qDebug() << "Battery is drained! Turning off...";
      auto offScreenStack = ui->offScreen->findChild<QStackedWidget*>("offScreenStack");
      if (offScreenStack) {
         offScreenStack->setCurrentIndex(0);  // index 0 is the off screen
      }
   }
}

void MainWindow::checkBatteryLevelPopup()
{
   qDebug() << "Checking battery level for popup...";

   // don't do anything if we're charging
   if (Data::getInstance().isCharging()) {
      return;
   }

   if (Data::getInstance().getBatteryLevel() <= 20) {
      Logger::warning("Battery level has dropped below 20%.");
      PopupManager::showPopup("Low Battery!", "Battery level is low. Please charge your device.");
   }
}

MainWindow::~MainWindow()
{
   delete batteryLevelValidator;
   delete storedInsulinLevelValidator;
   delete powerHoldTimer;
   delete batteryTimer;
   delete cgmLevelValidator;
   delete ui;
}
