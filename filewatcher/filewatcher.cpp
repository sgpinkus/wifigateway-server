#include "filewatcher.h"


/**
 * Watch one file for new lines and sned them to client. File is identified by its name not inode
 * QFileSystemWatcher is good, this class wraps it to give exactly what we want.
 * Need to handle things like move delete touch, and keep watching for new lines without producing duplicate lines.
 * This class doe this and  I.f.s to client via a newLine() signal.
 * Note if exactly the same thing is written (not appended) to the file twice it will not be detected.
 */
FileWatcher::FileWatcher( QString path, QObject * parent ) : QObject( parent )
{
  watchedFile = new QFile(path,this);
  watchedDir = new QDir(path);
  watchedDir->cdUp();
  watcher = new QFileSystemWatcher(this);
  watcher->addPath( watchedFile->fileName() );
  watcher->addPath( watchedDir->path() );
  currSize = watchedFile->size();
  // Using the inode like a file id.
  inode = getINode();
  QObject::connect( watcher, SIGNAL(directoryChanged(QString)), this, SLOT(watchedDirChanged(QString)) );
  QObject::connect( watcher, SIGNAL(fileChanged(QString)), this, SLOT(watchedFileChanged(QString)) );
}
\

FileWatcher::~FileWatcher()
{
    delete watchedDir;
}


/**
 * The file was modified renamed or deleted. Need to rewatch if renamed or deleted.
 * NOTE when Qt says "signaled when been modified, renamed or removed from disk",
 *  by "modified" they mean any metadata not m_time - see inotfiy IN_ALL_EVENTS.
 * Start scan from beginning if the file is newer than the one we were reading last time.
 */
void FileWatcher::watchedFileChanged( QString path )
{
  qDebug() << "In function " << __func__ << " " << currSize << " " << inode;

  if( isFileNewer() )
  {
    qDebug() << "FILE IS NEW";
    currSize = 0;
  }

  FileWatcher::scan();
}


/**
 * File E and not being watched => file is new ^ currSize is invalid, so;
 * currSize <- 0; watch; rescan;
 * This is needed since if the file is moved or deleted it is unwatched.
 */
void FileWatcher::watchedDirChanged( QString path )
{
  qDebug() << "In function " << __func__ << beingWatched() << watchedFile->exists();
  if( watchedFile->exists() && ! beingWatched() )
  {
    qDebug() << "File was not being watched. Watching";
    watcher->addPath( watchedFile->fileName() );
    currSize = 0;
    inode = getINode();
    FileWatcher::scan();
  }
  else
  {
    //the file ! E or we are already watching it.
  }
}


/**
 * Or rather file is different. ~ => newer.
 * Added size check too in case someone goes `>kern.log` - not perfect.
 * Note file is newer if file now DNE, thats OK.
 */
bool FileWatcher::isFileNewer()
{
  ino_t newINode = FileWatcher::getINode();
  size_t newSize = watchedFile->size();
  qDebug() << "In function " << __func__ << inode << newINode;

  if( ( newINode != inode ) || ( newSize < currSize ) )
  {
    return true;
  }
  return false;
}


ino_t FileWatcher::getINode()
{
  struct stat stat_buf;
  ino_t retval = 0;

  if( stat( watchedFile->fileName().toAscii(), &stat_buf ) != -1 )
  {
    retval = stat_buf.st_ino;
  }
  return retval;
}


/**
 * wrapper
 */
void FileWatcher::scan()
{
  qDebug() << "In function " << __func__;
  QString line;
  bool retval = false;

  if( watchedFile->open( QIODevice::ReadOnly ) )
  {
    watchedFile->seek( currSize );
    line = watchedFile->readLine(1024);

    while( line.size() > 0 )
    {
      line = line.trimmed();
      emit newLine(line);
      line = watchedFile->readLine();
    }
    currSize = watchedFile->pos();
  }
  watchedFile->close();
}


/**
 * Is watchedFile->fileName() being watched?
 */
bool FileWatcher::beingWatched()
{
  qDebug() << "In function " << __func__;
  QStringList paths = watcher->files();

  //foreach(QString p, paths){qDebug() << p;}

  if( paths.indexOf( watchedFile->fileName() ) != -1 )
  {
     return true;
  }
  return false;
}
