#ifndef GATEWAY_H
#define GATEWAY_H

#include <QObject>
#include <QString>

/**
 * Simple client session orientated wrapper on IPTables.
 * IPTables is not used to store state in general.
 * Instead this class broadcasts session related events for something else to handle.
 * Client are identified by MAC.
 */
class GateWay : public QObject
{
    Q_OBJECT
public:
    explicit GateWay(QObject *parent = 0);
    //bool add_host(QString mac);
    //bool remove_host(QString mac);

signals:

public slots:

};

#endif // GATEWAY_H
