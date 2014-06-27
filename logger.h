#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QString>
#include "files.h"


#define LogMessage(msg) Logger::getLogger()->logMessage( QString( "file:" ) + __FILE__ + QString(",function:") + __func__ + QString(",line:") + QString::number( __LINE__ ) + QString(": ") + msg );
#define LogError(msg) Logger::getLogger()->logError( QString( "file:" ) + __FILE__ + QString(",function:") + __func__ + QString(",line:") + QString::number( __LINE__ ) + QString(": ") + msg );

/**
 * Implemented as singleton but there is little use in it ATM.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    void logMessage( QString message, QString file = QString() );
    void logError( QString message, QString file = QString() );
    static Logger * getLogger(QObject * parent = 0);
    static QString getLogDir();

private:
    QDir * logDir;
    QFile * messageFile;
    QFile * errorFile;
    static Logger * logger;

    explicit Logger(QObject *parent = 0);

signals:

public slots:

};



#endif // LOGGER_H
