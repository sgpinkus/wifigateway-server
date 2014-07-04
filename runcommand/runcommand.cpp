#include "runcommand.h"

/**
 * Provides simple wrappers on QProcess to allow arbitrary shell commands (/bin/bash) to be run.
 * Also allows calls to be asynchronous, emitting the finished(int,QString) signal when done.
 * For async calls exit status cannot be determined at time of call. If there was an error setting up the async call -1 will be returned else 0 is returned.
 * For async calls the return value provided by the *signal* is always the exit status of the shell command or -1 if the call timed out.
 * The QTimer is necessary because start ordinarily start()ed calls don't timeout - apparently.
 */
RunCommand::RunCommand(QObject * parent) : QProcess(parent)
{
  timer.setSingleShot(true);
}


/**
 * Internal callback for runCommand() completion.
 * Disconnection should drop pending signals.
 * @emits exitStatus or -1 if call did not complete in timeout.
 */
void RunCommand::finishedInternal(int result)
{
  disconnect(this, SIGNAL(finished(int)), this, SLOT(finishedInternal(int)));
  disconnect(&timer, SIGNAL(timeout()), this, SLOT(finishedInternalTimeout()));
  timer.stop();
  this->kill();
  this->waitForFinished();
  QString output;
  output = this->readAll();
  emit finished(result, output);
}




/**
 * Internal callback for runCommandExec() completion.
 * Disconnection should drop pending signals.
 * @emits exitStatus or -1 if call did not complete in timeout.
 */
void RunCommand::finishedInternalExec(int result)
{
  disconnect(this, SIGNAL(finished(int)), this, SLOT(finishedInternalExec(int)));
  disconnect(&timer, SIGNAL(timeout()), this, SLOT(finishedInternalExecTimeout()));
  timer.stop();
  this->kill();
  this->waitForFinished();
  el.exit(result);
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
int RunCommand::runCommand(QString cmd, int timeout)
{
  int retval = -1;
  QStringList args;

  if((this->state() != QProcess::Running) && (this->state() != QProcess::Starting))
  {
    retval = 0;
    args << "-c";
    args << cmd;

    this->start("/bin/bash", args);
    timer.start(timeout);
    connect(&timer, SIGNAL(timeout()), SLOT(finishedInternalTimeout()));
    connect(this, SIGNAL(finished(int)), this, SLOT(finishedInternal(int)));
  }
  else {
    qDebug() << "QPROCESS running!!??";
  }
  return retval;
}


/**
 * The make event loop work while blocking version of runCommandWait().
 * That is, the same semantics as event loop exec() call.
 * finishedInternalExec() handles returning value from exec() to here.
 * @returns exitStatus or -1 on timeout or failure.
 */
int RunCommand::runCommandExec(QString cmd, QString& strBuf, int timeout)
{
  int retval = -1;
  QStringList args;

  if((this->state() != QProcess::Running) && (this->state() != QProcess::Starting))
  {
    retval = 0;
    args << "-c";
    args << cmd;
    this->start("/bin/bash", args);
    timer.start(timeout);
    connect(&timer, SIGNAL(timeout()), SLOT(finishedInternalExecTimeout()));
    connect(this, SIGNAL(finished(int)), this, SLOT(finishedInternalExec(int)));
    retval = el.exec();
    strBuf = this->readAll();
  }
  return retval;
}


/**
 * Basically append "/bin/bash -c " to what ever is passed in and call execCommand().
 * @returns exitStatus or -1 on timeout or failure.
 */
int RunCommand::runCommandWait(QString cmd, QString& strBuf, int timeout)
{
  QProcess p;
  int retval = -1;
  bool completed = false;
  QStringList args;

  args << "-c";
  args << cmd;
  p.start("/bin/bash", args);

  completed = p.waitForFinished(timeout);
  if(completed)
  {
    retval = p.exitCode();
    strBuf = p.readAll();
  }
  else
  {
    retval = -1;
  }
  return retval;
}
