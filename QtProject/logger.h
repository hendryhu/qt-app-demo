#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
};

class Logger
{
public:
    // Initializes the logger and creates a fresh latest.log.
    static bool initialize();
    
    // Logging methods
    static void log(LogLevel level, const QString &message);
    static void info(const QString &message)    { log(LogLevel::INFO, message); }
    static void warning(const QString &message) { log(LogLevel::WARNING, message); }
    static void error(const QString &message)   { log(LogLevel::ERROR, message); }
    
    // Cleanup method: writes end message, closes file and renames latest.log to include the date.
    static void cleanup();
    
    // Log retrieval methods
    static QString getLogContents(const QString &logFile);
    static QString getLatestLogContents();
    static QStringList getAvailableLogFiles();

private:
    static QString logLevelToString(LogLevel level);
    static QString currentLogFile;
    static QFile logFileHandle;
    static QTextStream logStream;
};

#endif // LOGGER_H
