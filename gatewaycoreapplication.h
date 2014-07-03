#ifndef GATEWAYAPPLICATION_H
#define GATEWAYAPPLICATION_H

#include <QObject>
#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>

class GatewayCoreApplication : public QCoreApplication
{
  Q_OBJECT
public:
  explicit GatewayCoreApplication(int& argc, char ** argv);
};

#endif // GATEWAYAPPLICATION_H
