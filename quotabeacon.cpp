#include "quotabeacon.h"

/**
 * Turn this line in log:
 *
 *  "Jan  6 10:15:40 localhost kernel: [ 8847.438047] GW_SESSION_1MB_HACK IN=eth0 OUT=wlan0 SRC=192.168.2.150 DST=200.17.202.197 LEN=52 TOS=0x00 PREC=0x00 TTL=63 ID=30158 DF PROTO=TCP SPT=46518 DPT=80 WINDOW=770 RES=0x00 ACK URGP=0 MARK=0x3"
 *
 * into newLine(<src_ip>) signal.
 * @todo Not this.
 * @see Controller.
 */
QuotaBeacon::QuotaBeacon(QObject *parent) : QObject(parent)
{
  w = new FileWatcher(GW_QUOTA_LOG_FILE,this);
  r = new QRegExp(QString(GW_QUOTA_PREFIX) + ".*SRC=([^\\s]+)\\s.*");
  r->setMinimal( true );
  connect(w,SIGNAL(newLine(QString)),this,SLOT(newLine(QString)));
}


void QuotaBeacon::newLine(QString line)
{
  qDebug() << "In function " << __func__;
  QString ipMatch;
  r->indexIn(line);
  ipMatch = r->cap(1);
  qDebug() << line << ipMatch;
  if( ipMatch.length() > 0 )
  {
    emit quotaEvent(ipMatch);
  }
}
