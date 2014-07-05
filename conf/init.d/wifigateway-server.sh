#!/bin/bash
### BEGIN INIT INFO
# Provides:          wifigatewaysrv
# Required-Start:    $remote_fs $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Wifi Gateway Server
### END INIT INFO

# Name has to be <= 15 char. Meh.
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/home/sam/Projects/wifigateway-server
DAEMON=/home/sam/Projects/wifigateway-server/wifigateway-server
NAME=wifigatewaysrv
#USER=root
DESC="Wifi Gateway Server"

. /lib/init/vars.sh
. /lib/lsb/init-functions

function _start()
{
	log_daemon_msg "Starting $DESC" "$NAME "
	echo start-stop-daemon --start --oknodo --background --exec $DAEMON -- $DAEMON_OPTS
	log_end_msg $?
}


function _stop()
{
	log_daemon_msg "Stopping $DESC" "$NAME "
	start-stop-daemon --stop --oknodo --name "$NAME" -- $DAEMON_OPTS
	log_end_msg $?
}


function _status()
{
	status_of_proc "$DAEMON" "$NAME" && exit 0 || exit $?
}


case "$1" in
start)
	_start
	;;
stop)
	_stop
	;;
restart)
	_stop && _start
	;;
status)
	_status
	;;
*)
	echo "Usage: $0 {start|stop|restart|status}"
	exit 1
esac
