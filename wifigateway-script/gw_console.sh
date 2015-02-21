#!/bin/bash
cd `dirname $0`
./dbus_shell.php system com.epicmorsel.dbus com.epicmorsel.dbus.gateway /com/epicmorsel/dbus/gateway
