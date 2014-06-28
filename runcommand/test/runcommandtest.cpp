#include <QtTest/QtTest>
#include <QDebug>
#include <unistd.h>
#include "runcommandtest.h"
#include "runcommand.h"


void RunCommandTest::setup() {
  lastOut = "";
  lastRetval = -2;
}

void RunCommandTest::testRunCommandCommon_data() {
  QTest::addColumn<QString>("command");
  QTest::addColumn<int>("retval");
  QTest::addColumn<QString>("output");

  QTest::newRow("true") << "true" << 0 << "";
  QTest::newRow("false") << "false" << 1 << "";
  QTest::newRow("cmd-that-dne") << "cmd-that-dne" << 127 << "";
  QTest::newRow("echo foo") << "echo foo" << 0 << "foo\n";
  QTest::newRow("ping -c 1 8.8.8.8") << "ping -c 1 8.8.8.8" << 0 << "";
}

void RunCommandTest::testRunCommandWait_data() {
  testRunCommandCommon_data();
}

void RunCommandTest::testRunCommandExec_data() {
  testRunCommandCommon_data();
}

void RunCommandTest::testRunCommand_data() {
  testRunCommandCommon_data();
}

void RunCommandTest::testRunCommandWait() {
  int ret;
  QString out;
  RunCommand r;
  QFETCH(QString, command);
  QFETCH(int, retval);
  QFETCH(QString, output);

  ret = r.runCommandWait(command, out, 5000);
  QCOMPARE(ret,retval);
  if(output.length()) {
    QCOMPARE(out,output);
  }
  //qDebug() << command << " " << retval;
}

void RunCommandTest::testRunCommandExec() {
  int ret;
  QString out;
  RunCommand r;
  QFETCH(QString, command);
  QFETCH(int, retval);
  QFETCH(QString, output);

  ret = r.runCommandWait(command, out, 5000);
  QCOMPARE(ret,retval);
  if(output.length()) {
    QCOMPARE(out,output);
  }
  //qDebug() << command << " " << retval;
}

/**
 * Test async call.
 */
void RunCommandTest::testRunCommand() {
  int ret;
  QString out;
  RunCommand r;
  QSignalSpy spy(&r, SIGNAL(finished(int,QString)));
  QFETCH(QString, command);
  QFETCH(int, retval);
  QFETCH(QString, output);
  QEventLoop loop;
  QObject::connect(&r, SIGNAL(finished(int,QString)), &loop, SLOT(quit()));
  QObject::connect(&r, SIGNAL(finished(int,QString)), this, SLOT(receiveOuput(int,QString)));
  ret = r.runCommand(command, 5000);
  QVERIFY(ret != -1);

  loop.exec();

  QCOMPARE(lastRetval,retval);
  if(output.length()) {
    QCOMPARE(lastOut,output);
  }
}

void RunCommandTest::receiveOuput(int retval, QString output) {
  lastOut = output;
  lastRetval = retval;
}

QTEST_MAIN(RunCommandTest)
