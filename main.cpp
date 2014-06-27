#include <QtCore/QCoreApplication>
#include <QtDBus>
#include <QtDebug>
#include <QDBusMetaType>
#include "controller.h"
#include "controllerdbusadaptor.h"
#include "sessionexport.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDBusConnection bus = QDBusConnection::connectToBus(QDBusConnection::SystemBus, DBUS_NAME );
    Controller * controller = new Controller();
    ControllerDBusAdaptor * adaptor = new ControllerDBusAdaptor(controller);

    // grab our name.
    while( ! bus.registerService(DBUS_NAME) )
    {
        qDebug() << "Registration of name failed";
        sleep(1);
    }
    // connect to D-Bus and register Object - not the adaptor.
    while( ! bus.registerObject(DBUS_OBJECT_PATH, controller) )
    {
        qDebug() << "Registration Failed";
        sleep(1);
    }

    qDebug() << "DBus connection established. BaseName=" << bus.baseService() << ", Name=" << bus.name();
    return a.exec();
}
