#include <QtGlobal>
#include <stdexcept>
#include "gatewaycoreapplication.h"
#include "controller.h"
#include "controllerdbusadaptor.h"


int main(int argc, char *argv[])
{
  GatewayCoreApplication a(argc, argv);

  try
  {
    Controller controller;
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
