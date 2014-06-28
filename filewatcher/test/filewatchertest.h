#ifndef FILEWATCHERTEST_H
#define FILEWATCHERTEST_H

#include <QObject>

class FileWatcherTest: public QObject
{
    Q_OBJECT
public:
  static const char * TEST_FILE;

private slots:
  void initTestCase();
  void cleanupTestCase();
  void setup();
  void testFileWatcher();
  void receiveLine(QString line);

private:
  QList<QString> linesReceived;
};

#endif // FILEWATCHERTEST_H
