#include <stdexcept>
#include "controller.h"

/**
 * Implements a basic free access Internet gateway.
 * Is a wrapper on IPTables, providing the necessary state to manage sessions.
 * Session have time,quota,bandwidth as well as other timers. {@see Controller::Session}.
 * Quota is managed here rather than in IPTables so we know how much is used {@see QuotaBeacon}.
 * An object of this class is initialized then waits for connections requests and maintains state from there.
 * @todo settings - less lazy dep injection.
 */
Controller::Controller(QSettings& settings, QObject *parent) : QObject(parent),
  settings(settings)
{
  QString buf;
  int success;
  QString extIf = settings.value("extif","").toString();
  QString intIf = settings.value("intif","").toString();

  if(!extIf.length() || !intIf.length())
  {
    throw std::runtime_error("Config error. No interface def provided");
  }

  // script inits firewall and chain(s) assumed by add, rem, etc.
  QString cmd = QDir::current().path() + "/wifigateway-script/gw_init.sh EXTIF=%1 INTIF=%2";
  success = runcommand.runCommandExec(cmd.arg(extIf).arg(intIf), buf, 2000);
  if(success != 0)
  {
    throw std::runtime_error(QString("Failed initializing IPTables rules: %1; %2").arg(success).arg(buf).toAscii().data());
  }

  connect(&beacon,SIGNAL(quotaEvent(QString)),this,SLOT(updateQuotaRemaining(QString)));
  // all time updates off this.
  connect(&tickTimer,SIGNAL(timeout()),this,SLOT(tick()));
  tickTimer.start(1000);
}


Controller::~Controller()
{
  qDebug() << __FILE__ << __func__;
  cleanup();
}


/**
 * Do our best to tidy up our mess.
 */
void Controller::cleanup()
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  QString cmd = QDir::current().path() + "/wifigateway-script/gw_clean_up.sh";
  runcommand.runCommandExec(cmd, buf, 2000);
  clearSessions();
}


/**
 * Create a new session record, in the UNINITIALIZED state.
 * If record already exists 0 (success) is returned and no record changes.
 * Theres no current use case for calling the directly. Just use startSession().
 * @param IP
 * @return 0 on success.
 */
int Controller::newSession(QString IP)
{
  qDebug() << __FILE__ << __func__;

  if(!sessions.contains(IP))
  {
    Session * session = new Session;
    initSession(session, IP);
    sessions.insert(IP,session);
  }
  return 0;
}


/**
 * Start a session. Will create a session record if one does not exist.
 * Will change the state of the record to started if it is startable {@see startable()}.
 * @param IP
 * @return 0 on success
 */
int Controller::startSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  newSession(IP);
  Session * session = sessions.value(IP);
  if(!session)
  {
    success = -1;
  }
  else if(session->state == Controller::STARTED)
  {
    success = 0;
  }
  else if(!isExhausted(session))
  {
    QString buf;
    QString cmd = QDir::current().path()
                  + "/wifigateway-script/gw_add_host.sh "
                  + IP + " "
                  + QString::number(session->bandwidth) + " "
                  + QString::number(session->quotaRemaining);
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
      session->activityTimeRemaining = Controller::ACTIVITY_TIME;
      session->updateTimeRemaining = Controller::UPDATE_TIME;
    }
  }
  return success;
}


/**
 * Remove access from host and stop timers for specifed period or until play().
 * Session will restart auto after period.
 * If host is already paused, not a failure as gw_remove_host.sh returns -1 if host DNE in rules.
 * @returns 0 on success, the result of IPTables add or -1 on other failure.
 */
int Controller::pauseSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;
  Session * session = sessions.value(IP);

  if(!session)
  {
    success = -1;
  }
  else if(session->state == Controller::PAUSED)
  {
    success = 0;
  }
  else if(session->state == Controller::STARTED)
  {
    QString cmd = QDir::current().path() + "/wifigateway-script/gw_remove_host.sh " + IP;
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
 * @returns 0 on success, the result of IPTables add or -1 on other failure.
 */
int Controller::playSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  Session * session = sessions.value(IP);

  if(!session)
  {
    success = -1;
  }
  else if(session->state == Controller::STARTED)
  {
    success = 0;
  }
  else if(session->state == Controller::PAUSED)
  {
    success = startSession(IP);
    if(!success)
    {
      qDebug() << "Failed IPTables rule add " << success << buf;
    }
  }
  return success;
}


/**
 * End the session.
 * Preserve a record of it. This record will be thawed if a client matching ident attempt to connect later.
 * @returns 0 on success, the result of IPTables add or -1 on other failure.
 */
int Controller::endSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  Session * session = sessions.value(IP);

  if(!session)
  {
    success = -1;
  }
  else if(session->state == Controller::ENDED)
  {
    success = 0;
  }
  else
  {
    QString cmd = QDir::current().path() + "/wifigateway-script/gw_remove_host.sh " + IP;
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
  return success;
}


/**
 * Private function to move session to exhausted state.
 * @param session
 */
int Controller::exhaustSession(Session * session)
{
  qDebug() << __FILE__ << __func__;
  QString buf;
  int success = -1;

  if(!session)
  {
    success = -1;
  }
  else if(session->state == Controller::EXHAUSTED)
  {
    success = 0;
  }
  else
  {
    QString cmd = QDir::current().path() + "/wifigateway-script/gw_remove_host.sh " + session->IP;
    success = runcommand.runCommandExec(cmd, buf, 2000);
    if(success == 0)
    {
      session->state = Controller::EXHAUSTED;
    }
    else
    {
      qDebug() << "Failed IPTables rule remove " << success << buf;
    }
  }
  return success;
}


/**
 * Controller::isExhausted.
 * @return whether session is exhausted.
 */
