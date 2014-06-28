#ifndef SESSIONEXPORT_H
#define SESSIONEXPORT_H

#include <QString>
#include <QDBusMetaType>

/**
 * Exactly the same as Controller::Session,
 * but declared for marshalling data in/out of dbus.
 */
struct SessionExport
{
    QString IP;
    QString MAC;
    qint32 state;
    qint32 time;
    qint32 quota;
    qint32 bandwidth;
    qint32 timeRemaining;
    qint32 quotaRemaining;
    qint32 activityTimeRemaining;
    qint32 updateTimeRemaining;
    qint32 pauseTimeRemaining;
    qint32 endTimeRemaining;
    qint32 sessionTime;
};
Q_DECLARE_METATYPE(SessionExport)

#endif // SESSIONEXPORT_H
