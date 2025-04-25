#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QWidget>

namespace Ui {
class PopupWidget;
}

class PopupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PopupWidget(QWidget *parent = nullptr);
    ~PopupWidget();

    void setMessage(const QString& message);
    void setHeader(const QString& header);
    void setOkButtonText(const QString& text);
    void setCancelButtonText(const QString& text);
    void setCancelButtonVisible(bool visible);

signals: // Add signals section
    void okClicked();
    void cancelClicked();

private slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();

private:
    Ui::PopupWidget *ui;
};

#endif // POPUPWIDGET.H
