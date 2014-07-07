#
# Regular cron jobs for the wifigateway-server package
#
0 4	* * *	root	[ -x /usr/bin/wifigateway-server_maintenance ] && /usr/bin/wifigateway-server_maintenance
