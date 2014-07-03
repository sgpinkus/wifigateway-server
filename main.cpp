#include <QtGlobal>
#include <QSettings>
#include <stdexcept>
#include <unistd.h>
#include "files.h"
#include "gatewaycoreapplication.h"
#include "controller.h"
#include "controllerdbusadaptor.h"
#include "handle_signals.h"


extern void sig_clean_up()
{
  if(qApp) {
    qApp->quit();
  }
}


int main(int argc, char *argv[])
{
  handle_signals(sig_clean_up);
  GatewayCoreApplication a(argc, argv);
  try
  {
    QSettings settings(CONFIG_FILE, QSettings::IniFormat);
    Controller controller(settings);
    ControllerDBusAdaptor adaptor(&controller);
    QObject::connect(qApp,SIGNAL(aboutToQuit()),&controller,SLOT(cleanup()));
    return a.exec();
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
