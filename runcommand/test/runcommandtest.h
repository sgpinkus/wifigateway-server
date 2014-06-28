#include <QObject>

class RunCommandTest: public QObject
{
  Q_OBJECT
private slots:
  void setup();
  void testRunCommandCommon_data();
  void testRunCommandWait_data();
  void testRunCommandExec_data();
  void testRunCommand_data();
  void testRunCommandWait();
  void testRunCommandExec();
  void testRunCommand();
  void receiveOuput(int retval, QString output);

private:
  QString lastOut;
  int lastRetval;
};
