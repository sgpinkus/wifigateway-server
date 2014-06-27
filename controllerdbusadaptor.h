#ifndef CONTROLLERDBUSADAPTOR_H
#define CONTROLLERDBUSADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QString>
#include <QList>
#include "controller.h"

#define DBUS_NAME "com.epicmorsel.dbus"
#define DBUS_INTERFACE "com.epicmorsel.dbus.gateway"
#define DBUS_OBJECT_PATH "/com/epicmorsel/dbus/gateway"

/**
 * Pipes dbus method call directly to object implementing backend.
 * Nothing else.
 */
class ControllerDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.epicmorsel.dbus.gateway" )

public:
    ControllerDBusAdaptor(Controller * parent);

public slots:
    int newSession(QString IP);
    int setBandwidth(QString IP, qint32 bw);
    int setQuota(QString IP, qint32 quota);
    int pauseSession(QString IP);
    int playSession(QString IP);
    int endSession(QString IP);
    int updateSession(QString IP);
    SessionExport getStats(QString IP);
    QStringList getIPs();

private:
    Controller * controller;
};

#endif // CONTROLLERDBUSADAPTOR_H
