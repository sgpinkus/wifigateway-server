#include "controllerdbusadaptor.h"

// Marshall the structureure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const SessionExport &structure)
{
    argument.beginStructure();
    argument << structure.IP
             << structure.MAC
             << structure.state
             << structure.time
             << structure.quota
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

// Retrieve the structureure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, SessionExport &structure)
{
    argument.beginStructure();
    argument >> structure.IP
             >> structure.MAC
             >> structure.state
             >> structure.time
             >> structure.quota
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

/**
 * Passthru everything to the actual object...
 * Forcing this seems sensless but its required.
 */
ControllerDBusAdaptor::ControllerDBusAdaptor(Controller * parent) : QDBusAbstractAdaptor(parent)
{
    controller = parent;
    // Dynamically register type for comms over dbus.
    qDBusRegisterMetaType<SessionExport>();
}

int ControllerDBusAdaptor::newSession(QString IP)
{
    return controller->newSession(IP);
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

int ControllerDBusAdaptor::updateSession(QString IP)
{
    return controller->updateSession(IP);
}

SessionExport ControllerDBusAdaptor::getStats(QString IP)
{
    SessionExport retval;
    Controller::Session stats = controller->getStats(IP);
    retval.IP = 	stats.IP;
    retval.MAC = 	stats.MAC;
    retval.state = 	stats.state;
    retval.time = 	stats.time;
    retval.quota = 	stats.quota;
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
