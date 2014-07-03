#ifndef CONTROLLERDBUSADAPTOR_H
#define CONTROLLERDBUSADAPTOR_H

#include <QtDBus>
#include <QtDebug>
#include <QString>
#include <QList>
#include <QDBusMetaType>
#include <QDBusAbstractAdaptor>
#include "sessionexport.h"
#include "dbus.h"
class Controller;

/**
 * Pipes dbus method calls to object implementing the backend.
 */
class ControllerDBusAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.epicmorsel.dbus.gateway")

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
  SessionExport getSession(QString IP);
  QStringList getIPs();

private:
  Controller * controller;
  QDBusConnection bus;

  bool registerService();
  bool registerObject();
};

#endif // CONTROLLERDBUSADAPTOR_H
