/**
 * $HeadURL: file:///home/sam/svn/repos/gateway/code/logger.cpp $
 * $Revision: 3 $
 * $LastChangedBy: sam $
 * $LastChangedDate: 2013-01-05 12:45:58 +0800 (Sat, 05 Jan 2013) $
 */

#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include "logger.h"

Logger * Logger::logger = 0;

/**
 * Basic logger object.
 * This is a singleton object not a class - basically because of the date in log path.
 * Note there are two main log files; error and msg. But you can pass in any filename.
 * Pointers are kept to the open (they are actually open) for eff (lost if the file name changes alot).
 */
Logger::Logger(QObject *parent) : QObject(parent)
{
    QString logPath = QDir::homePath() + "/tmp/";
    logDir = new QDir();
    logDir->mkpath( logPath );
    logDir->cd( logPath );
    messageFile = new QFile(this);
    errorFile = new QFile(this);

    qDebug() << "Creating log dir:" << logDir->path();
    if ( ! logDir->exists() )
    {
        qDebug() << "Failed creating log dir!";
    }
}


/**
 * Static method to get logger instance.
 */
Logger * Logger::getLogger(QObject * parent)
{
    if( logger == 0 )
    {
        logger = new Logger(parent);
    }
    return logger;
}


/**
 * Get the logging dir. Used by custom logging.
 */
QString Logger::getLogDir()
{
  Logger * theLogger = getLogger();
  return theLogger->logDir->path();
}


/**
 * Log a message in the session log dir.
 */
void Logger::logMessage( QString message, QString file )
{
    QString logTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    messageFile->setFileName( logDir->path() + "/" + MESSAGE_LOG );

    if( ! file.isEmpty() )
    {
        messageFile->setFileName( logDir->path() + "/" + file );
    }

    if( messageFile->open( QIODevice::Append | QIODevice::ReadWrite | QIODevice::Unbuffered ) )
    {
        QTextStream out(messageFile);
        out << logTime + "," + message + "\n";
        messageFile->flush();
    }
    else
    {
        qDebug() << "******** Failed to write" << message << "to file" << messageFile->fileName();
    }
    messageFile->close();
}


/**
 * This is actually only used for logging doser specific errors ATM,
 * not general errors which we dont log.
 */
void Logger::logError( QString message, QString file )
{
    QString logTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    errorFile->setFileName( logDir->path() + "/" + ERROR_LOG );

    if( ! file.isEmpty() )
    {
        errorFile->setFileName( logDir->path() + "/" + file );
    }

    if( errorFile->open( QIODevice::Append | QIODevice::ReadWrite | QIODevice::Unbuffered ) )
    {
        QTextStream out(errorFile);
        out << logTime + "," + message + "\n";
        errorFile->flush();
    }
    else
    {
        qDebug() << "******** Failed to write" << message << "to file" << errorFile->fileName();
    }
    errorFile->close();
}
