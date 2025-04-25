#ifndef POPUPMANAGER_H
#define POPUPMANAGER_H

#include <QObject>
#include <QString>
#include <functional>
#include <QGridLayout>
#include "data.h"

class PopupWidget;

class PopupManager : public QObject
{
    Q_OBJECT
public:
    static PopupManager& getInstance();
    static void registerOverlayParent(QWidget* parent);

public slots:
    static void showPopup(const QString& header, const QString& message, 
        bool showCancelButton = false, 
        const QString& okButtonText = "OK", const QString& cancelButtonText = "Cancel", 
        std::function<void()> okCallback = nullptr, std::function<void()> cancelCallback = nullptr);
    static void hidePopup();

private:
    PopupManager();
    ~PopupManager() override;
    static PopupManager* instance;
    PopupWidget* popupWidgetInstance; 
    static QWidget* s_popupOverlayParent;
};

#endif // POPUPMANAGER_H