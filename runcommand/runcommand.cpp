#include "runcommand.h"

/**
 * Provides simple wrappers on QProcess to allow arbitrary shell commands (/bin/bash) to be run.
 * Also allows calls to be asynchronous, emitting the finished(int,QString) signal when done.
 * For async calls exit status cannot be determined at time of call. If there was an error setting up the async call -1 will be returned else 0 is returned.
 * For async calls the return value provided by the *signal* is always the exit status of the shell command or -1 if the call timed out.
 * The QTimer is necessary because start ordinarily start()ed calls don't timeout - apparently.
 */
RunCommand::RunCommand( QObject * parent ) : QProcess(parent)
{
  e = new QEventLoop(this);
  t = new QTimer(this);
  t->setSingleShot(true);
  #ifdef RUNCOMMAND_TESTING
  testCnt = 0;
  #endif  
}


/**
 * Internal callback for runCommand() completion.
 * Disconnection should drop pending signals.
 * @emits exitStatus or -1 if call did not complete in timeout.
 */
void RunCommand::finishedInternal( int result )
{
  disconnect( this, SIGNAL( finished(int) ), this, SLOT( finishedInternal(int) ) );
  disconnect( t, SIGNAL( timeout() ), this, SLOT( finishedInternalTimeout() ) );
  t->stop();
  this->kill();
  this->waitForFinished();
  QString output;
  output = this->readAll();
  emit finished( result, output );
}




/**
 * Internal callback for runCommandExec() completion.
 * Disconnection should drop pending signals.
 * @emits exitStatus or -1 if call did not complete in timeout.
 */
void RunCommand::finishedInternalExec( int result )
{
  disconnect( this, SIGNAL( finished(int) ), this, SLOT( finishedInternalExec(int) ) );
  disconnect( t, SIGNAL( timeout() ), this, SLOT( finishedInternalExecTimeout() ) );
  t->stop();
  this->kill();
  this->waitForFinished();
  e->exit( result );
}


/**
 * For timeout. Introduces integer.
 */
void RunCommand::finishedInternalTimeout()
{
  finishedInternal(-1);
}


/**
 * For timeout. Introduces integer.
 */
void RunCommand::finishedInternalExecTimeout()
{
  finishedInternalExec(-1);
}


/**
 * Asynchronous version of runCommandWait().
 * @returns 0 if the async call was started successfully else -1.
 */
int RunCommand::runCommand( QString cmd, int timeout )
{
  int retval = -1;
  QStringList args;

  if( ( this->state() != QProcess::Running ) && ( this->state() != QProcess::Starting ) )
  {
    retval = 0;
    args << "-c";
    args << cmd;

    this->start( "/bin/bash", args );
    t->start(timeout);
    connect( t, SIGNAL(timeout()), SLOT(finishedInternalTimeout()) );
    connect( this, SIGNAL(finished(int)), this, SLOT(finishedInternal(int)) );
  }
  return retval;
}


/**
 * Make the damn event loop work while blocking version of runCommandWait().
 * finishedInternalExec() handles returning value from exec() here.
 * @returns exitStatus or -1 on timeout or failure.
 */
int RunCommand::runCommandExec( QString cmd, QString * strBuf, int timeout )
{
  int retval = -1;
  QStringList args;

  if( ( this->state() != QProcess::Running ) && ( this->state() != QProcess::Starting ) )
  {
    retval = 0;
    args << "-c";
    args << cmd;
    qDebug() << args;
    this->start( "/bin/bash", args );
    t->start(timeout);
    connect( t, SIGNAL(timeout()), SLOT(finishedInternalExecTimeout()) );
    connect( this, SIGNAL(finished(int)), this, SLOT(finishedInternalExec(int)) );
    retval = e->exec();
    *strBuf = this->readAll();
  }
  return retval;
}


/**
 * Basically append "/bin/bash -c " to what ever is passed in and call execCommand().
 * @returns exitStatus or -1 on timeout or failure.
 */
int RunCommand::runCommandWait( QString cmd, QString * strBuf, int timeout )
{
  QProcess p;
  int retval = -1;
  bool completed = false;
  QStringList args;

  args << "-c";
  args << cmd;
  p.start( "/bin/bash", args );

  completed = p.waitForFinished( timeout );
  if( completed )
  {
    retval = p.exitCode();
    *strBuf = p.readAll();
  }
  else
  {
    retval = -1;
  }
  return retval;
}


#ifdef RUNCOMMAND_TESTING
void RunCommand::test()
{
  QString output;
  int s;
  qDebug() << "### IN " __FILE__ << ":" << __FUNCTION__ << "\n";
  switch( testCnt )
   {
   case 0:
     qDebug( "#################################################" );
     qDebug( "Test runCommand()" );
     s = runCommand( "ping -c 2 8.8.8.8", 5000 );
     qDebug() << s;
     s = runCommand( "ping -c 2 8.8.8.8", 5000 );
     qDebug() << s;
     s = runCommand( "ping -c 2 8.8.8.8", 5000 );
     qDebug() << s;
     s = runCommand( "sleep 1", 5000 );
     qDebug() << s;
     s = runCommand( "sleep 1", 5000 );
     qDebug() << s;
     connect( this, SIGNAL(finished(int,QString)), this, SLOT(testCB(int,QString)));
     break;
   case 1:
     qDebug( "#################################################" );
     qDebug( "Test runCommand() again" );
     runCommand( "ping -c 2 8.8.8.8", 5000 );
     connect( this, SIGNAL(finished(int,QString)), this, SLOT(testCB(int,QString)));
     break;
   case 2:
     qDebug( "#################################################" );
     qDebug( "Test runCommadWait()" );
     s = runCommandWait( "ping -c 2 8.8.8.8", &output, 5000 );
     qDebug() <<  s << output;
     qDebug( "#################################################" );
     qDebug( "Test runCommandExec()" );
     s = runCommandExec( "ping -c 2 8.8.8.8", &output, 5000 );
     qDebug() << s << output;
     s = runCommandExec( "ping -c 2 8.8.8.8", &output, 5000 );
     qDebug() << s;
     qDebug() <<  s << output;
     qDebug( "#################################################" );
     qDebug( "Test runCommandExec() and timeout" );
     s = runCommandExec( "sleep 5", &output, 2000 );
     qDebug() << s;
     qDebug() <<  s << output;
     s = runCommandExec( "sleep 2", &output, 5000 );
     qDebug() << s;
     qDebug() <<  s << output;
     test2();
     exit(0);
  }
}

void RunCommand::test2()
{
  QString output;
  int s;
  qDebug() << "### IN " __FILE__ << ":" << __FUNCTION__ << "\n";
  s = runCommand( "ping -c 10 8.8.8.8", 10000 );
  qDebug() << s;
  while( state() != QProcess::NotRunning )
  {
    output = readLine(2);
    qDebug() << output;
  }
}

void RunCommand::testCB(int exitCode, QString output )
{
  qDebug() << "### IN " __FILE__ << ":" << __FUNCTION__ << "\n";
  qDebug() << exitCode << output;
  testCnt++;
  test();
}
#endif
