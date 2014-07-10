#include <QtDebug>
#include <stdexcept>
#include "controllerdbusadaptor.h"
#include "controller.h"
#define QT_NO_DEBUG_OUTPUT 1

/**
 * Passes thru everything to the actual object from the adaptor.
 * Note since we using the system dbus, you need to configure permissions for this app.
 */
ControllerDBusAdaptor::ControllerDBusAdaptor(Controller * parent) : QDBusAbstractAdaptor(parent),
  controller(parent),
  bus(QDBusConnection::connectToBus(QDBusConnection::SystemBus, DBUS_SERVICE))
{
  if(!bus.isConnected())
  {
    throw std::runtime_error("No DBUS");
  }
  if(!registerService())
  {
    throw std::runtime_error("DBUS service registration failed");
  }
  if(!registerObject())
  {
    throw std::runtime_error("DBUS object registration failed");
  }

  qDebug() << "DBus connection established. BaseName=" << bus.baseService() << ", Name=" << bus.name();
  // Dynamically register type for comms over dbus.
  qDBusRegisterMetaType<SessionExport>();
}


bool ControllerDBusAdaptor::registerService()
{
  int retries = 0;
  while(!bus.registerService(DBUS_SERVICE) && retries++ < DBUS_RETRIES)
  {
    qDebug() << "DBUS service registration of name failed. Retrying.";
    sleep(1);
  }
  return (retries<DBUS_RETRIES);
}


bool ControllerDBusAdaptor::registerObject()
{
  int retries = 0;
  // connect to D-Bus and register Object - not the adaptor.
  while(!bus.registerObject(DBUS_OBJECT_PATH, controller) && retries++ < DBUS_RETRIES)
  {
    qDebug() << "DBUS object registration failed";
    sleep(1);
  }
  return (retries<DBUS_RETRIES);
}


/**
 * Marshall the structured data into a D-Bus argument.
 */
QDBusArgument &operator<<(QDBusArgument &argument, const SessionExport &structure)
{
  argument.beginStructure();
  argument << structure.IP
           << structure.MAC
           << structure.state
           << structure.bandwidth
           << structure.timeRemaining
           << structure.quotaRemaining
           << structure.activityTimeRemaining
           << structure.updateTimeRemaining
           << structure.pauseTimeRemaining
           << structure.endTimeRemaining
           << structure.sessionTime;
  argument.endStructure();
  return argument;
}


/**
 * Retrieve the structured data from the D-Bus argument.
 */
const QDBusArgument &operator>>(const QDBusArgument &argument, SessionExport &structure)
{
  argument.beginStructure();
  argument >> structure.IP
           >> structure.MAC
           >> structure.state
           >> structure.bandwidth
           >> structure.timeRemaining
           >> structure.quotaRemaining
           >> structure.activityTimeRemaining
           >> structure.updateTimeRemaining
           >> structure.pauseTimeRemaining
           >> structure.endTimeRemaining
           >> structure.sessionTime;
  argument.endStructure();
  return argument;
}

int ControllerDBusAdaptor::newSession(QString IP)
{
  return controller->newSession(IP);
}

int ControllerDBusAdaptor::startSession(QString IP)
{
  return controller->startSession(IP);
}

int ControllerDBusAdaptor::pauseSession(QString IP)
{
  return controller->pauseSession(IP);
}

int ControllerDBusAdaptor::playSession(QString IP)
{
  return controller->playSession(IP);
}

int ControllerDBusAdaptor::endSession(QString IP)
{
  return controller->endSession(IP);
}

int ControllerDBusAdaptor::deleteSession(QString IP)
{
  return controller->deleteSession(IP);
}

int ControllerDBusAdaptor::updateSession(QString IP)
{
  return controller->updateSession(IP);
}

SessionExport ControllerDBusAdaptor::getSession(QString IP)
{
  SessionExport retval;
  Controller::Session stats = controller->getSession(IP);
  retval.IP = 	stats.IP;
  retval.MAC = 	stats.MAC;
  retval.state = 	stats.state;
  retval.bandwidth = 	stats.bandwidth;
  retval.timeRemaining = 	stats.timeRemaining;
  retval.quotaRemaining = 	stats.quotaRemaining;
  retval.activityTimeRemaining = 	stats.activityTimeRemaining;
  retval.updateTimeRemaining = 	stats.updateTimeRemaining;
  retval.pauseTimeRemaining = 	stats.pauseTimeRemaining;
  retval.endTimeRemaining =	stats.endTimeRemaining;
  retval.sessionTime =	stats.sessionTime;
  return retval;
}

QStringList ControllerDBusAdaptor::getIPs()
{
  return controller->getIPs();
}

int ControllerDBusAdaptor::setBandwidth(QString IP, qint32 bw)
{
  return controller->setBandwidth(IP, bw);
}

int ControllerDBusAdaptor::setQuota(QString IP, qint32 quota)
{
  return controller->setQuota(IP, quota);
}

int ControllerDBusAdaptor::setTime(QString IP, qint32 time)
{
  return controller->setQuota(IP, time);
}

int ControllerDBusAdaptor::setDefaultBandWidth(quint32 bw)
{
  return controller->setDefaultBandWidth(bw);
}

int ControllerDBusAdaptor::setDefaultQuota(qint32 quota)
{
  return controller->setDefaultQuota(quota);
}

int ControllerDBusAdaptor::setDefaultTime(qint32 time)
{
  return controller->setDefaultTime(time);
}

SessionExport ControllerDBusAdaptor::getDefaultSession()
{
  SessionExport retval;
  Controller::Session s = controller->getDefaultSession();
  retval.bandwidth = s.bandwidth;
  retval.quotaRemaining = s.quotaRemaining;
  retval.timeRemaining = s.timeRemaining;
  return retval;
}

int ControllerDBusAdaptor::setDefaultSession(const SessionExport& s)
{
  Controller::Session new_s;
  new_s.bandwidth = s.bandwidth;
  new_s.quotaRemaining = s.quotaRemaining;
  new_s.timeRemaining = s.timeRemaining;
  return controller->setDefaultSession(new_s);
}
