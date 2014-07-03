#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QtDBus>
#include <QString>
#include <QStringList>
#include <QSettings>
#include "runcommand/runcommand.h"
#include "sessionexport.h"
#include "quotabeacon.h"

class Controller : public QObject
{
  Q_OBJECT
public:
  explicit Controller(QSettings& settings, QObject *parent = 0);
  ~Controller();
  const static quint32 DEFAULT_BANDWIDTH = 1000;
  const static qint32 DEFAULT_QUOTA = 500;
  const static qint32 DEFAULT_TIME = -1;
  const static quint32 ACTIVITY_TIME = 60;
  const static quint32 UPDATE_TIME = 60;
  const static quint32 PAUSE_TIME = 60;
  const static quint32 END_TIME = 3600;
  const static quint32 TIME_WARNING_TIME = 30;
  const static quint32 UPDATE_WARNING_TIME = 15;
  const static quint32 ACTIVITY_WARNING_TIME = 15;
  const static quint32 PAUSE_WARNING_TIME = 15;
  typedef enum { UNINITIALIZED, STARTED, PAUSED, EXHAUSTED, ENDED } SessionState;

  // Main struct describing a session.
  typedef struct
  {
    QString IP;
    QString MAC;
    SessionState state;            // state.
    quint32 bandwidth;             // bandwidth limit on session. There is always a default.
    qint32 timeRemaining;          // time (s) used in the session. -1 if there is no limit.
    qint32 quotaRemaining;         // quota (~MB) used in session. -1 if there is no quota limit.
    quint32 activityTimeRemaining; // time how long there has been no activity from teh hhost for.
    quint32 updateTimeRemaining;   // watch dog timer
    quint32 pauseTimeRemaining;    // time how long been in paused state
    quint32 endTimeRemaining;      // time how long been in ended state
    quint32 sessionTime;           // overall time in started state.
  } Session;

public slots:
  int newSession(QString IP);
  int startSession(QString IP);
  int pauseSession(QString IP);
  int playSession(QString IP);
  int updateSession(QString IP);
  int endSession(QString IP);
  int setBandwidth(QString IP, quint32 bw);
  int setQuota(QString IP, qint32 quota);
  int setTime(QString IP, qint32 time);
  int updateQuotaRemaining(QString IP);
  Session getSession(QString IP);
  QList <Session> getAllSessions();
  QStringList getIPs();
  void tick();

private:
  RunCommand runcommand;
  QTimer tickTimer;
  QMap <QString,Session*> sessions;
  QuotaBeacon beacon;
  QSettings& settings;

  int exhaustSession(Session * session);
  bool isExhausted(Session * session);
  void initSession(Session * session, QString IP);
  void removeSession(QString IP);
  void removeSession(Session * session);
  void dumpSession(Session * session);
  void clearSessions();

private slots:
  void cleanup();

signals:
  void timeWarning();
  void updateTimeWarning();
  void activityTimeWarning();
  void pauseTimeWarning();
};

#endif // CONTROLLER_H
