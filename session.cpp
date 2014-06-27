#include "session.h"

Session::Session(QString IP, QObject *parent) : QObject(parent)
{
    state = Session::UNINITIALIZED;
    this->IP = IP;
    MAC = QString();
    updateTimer = new QTimer(this);
    activityTimer = new QTimer(this);
    endTimer = new QTimer(this);
    timeTimer = new QTimer(this);
    time = Session::DEFAULT_TIME;
    timeRemaining = time;
    activityTimeRemaining = Session::ACTIVITY_TIME;
    updateTimeRemaining = Session::UPDATE_TIME;
    endTimeRemaining = Session::END_TIME;
    quota = Session::DEFAULT_QUOTA;
    quotaRemaining = quota;
    bandwidth = Session::DEFAULT_BANDWIDTH;
    runcommand = new RunCommand(this);

    connect(timeTimer,SIGNAL(timeout()),this,SLOT(timeTimerTimeout()));
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTimeout()));
    connect(activityTimer,SIGNAL(timeout()),this,SLOT(activityTimerTimeout()));
    connect(endTimer,SIGNAL(timeout()),this,SLOT(endTimerTimeout()));

    connect(this,SIGNAL(terminate()),(Controller)parent,SLOT(terminate()));
}


int Session::start()
{
    QString buf;
    int success = -1;

    success = runcommand->runCommandExec( "gw_add_host.sh " + IP, &buf, 2 );

    if( success == 0 )
    {
        updateTimer->start(1000);
        activityTimer->start(1000);
        timeTimer->start(1000);
        updateTimeRemaining = Session::UPDATE_TIME;
        activityTimeRemaining = Session::ACTIVITY_TIME;
    }
    return success;
}


int Session::pause()
{
    QString buf;
    int success = -1;

    success = runcommand->runCommandExec( "gw_remove_host.sh " + IP, &buf, 2 );

    if( success == 0 )
    {
        updateTimer->stop();
        activityTimer->stop();
        timeTimer->stop();
    }
    return success;
}


int Session::play()
{
    QString buf;
    int success = -1;

    success = runcommand->runCommandExec( "gw_add_host.sh " + IP, &buf, 2 );

    if( success == 0 )
    {
        updateTimer->start(1000);
        activityTimer->start(1000);
        timeTimer->start(1000);
        updateTimeRemaining = Session::UPDATE_TIME;
        activityTimeRemaining = Session::ACTIVITY_TIME;
    }
    return success;
}


int Session::end()
{
    pause();
    endTimer->start();
    emit ended();
    return 0;
}


void Session::timeTimerTimeout()
{
    timeRemaining--;
    if( timeRemaining <= 0 )
    {
        end();
    }
    else if( timeRemaining == TIME_WARNING_TIME )
    {
        emit timeWarning(timeRemaining);
    }
}


void Session::updateTimerTimeout()
{
    updateTimeRemaining--;
    if( updateTimeRemaining <= 0 )
    {
        end();
    }

}


void Session::activityTimerTimeout()
{
    activityTimeRemaining--;
    if( activityTimeRemaining <= 0 )
    {
        end();
    }
    else if( activityTimeRemaining == ACTIVITY_WARNING_TIME )
    {
        emit activityWarning(activityTimeRemaining);
    }
}


void Session::endTimerTimeTimout()
{
    endTimeRemaining--;
    if( activityTimeRemaining <= 0 )
    {
       end();
       emit terminate();
    }
}
