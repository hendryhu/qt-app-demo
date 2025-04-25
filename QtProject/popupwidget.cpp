#include "popupwidget.h"
#include "ui_popupwidget.h"

PopupWidget::PopupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PopupWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, &QPushButton::clicked, this, &PopupWidget::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &PopupWidget::onCancelButtonClicked);
    ui->cancelButton->hide();
}

PopupWidget::~PopupWidget()
{
    delete ui;
}

void PopupWidget::setMessage(const QString& message)
{
    ui->messageLabel->setText(message);
}

void PopupWidget::setHeader(const QString& header)
{
    ui->headerLabel->setText(header);
}

void PopupWidget::setOkButtonText(const QString& text)
{
    ui->okButton->setText(text);
}

void PopupWidget::setCancelButtonText(const QString& text)
{
    ui->cancelButton->setText(text);
}

void PopupWidget::setCancelButtonVisible(bool visible)
{
    if (visible) {
        ui->cancelButton->show();
    } else {
        ui->cancelButton->hide();
    }
}

void PopupWidget::onOkButtonClicked()
{
    emit okClicked();
}

void PopupWidget::onCancelButtonClicked()
{
    emit cancelClicked();
}
