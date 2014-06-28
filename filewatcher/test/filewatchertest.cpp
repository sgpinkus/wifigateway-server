#include <QtTest/QtTest>
#include <QDebug>
#include "filewatchertest.h"
#include "filewatcher.h"

const char * FileWatcherTest::TEST_FILE = "/tmp/filewatcher-testdata";

void FileWatcherTest::setup() {
  linesReceived.clear();
}

void FileWatcherTest::initTestCase() {
  QFileInfo fInfo = (QString(__FILE__));
  QDir::setCurrent(fInfo.dir().path());
}

void FileWatcherTest::cleanupTestCase() {
  QFile dataFile(FileWatcherTest::TEST_FILE);
  //dataFile.remove();
}

void FileWatcherTest::testFileWatcher() {
  QFile dataFile(FileWatcherTest::TEST_FILE);
  dataFile.open(QIODevice::WriteOnly);
  FileWatcher fw(FileWatcherTest::TEST_FILE);
  QSignalSpy spy(&fw, SIGNAL(newLine(QString)));
  QEventLoop loop;
  QObject::connect(&fw, SIGNAL(newLine(QString)), this, SLOT(receiveLine(QString)));
  QObject::connect(&fw, SIGNAL(newLine(QString)), &loop, SLOT(quit()));

  dataFile.write("This is a line yo!\n");
  dataFile.write("I ate 11 crumpets\n");
  dataFile.write("I ate 11 crumpets\n");
  dataFile.write("I ate 11 crumpets\n");
  dataFile.write("This is a line yo!\n");
  dataFile.write("This is a line yo!\n");
  dataFile.write("This is a line yo!\n");
  dataFile.flush();
  loop.exec();
  QCOMPARE(spy.count(), 7);
  QCOMPARE(linesReceived[0], QString("This is a line yo!"));
  QCOMPARE(linesReceived[3], QString("I ate 11 crumpets"));

  dataFile.close();
  dataFile.open(QIODevice::WriteOnly);
  dataFile.write("This is a line yo!\n");
  dataFile.flush();
  loop.exec();

  QCOMPARE(spy.count(), 8);
  QCOMPARE(linesReceived[7], QString("This is a line yo!"));
}

void FileWatcherTest::receiveLine(QString line) {
  linesReceived += line;
}

QTEST_MAIN(FileWatcherTest)
