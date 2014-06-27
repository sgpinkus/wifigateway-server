#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QTimer>
#include "runcommand/runcommand.h"

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(QString IP, QObject *parent = 0);

};

#endif // SESSION_H
