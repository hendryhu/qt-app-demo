#include "popupmanager.h"
#include "popupwidget.h"
#include <QDebug>

PopupManager* PopupManager::instance = nullptr;
QWidget* PopupManager::s_popupOverlayParent = nullptr;

PopupManager::PopupManager() : popupWidgetInstance(nullptr)
{
}

PopupManager::~PopupManager()
{
    delete popupWidgetInstance;
}

PopupManager& PopupManager::getInstance()
{
    if (!instance) {
        instance = new PopupManager();
    }
    return *instance;
}

void PopupManager::registerOverlayParent(QWidget* parent)
{
    s_popupOverlayParent = parent;
    qDebug() << "PopupManager: Registered overlay parent:" << parent;
}

void PopupManager::showPopup(const QString& header, const QString& message, 
    bool showCancelButton, 
    const QString& okButtonText, const QString& cancelButtonText,  
    std::function<void()> okCallback, std::function<void()> cancelCallback)
{
    // if parent is unregistered, do nothing
    if (!s_popupOverlayParent) {
        qDebug() << "PopupManager: No overlay parent registered. Cannot show popup.";
        return;
    }

    // don't do anything when the screen is off
    if (Data::getInstance().getCurrentScreen() == Data::OffScreenType) {
        return;
    }

    qDebug() << "PopupManager::showPopup called with header:" << header << " and message:" << message;
    qDebug() << "okButtonText:" << okButtonText << "cancelButtonText:" << cancelButtonText << "showCancelButton:" << showCancelButton;

    PopupManager& manager = getInstance();

    if (!manager.popupWidgetInstance) {
        QGridLayout* layout = new QGridLayout(s_popupOverlayParent);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        manager.popupWidgetInstance = new PopupWidget(s_popupOverlayParent);
        layout->addWidget(manager.popupWidgetInstance, 0, 0, 1, 1);
        layout->setAlignment(Qt::AlignCenter);
        s_popupOverlayParent->setLayout(layout);
        s_popupOverlayParent->setVisible(false);
    }

    manager.popupWidgetInstance->setMessage(message);
    manager.popupWidgetInstance->setHeader(header);
    manager.popupWidgetInstance->setOkButtonText(okButtonText);
    manager.popupWidgetInstance->setCancelButtonText(cancelButtonText);
    manager.popupWidgetInstance->setCancelButtonVisible(showCancelButton);

    disconnect(manager.popupWidgetInstance, &PopupWidget::okClicked, nullptr, nullptr);
    disconnect(manager.popupWidgetInstance, &PopupWidget::cancelClicked, nullptr, nullptr);

    // Connect new callbacks
    if (okCallback) {
        connect(manager.popupWidgetInstance, &PopupWidget::okClicked, &manager, [okCallback]() {
            okCallback();
            PopupManager::hidePopup(); // Hide popup after callback
        });
    } else {
        connect(manager.popupWidgetInstance, &PopupWidget::okClicked, &manager, []() {
            PopupManager::hidePopup(); // Hide popup even without callback
        });
    }

    if (cancelCallback) {
        connect(manager.popupWidgetInstance, &PopupWidget::cancelClicked, &manager, [cancelCallback]() {
            cancelCallback();
            PopupManager::hidePopup(); // Hide popup after callback
        });
    } else {
        connect(manager.popupWidgetInstance, &PopupWidget::cancelClicked, &manager, []() {
            PopupManager::hidePopup(); // Hide popup even without callback
        });
    }

    s_popupOverlayParent->setVisible(true);
}

void PopupManager::hidePopup()
{
    PopupManager& manager = getInstance();
    if (manager.popupWidgetInstance) {
        manager.popupWidgetInstance->parentWidget()->setVisible(false);

        // disconnect all signals
        disconnect(manager.popupWidgetInstance, &PopupWidget::okClicked, nullptr, nullptr);
        disconnect(manager.popupWidgetInstance, &PopupWidget::cancelClicked, nullptr, nullptr);
    }
}