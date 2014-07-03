#include <QtGlobal>
#include <QSettings>
#include <stdexcept>
#include "common.h"
#include "files.h"
#include "gatewaycoreapplication.h"
#include "controller.h"
#include "controllerdbusadaptor.h"

int main(int argc, char *argv[])
{
  GatewayCoreApplication a(argc, argv);

  try
  {
    QSettings settings(CONFIG_FILE, QSettings::IniFormat);
    Controller controller(settings);
    ControllerDBusAdaptor adaptor(&controller);
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
