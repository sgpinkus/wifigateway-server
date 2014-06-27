#ifndef RUNCOMMAND_H
#define RUNCOMMAND_H

#include <QtCore>

//#define RUNCOMMAND_TESTING

class RunCommand : public QProcess
{
  Q_OBJECT

  public:
    explicit RunCommand( QObject * parent = 0 );
    int runCommand( QString cmd, int timeout = 30000 );
    int runCommandExec( QString cmd, QString * strBuf, int timeout = 30000 );
    int static runCommandWait( QString cmd, QString * strBuf, int timeout = 30000 );

  public slots:

  private slots:
    void finishedInternal( int exitCode );
    void finishedInternalExec( int exitCode );
    void finishedInternalTimeout();
    void finishedInternalExecTimeout();

  signals:
    void finished( int exitCode, QString output );

  private:
    QEventLoop * e;
    QTimer * t;
    QString * strBuf;
    
  #ifdef RUNCOMMAND_TESTING
  public slots:
    void test();
    void test2();
    void testCB(int exitCode, QString output );
  public:
    int testCnt;
  #endif   
};

#endif // RUNCOMMAND_H
