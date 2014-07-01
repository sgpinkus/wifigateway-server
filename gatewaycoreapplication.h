#ifndef GATEWAYAPPLICATION_H
#define GATEWAYAPPLICATION_H

#include <QCoreApplication>

class GatewayCoreApplication : public QCoreApplication
{
public:
  GatewayCoreApplication(int& argc, char ** argv);
};

#endif // GATEWAYAPPLICATION_H
