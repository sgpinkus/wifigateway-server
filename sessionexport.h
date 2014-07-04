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
  quint32 bandwidth;
  qint32 timeRemaining;
  qint32 quotaRemaining;
  quint32 activityTimeRemaining;
  quint32 updateTimeRemaining;
  quint32 pauseTimeRemaining;
  quint32 endTimeRemaining;
  quint32 sessionTime;
};
Q_DECLARE_METATYPE(SessionExport)

#endif // SESSIONEXPORT_H
