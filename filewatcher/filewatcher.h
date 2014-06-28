#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QtCore>
#include <QFileSystemWatcher>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Ui {
    class FileWatcher;
}

class FileWatcher : public QObject
{
  Q_OBJECT

public:

  explicit FileWatcher(QString path, QObject * parent = 0);
  ~FileWatcher();

private slots:

  void watchedFileChanged(QString path);
  void watchedDirChanged(QString path);
  void scan();
  bool beingWatched();
  bool isFileNewer();
  ino_t getINode();

signals:
  void newLine(QString line);

private:

  QFile watchedFile;
  QDir watchedDir;
  QFileSystemWatcher watcher;
  unsigned int currSize;
  ino_t inode;

};
#endif // FILEWATCHER_H
