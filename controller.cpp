#include <stdexcept>
#include "controller.h"

/**
 * Implements a basic free access Internet gateway.
 * Basic as in no user persistence or awareness, only sessions.
 * Is a wrapper on IPTables providing the necessary state to manage sessions.
 * Session have time,quota,bandwidth as well as other timers. {@see Controller::Session}.
 * Note quota is managed here rather than in IPTables so we know how much is used.
 * @see QuotaBeacon.
 */
Controller::Controller(QObject *parent) : QObject(parent)
{
  QString buf;
  int success;

  // script inits firewall and chain(s) assumed by add, rem, etc.
  QString cmd = QDir::current().path() + "/script/gw_init.sh";
  success = runcommand.runCommandExec(cmd, buf, 2000);
  if(success != 0)
  {
    throw std::runtime_error(QString("Failed initializing IPTables rules: %1; %2").arg(success).arg(buf).toAscii().data());
  }

  connect(&beacon,SIGNAL(quotaEvent(QString)),this,SLOT(updateQuotaRemaining(QString)));
  // all time updates off this.
  connect(&tickTimer,SIGNAL(timeout()),this,SLOT(tick()));
  tickTimer.start(1000);
}


/**
 * Do our best to tidy up our mess.
 */
Controller::~Controller()
{
  QString buf;
  QString cmd = QDir::current().path() + "/script/gw_clean_up.sh";
  runcommand.runCommandExec(cmd, buf, 2000);
}


/**
 * Allow a host with given IP access to internet, inserting IPTables rule(s), and initing a record.
 * If record already exists 0 (success) is returned and no record changes.
 * @returns Zero on success, the result of IPTables add, or -1 on other failure.
 */
int Controller::newSession(QString IP)
{
  sleep(1);
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(!sessions.contains(IP))
  {
    Session * session = new Session;
    initSession(session, IP);
    sessions.insert(IP,session);
    success = startSession(IP);
    if(success != 0)
    {
      removeSession(IP);
    }
  }
  else
  {
    success = 0;
  }
  return success;
}


int Controller::startSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    QString buf;
    QString cmd = QDir::current().path()
                  + "/script/gw_add_host.sh "
                  + IP + " "
                  + QString::number(session->bandwidth) + " "
                  + QString::number(session->quota);
    qDebug() << cmd;
    success = runcommand.runCommandExec(cmd, buf, 2000);
    qDebug() << buf;
    if(success != 0)
    {
      qDebug() << "Failed IPTables rule insert " << success << buf;
    }
    else
    {
      session->state = Controller::STARTED;
    }
  }
  return success;
}


/**
 * Remove access from host and stop timers for specifed period or until play().
 * Session will restart auto after period.
 * If host is already paused, not a failure as gw_remove_host.sh returns -1 if host DNE in rules.
 * @returns Zero on success, the result of IPTables add or -1 on other failure.
 */
int Controller::pauseSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  if(sessions.contains(IP))
  {
    QString cmd = QDir::current().path() + "/script/gw_remove_host.sh " + IP;
    Session * session = sessions.value(IP);
    success = runcommand.runCommandExec(cmd, buf, 2000);
    if(success == 0)
    {
      session->state = Controller::PAUSED;
    }
    else
    {
      qDebug() << "Failed IPTables rule remove " << success << buf;
    }
  }
  return success;
}


/**
 * Resume paused session.
 * If session exists and is already in STARTED state, has no effect and return 0 (success).
 * @returns Zero on success, the result of IPTables add or -1 on other failure.
 */
int Controller::playSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    if(session->state == Controller::PAUSED)
    {
      success = startSession(IP);
      if(success == 0)
      {
        session->activityTimeRemaining = Controller::ACTIVITY_TIME;
        session->updateTimeRemaining = Controller::UPDATE_TIME;
      }
      else
      {
        qDebug() << "Failed IPTables rule add " << success << buf;
      }
    }
    else if(session->state == Controller::STARTED)
    {
      success = 0;
    }
  }
  return success;
}


/**
 * End the session, keeping a record of it for a time to prevent reopening.
 * @returns Zero on success, the result of IPTables add or -1 on other failure.
 */
int Controller::endSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    if(session->state != Controller::ENDED)
    {
      QString cmd = QDir::current().path() + "/script/gw_remove_host.sh " + IP;
      success = runcommand.runCommandExec(cmd, buf, 2000);
      if(success == 0)
      {
        session->state = Controller::ENDED;
      }
      else
      {
        qDebug() << "Failed IPTables rule remove " << success << buf;
      }
    }
    else
    {
      success = 0;
    }
  }
  return success;
}


/**
 * Reset counter update counter to prevent session being ended.
 * Script in a browser window triggers this s.t. when the window is closed the session ends promptly.
 */
int Controller::updateSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->updateTimeRemaining = Controller::UPDATE_TIME;
    success = 0;
  }
  return success;
}


/**
 * Decrement quota remaining be one unit - MB.
 * @see QuotaBeacon.
 */
int Controller::updateQuotaRemaining(QString IP)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->quotaRemaining--;
    success = 0;
    qDebug() << "OK" << session->quotaRemaining;
  }
  return success;
}


/**
 * Set bandwidth. Hack to get it into IP tables.
 */
