#ifndef QUOTABEACON_H
#define QUOTABEACON_H

#include <QObject>
#include "filewatcher/filewatcher.h"

#define GW_QUOTA_LOG_FILE "/var/log/kern.log"
#define GW_QUOTA_PREFIX "GW_SESSION_1MB_HACK"

class QuotaBeacon : public QObject
{
  Q_OBJECT
public:
  explicit QuotaBeacon(QObject *parent = 0);

signals:
  void quotaEvent(QString IP);

private slots:
  void newLine(QString line);

private:
  FileWatcher * w;
  QRegExp * r;


};

#endif // QUOTABEACON_H
