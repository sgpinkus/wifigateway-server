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
  int startSession(QString IP);
  int pauseSession(QString IP);
  int playSession(QString IP);
  int endSession(QString IP);
  int deleteSession(QString IP);
  int updateSession(QString IP);
  int setBandwidth(QString IP, qint32 bw);
  int setQuota(QString IP, qint32 quota);
  int setTime(QString IP, qint32 time);
  int setDefaultBandWidth(quint32 bw);
  int setDefaultQuota(qint32 quota);
  int setDefaultTime(qint32 time);
  SessionExport getDefaultSession();
  int setDefaultSession(const SessionExport& s);
  SessionExport getSession(QString IP);
  QStringList getIPs();

private:
  Controller * controller;
  QDBusConnection bus;

  bool registerService();
  bool registerObject();
};

#endif // CONTROLLERDBUSADAPTOR_H
