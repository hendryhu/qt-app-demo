#include "offscreen.h"
#include "ui_offscreen.h"
#include "data.h"

OffScreen::OffScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OffScreen)
{
    ui->setupUi(this);

    connect(ui->buttonOK, &QPushButton::clicked, this, &OffScreen::onPinOkButtonClicked);

    // set up the PIN edit field
    pinValidator = new QIntValidator(0, 9999, this);
    ui->PINEdit->setValidator(pinValidator);
    ui->PINEdit->setMaxLength(4);
    connect(ui->PINEdit, &QLineEdit::returnPressed, this, &OffScreen::onPinOkButtonClicked);

    const QList<QPushButton*> digitButtons = {
        ui->button0, ui->button1, ui->button2, ui->button3,
        ui->button4, ui->button5, ui->button6, ui->button7,
        ui->button8, ui->button9
    };

    // Ensure all buttons are properly initialized
    for (QPushButton* button : digitButtons) {
        Q_ASSERT(button != nullptr);
    }
    for (QPushButton* button : digitButtons) {
        connect(button, &QPushButton::clicked, this, &OffScreen::onPinDigitButtonClicked);
    }
    connect(ui->buttonDel, &QPushButton::clicked, this, &OffScreen::onPinDeleteButtonClicked);

    // run onBootScreenShown when the offScreenStack has its page set to 1
    connect(ui->offScreenStack, &QStackedWidget::currentChanged, this, [this](int index) {
        if (index == 1) { // Assuming 1 is the index for the boot screen
            onBootScreenShown();
        }
    });

    // connect boot timer to progressBar, so the progress bar fills up as the timer runs
    bootTimeLine = new QTimeLine(1000, this);
    bootTimeLine->setFrameRange(0, 100);
    bootTimeLine->setCurveShape(QTimeLine::EaseInOutCurve);
    connect(bootTimeLine, &QTimeLine::frameChanged,
        ui->progressBar, &QProgressBar::setValue);
    connect(bootTimeLine, &QTimeLine::finished, this, &OffScreen::onBootTimerTimeout);
}

OffScreen::~OffScreen()
{
    delete bootTimeLine;
    delete pinValidator;
    delete ui;
}

void OffScreen::onBootScreenShown()
{
    bootTimeLine->setCurrentTime(0);
    bootTimeLine->setDuration(1000); // 1 second duration
    bootTimeLine->start();
    ui->progressBar->setValue(0);
}

void OffScreen::onBootTimerTimeout()
{
    ui->offScreenStack->setCurrentIndex(3);
    bootTimeLine->stop();
    bootTimeLine->setCurrentTime(0);
    ui->PINEdit->clear();
}

void OffScreen::onPinDigitButtonClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (clickedButton && ui->PINEdit->text().length() < 4) {
        QString digit = clickedButton->text(); // Assumes button text is the digit
        ui->PINEdit->insert(digit);
    }
}

void OffScreen::onPinOkButtonClicked()
{
    QString pin = ui->PINEdit->text();
    if (pin == Data::getInstance().getPinCode()) {
        // Pin is correct, proceed to the last active screen
        Logger::info("User login successful.");
        Data::getInstance().setCurrentScreen(Data::getInstance().getLastActiveScreen());
        ui->PINEdit->clear();
    } else {
        Logger::warning("Incorrect PIN entered.");
        ui->PINLabel->setText("Incorrect PIN. Try again.");
        ui->PINEdit->clear();
    }
}

void OffScreen::onPinDeleteButtonClicked()
{
    ui->PINEdit->backspace();
}
