#include "logger.h"
#include "data.h"

QString Logger::currentLogFile = "";
QFile Logger::logFileHandle;
QTextStream Logger::logStream;

bool Logger::initialize()
{
    QString logsDirPath = QCoreApplication::applicationDirPath() + "/logs";
    
    QDir logsDir(logsDirPath);
    if (!logsDir.exists()) {
        logsDir.mkpath(".");
    }
    
    currentLogFile = logsDirPath + "/latest.log";
    
    if (QFile::exists(currentLogFile)) {
        QFile::remove(currentLogFile);
    }
    
    logFileHandle.setFileName(currentLogFile);
    if (!logFileHandle.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open log file:" << logFileHandle.errorString();
        return false;
    }
    logStream.setDevice(&logFileHandle);
    
    logStream << "=== Application started at " 
              << Data::getInstance().getSimulatedDateTime().toString("yyyy-MM-dd HH:mm:ss")
              << " ===" << Qt::endl;
    
    return true;
}

void Logger::log(LogLevel level, const QString &message)
{
    if (!logStream.device())
        return; // No device available, so dont log

    QString timeStamp = Data::getInstance().getSimulatedDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    logStream << timeStamp << " [" << logLevelToString(level) << "] " 
              << message << Qt::endl;
    logStream.flush();
}

QString Logger::logLevelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::cleanup()
{
    // Write the application end log if the stream is valid
    if (logStream.device()) {
        logStream << "=== Application ended at " 
                  << Data::getInstance().getSimulatedDateTime().toString("yyyy-MM-dd HH:mm:ss")
                  << " ===" << Qt::endl;
        logStream.flush();
    }
    
    // Close the log file and disassociate the QTextStream device
    logFileHandle.close();
    logStream.setDevice(nullptr);
    
    // Rename latest.log to include the current date.
    QDateTime now = Data::getInstance().getSimulatedDateTime();
    QString dateString = now.toString("yyyy-MM-dd");
    QString logsDirPath = QCoreApplication::applicationDirPath() + "/logs";
    QString newFilename = logsDirPath + "/" + dateString + ".log";
    
    // Append time if the file already exists.
    if (QFile::exists(newFilename)) {
        newFilename = logsDirPath + "/" + dateString + "_" + now.toString("HH-mm-ss") + ".log";
    }
    
    QFile::rename(currentLogFile, newFilename);
}

QString Logger::getLogContents(const QString &logFile)
{
    QFile file(logFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Failed to open log file: " + file.errorString();
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

QString Logger::getLatestLogContents()
{
    return getLogContents(currentLogFile);
}

QStringList Logger::getAvailableLogFiles()
{
    QString logsDirPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir logsDir(logsDirPath);
    
    if (!logsDir.exists()) {
        logsDir.mkpath(".");
        return QStringList();
    }
    
    // Get all log files sorted by modification time (newest first)
    logsDir.setNameFilters(QStringList() << "*.log");
    QStringList files = logsDir.entryList(QDir::Files, QDir::Time);
    
    // If latest.log exists, make sure its first in the list
    if (files.contains("latest.log")) {
        files.removeAll("latest.log");
        files.prepend("latest.log");
    }
    
    return files;
}