bool Controller::isExhausted(Session * session)
{
  Q_ASSERT(session);

  bool exhausted = false;
  if(session->timeRemaining == 0)
  {
    exhausted = true;
  }
  if(session->quotaRemaining == 0)
  {
    exhausted = true;
  }
  return exhausted;
}


/**
 * Reset update counter to prevent session being ended.
 * Scenario: Script in a browser window triggers this s.t. when the window is closed the update ceases.
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

  Session * session = sessions.value(IP);
  if(session)
  {
    session->quotaRemaining = (session->quotaRemaining > 0 ? session->quotaRemaining - 1 : session->quotaRemaining);
    success = 0;
    qDebug() << "Quota updated OK" << session->quotaRemaining;
  }
  return success;
}


/**
 * Updates all timers.
 * Mor eff than maintaining num session timers.
 */
void Controller::tick()
{
  qDebug() << __FILE__ << __func__;

  foreach(Session * session, sessions)
  {
    dumpSession(session);

    switch(session->state)
    {
      case Controller::STARTED :
      {
        session->timeRemaining = (session->timeRemaining > 0 ? session->timeRemaining - 1 : session->timeRemaining);
        session->updateTimeRemaining = (session->updateTimeRemaining > 0 ? session->updateTimeRemaining - 1 : 0);
        // not implmented yet.
        // session->activityTimeRemaining = (session->activityTimeRemaining > 0 ? session->activityTimeRemaining - 1 : 0);
        session->sessionTime++;

        // Transistion out of started EXHAUSTED | ENDED.
        if(isExhausted(session))
        {
          exhaustSession(session);
        }
        if(session->updateTimeRemaining == 0)
        {
          endSession(session->IP);
        }
        if(session->activityTimeRemaining == 0)
        {
          endSession(session->IP);
        }

        // Emit time warnings in running state.
        if((uint)session->timeRemaining == Controller::TIME_WARNING_TIME)
        {
          emit timeWarning();
        }
        if(session->updateTimeRemaining == Controller::UPDATE_WARNING_TIME)
        {
          emit updateTimeWarning();
        }
        if(session->activityTimeRemaining == Controller::ACTIVITY_WARNING_TIME)
        {
          emit activityTimeWarning();
        }
        break;
      }
      case Controller::EXHAUSTED :
      {
        // The update (watchdog) timer should eventually time out. If it doesnt we should maintain EXHAUSTED state.
        session->updateTimeRemaining = (session->updateTimeRemaining > 0 ? session->updateTimeRemaining - 1 : 0);
        // not implmented yet.
        // session->activityTimeRemaining = (session->activityTimeRemaining > 0 ? session->activityTimeRemaining - 1 : 0);
        session->sessionTime++;

        if(session->updateTimeRemaining == 0)
        {
          endSession(session->IP);
        }
        if(session->activityTimeRemaining == 0)
        {
          endSession(session->IP);
        }
        break;
      }
      case Controller::PAUSED :
      {
        session->pauseTimeRemaining = (session->pauseTimeRemaining > 0 ? session->pauseTimeRemaining - 1 : 0);
        if(session->pauseTimeRemaining == 0)
        {
          // Just start is again. Sorry.
          session->pauseTimeRemaining = Controller::PAUSE_TIME;
          playSession(session->IP);
        }
        if(session->pauseTimeRemaining == Controller::PAUSE_WARNING_TIME)
        {
          emit pauseTimeWarning();
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
  session->bandwidth = Controller::DEFAULT_BANDWIDTH;
  session->quotaRemaining = Controller::DEFAULT_QUOTA;
  session->timeRemaining = Controller::DEFAULT_TIME;
  session->activityTimeRemaining = Controller::ACTIVITY_TIME;
  session->updateTimeRemaining = Controller::UPDATE_TIME;
  session->pauseTimeRemaining = Controller::PAUSE_TIME;
  session->endTimeRemaining = Controller::END_TIME;
  session->sessionTime = 0;
}


/**
 * Set bandwidth. Pause/play is a hack to get it into IP tables.
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
 * Set quota. Pause/play is a hack to get it into IP tables.
 */
int Controller::setQuota(QString IP, qint32 quota)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->quotaRemaining = quota;
    pauseSession(IP);
    playSession(IP);
    success = 0;
  }
  return success;
}

/**
 * Set time outright.
 */
int Controller::setTime(QString IP, qint32 time)
{
  qDebug() << __FILE__ << __func__;
  int success = -1;

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    session->timeRemaining = time;
    success = 0;
  }
  return success;
}


/**
 * One emethod to get all params, client must know struct.
 */
Controller::Session Controller::getSession(QString IP)
{
  qDebug() << __FILE__ << __func__;
  Controller::Session retval = {"","",Controller::UNINITIALIZED,0,0,0,0,0,0,0,0};

  if(sessions.contains(IP))
  {
    Session * session = sessions.value(IP);
    retval = *session;
    retval.IP = IP;
  }
  return retval;
}


QList <Controller::Session> Controller::getAllSessions()
{
  QList <Controller::Session> list;
  foreach(Session * s, sessions)
  {
    list.append(*s);
  }
  return list;
}


QStringList Controller::getIPs()
{
  QStringList list = sessions.keys();
  return list;
}


void Controller::dumpSession(Controller::Session * session)
{
  qDebug() << "####" << session->IP << "###";
  qDebug() << "IP:" << session->IP;
  qDebug() << "MAC:" << session->MAC;
  qDebug() << "state:" << session->state;
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


void Controller::clearSessions()
{
  qDebug() << __FILE__ << __func__;
  foreach(Session * s, sessions)
  {
    delete s;
  }
  sessions.clear();
}
