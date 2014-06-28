#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QtDBus>
#include <QString>
#include <QStringList>
#include "runcommand/runcommand.h"
#include "sessionexport.h"
#include "quotabeacon.h"

class Controller : public QObject
{
  Q_OBJECT
public:
  explicit Controller(QObject *parent = 0);
  const static quint32 DEFAULT_QUOTA = 500;
  const static quint32 DEFAULT_TIME = 0;
  const static quint32 DEFAULT_BANDWIDTH = 1000;
  const static quint32 UPDATE_TIME = 30;
  const static quint32 ACTIVITY_TIME = 60;
  const static quint32 PAUSE_TIME = 30;
  const static quint32 END_TIME = 10;
  const static quint32 TIME_WARNING_TIME = 3;
  const static quint32 ACTIVITY_WARNING_TIME = 3;
  const static quint32 PAUSE_WARNING_TIME = 3;
  typedef enum { UNINITIALIZED, STARTED, PAUSED, ENDED } SessionState;

  // Main struct describing a session.
  typedef struct
  {
    QString IP;
    QString MAC;
    SessionState state;
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
  } Session;

public slots:
  int newSession(QString IP);
  int pauseSession(QString IP);
  int playSession(QString IP);
  int endSession(QString IP);
  int updateSession(QString IP);
  int startSession(QString IP);
  int setBandwidth(QString IP, qint32 bw);
  int setQuota(QString IP, qint32 quota);
  int updateQuotaRemaining(QString IP);
  Session getStats(QString IP);
  QList <Session> getAllStats();
  QStringList getIPs();

  void tick();

private:
  RunCommand runcommand;
  QTimer tickTimer;
  QMap <QString,Session*> sessions;
  QuotaBeacon beacon;

  void initSession(Session * session, QString IP);
  void removeSession(QString IP);
  void removeSession(Session * session);
  void dumpSession(Session * session);

signals:
  void timeWarning();
  void activityWarning();
  void pauseWarning();

};

#endif // CONTROLLER_H