int Controller::setBandwidth(QString IP, quint32 bw)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->bandwidth = bw;
    pauseSession(IP);
    playSession(IP);
    success = 0;
  }
  return success;
}


/**
 * Set quota. Hack to get it into IP tables.
 */
int Controller::setQuota(QString IP, quint32 quota)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->quota = quota;
    pauseSession(IP);
    playSession(IP);
    success = 0;
  }
  return success;
}


/**
 * One emethod to get all params, client must know struct.
 */
Controller::Session Controller::getStats(QString IP)
{
  qDebug() << __FILE__ << __func__;
  Controller::Session retval = {"","",Controller::UNINITIALIZED,0,0,0,0,0,0,0,0,0,0};

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    retval = *session;
    retval.IP = IP;
  }
  return retval;
}


QList <Controller::Session> Controller::getAllStats()
{
  QList <Controller::Session> list;
  return list;
}


QStringList Controller::getIPs()
{
  QStringList list = sessions.keys();
  return list;
}

/**
 * Updates all timers.
 * Mor eff than maintaining num session timers.
 */
void Controller::tick()
{
  qDebug() << __FILE__ << __func__;
  foreach(Session * session,sessions)
  {
    dumpSession(session);
    switch(session->state)
    {
    case Controller::STARTED :
    {
      session->pauseTimeRemaining = Controller::PAUSE_TIME;
      session->timeRemaining = (session->timeRemaining > 0 ? session->timeRemaining - 1 : 0);
      session->updateTimeRemaining = (session->updateTimeRemaining > 0 ? session->updateTimeRemaining - 1 : 0);
      //implement activity ping from IPTables.
      //session->activityTimeRemaining = (session->activityTimeRemaining > 0 ? session->activityTimeRemaining - 1 : 0);
      session->sessionTime++;

      if((session->time > 0) && session->timeRemaining == 0)
      {
        endSession(session->IP);
      }
      else if(session->activityTimeRemaining == 0)
      {
        endSession(session->IP);
      }
      else if(session->updateTimeRemaining == 0)
      {
        endSession(session->IP);
      }
      else if(session->quotaRemaining == 0)
      {
        endSession(session->IP);
      }
      else if(session->timeRemaining <= Controller::TIME_WARNING_TIME)
      {
        emit timeWarning();
      }
      else if(session->activityTimeRemaining <= Controller::ACTIVITY_WARNING_TIME)
      {
        emit activityWarning();
      }
      break;
    }
    case Controller::PAUSED :
    {
      session->pauseTimeRemaining = (session->pauseTimeRemaining > 0 ? session->pauseTimeRemaining - 1 : 0);
      if(session->pauseTimeRemaining == 0)
      {
        session->pauseTimeRemaining = Controller::PAUSE_TIME;
        playSession(session->IP);

      }
      if(session->pauseTimeRemaining <= Controller::PAUSE_WARNING_TIME)
      {
        emit pauseWarning();
      }
      break;
    }
    case Controller::ENDED :
    {
      session->endTimeRemaining = (session->endTimeRemaining ? session->endTimeRemaining - 1 : 0);
      if(session->endTimeRemaining == 0)
      {
        removeSession(session->IP);
      }
      break;
    }
    default:
    {}
    }
  }
}


void Controller::initSession(Controller::Session * session, QString IP)
{
  qDebug() << __FILE__ << __func__;
  session->state = Controller::UNINITIALIZED;
  session->IP = IP;
  session->MAC = QString();
  session->time = Controller::DEFAULT_TIME;
  session->quota = Controller::DEFAULT_QUOTA;
  session->bandwidth = Controller::DEFAULT_BANDWIDTH;
  session->quotaRemaining = session->quota;
  session->timeRemaining = session->time;
  session->activityTimeRemaining = Controller::ACTIVITY_TIME;
  session->updateTimeRemaining = Controller::UPDATE_TIME;
  session->pauseTimeRemaining = Controller::PAUSE_TIME;
  session->endTimeRemaining = Controller::END_TIME;
  session->sessionTime = 0;
}


void Controller::dumpSession(Controller::Session * session)
{
  qDebug() << "####" << session->IP << "###";
  qDebug() << "IP:" << session->IP;
  qDebug() << "MAC:" << session->MAC;
  qDebug() << "state:" << session->state;
  qDebug() << "time: " << session->time;
  qDebug() << "quota: " << session->quota;
  qDebug() << "bandwidth: " << session->bandwidth;
  qDebug() << "t rem: " << session->timeRemaining;
  qDebug() << "q rem: " << session->quotaRemaining;
  qDebug() << "a rem: " << session->activityTimeRemaining;
  qDebug() << "u rem: " << session->updateTimeRemaining;
  qDebug() << "p rem: " << session->pauseTimeRemaining;
  qDebug() << "e rem: " << session->endTimeRemaining;
  qDebug() << "t ses: " << session->sessionTime;
}


void Controller::removeSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    sessions.remove(IP);
    delete session;
  }
}


void Controller::removeSession(Session * session)
{
  qDebug() << __FILE__ << __func__;
  QString key = sessions.key(session);
  if(key.length() == 0)
  {
    sessions.remove(key);
    delete session;
  }
}
