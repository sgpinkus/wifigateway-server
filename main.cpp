#include <QtGlobal>
#include <QSettings>
#include <stdexcept>
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include "files.h"
#include "gatewaycoreapplication.h"
#include "controller.h"
#include "controllerdbusadaptor.h"
#include "handle_signals.h"


/**
 * Clean up on terminal signal.
 */
int sig_clean_up(int signum)
{
  int cont = 0;
  static int continued = 0;

  if(signum == SIGBUS || signum == SIGFPE || signum == SIGILL || signum == SIGSEGV)
  {
    write(STDERR_FILENO, "Abnormal Termination.\n", 22);
  }
  if(qApp)
  {
    qApp->quit();
    cont = 1;
  }
  if(continued)
  {
    cont = 0;
  }
  continued = 1;
  return cont;
}

/**
 * Simple pipe to syslog. Default Qt fatal generates a core dump and terminates.
 * Termination is handy, maintain that. If you want a core dump use SIGABRT.
 * @todo qFatal() still terminates app abruptly so qApp->quit() is a null op!
 * @param type Qt message type - there is only 4.
 * @param msg
 */
void sysLogMsgHandler(QtMsgType type, const char *msg)
{
  openlog(NULL, LOG_PID, LOG_DAEMON);
  switch (type) {
    #ifndef QT_NO_DEBUG_OUTPUT
    case QtDebugMsg:
      syslog(LOG_DEBUG, "DEBUG: %s", msg);
      break;
    #endif
    case QtWarningMsg:
      syslog(LOG_WARNING, "WARNING: %s", msg);
      break;
    case QtCriticalMsg:
      syslog(LOG_CRIT, "CRITICAL: %s", msg);
      break;
    case QtFatalMsg:
      syslog(LOG_DAEMON|LOG_ALERT, "FATAL: %s", msg);
      qApp->quit();
  }
}


int main(int argc, char *argv[])
{
  handle_signals(sig_clean_up);
#ifndef TESTING
  qInstallMsgHandler(sysLogMsgHandler);
#endif
  GatewayCoreApplication app(argc, argv);

  try
  {
    QSettings settings(CONFIG_FILE, QSettings::IniFormat);
    Controller controller(settings);
    ControllerDBusAdaptor adaptor(&controller);
    QObject::connect(qApp,SIGNAL(aboutToQuit()),&controller,SLOT(cleanup()), Qt::UniqueConnection);
    return app.exec();
  }
  catch(std::runtime_error& e)
  {
    qFatal(e.what());
  }
  catch(std::exception& e)
  {
    qFatal(e.what());
  }
}
