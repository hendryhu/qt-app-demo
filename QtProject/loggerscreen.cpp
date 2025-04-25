#include "loggerscreen.h"
#include "logger.h"
#include "ui_loggerscreen.h"
#include <QDateTime>
#include <QFile>

LoggerScreen::LoggerScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerScreen)
{
    ui->setupUi(this);
    Logger::initialize(); // Initialize log folder 

    // Populate the combo box with available log files,
    populateLogFiles();
    
    // Connect UI buttons to slots.
    connect(ui->refreshButton, &QPushButton::clicked, this, &LoggerScreen::refreshLog);
    connect(ui->clearButton, &QPushButton::clicked, this, &LoggerScreen::clearLog);
    //connect(ui->testLogButton, &QPushButton::clicked, this, &LoggerScreen::addTestLogEntry);
    
    Logger::info("Log viewer opened");
    refreshLog();
}

LoggerScreen::~LoggerScreen()
{
    Logger::cleanup();
    delete ui;
}

void LoggerScreen::populateLogFiles()
{
    ui->logFileComboBox->clear();
    
    QStringList logFiles = Logger::getAvailableLogFiles();
    
    // If we have log files, add them to the combobox
    if (!logFiles.isEmpty()) {
        ui->logFileComboBox->addItems(logFiles);
        
        int latestLogIndex = logFiles.indexOf("latest.log");
        if (latestLogIndex >= 0) {
            ui->logFileComboBox->setCurrentIndex(latestLogIndex);
        } else {
            // Default to first (newest) log if latest.log not found
            ui->logFileComboBox->setCurrentIndex(0);
        }
    }
    
    // Refresh the log content
    refreshLog();
}
void LoggerScreen::refreshLog()
{
    if (ui->logFileComboBox->count() == 0) {
        ui->logTextEdit->setPlainText("No log files available.");
        return;
    }
    
    QString selectedFile = ui->logFileComboBox->currentText();
    if (selectedFile.isEmpty()) {
        return;
    }
    
    QString logContent = Logger::getLogContents(QCoreApplication::applicationDirPath() + 
                                               "/logs/" + selectedFile);
    
    ui->logTextEdit->setPlainText(logContent);
}


void LoggerScreen::clearLog()
{
    ui->logTextEdit->clear();
    ui->logTextEdit->append("Log cleared at " 
                          + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

// void LoggerScreen::addTestLogEntry()
// {
//     QStringList testEntries = {
//         "Bolus delivery started: 3.5 units",
//         "Basal rate changed to 0.75 units/hour",
//         "Battery level: 72%",
//         "Insulin reservoir: 120 units remaining",
//         "Blood glucose reading: 142 mg/dL",
//         "Alarm: Occlusion detected",
//         "Auto-off feature activated"
//     };
    
//     int index = QDateTime::currentDateTime().time().msec() % testEntries.size();
//     QString entry = testEntries[index];
    
//     Logger::info(entry);
//     refreshLog();
// }
