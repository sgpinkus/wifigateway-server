#!/bin/bash
DBUS_SERVICE="com.epicmorsel.dbus"
DBUS_INTERFACE="com.epicmorsel.dbus.gateway"
DBUS_OBJECT_PATH="/com/epicmorsel/dbus/gateway"
DBUS_BUS="--system"

function list() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.getIPs
}

function show() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.getSession string:"$1"
}

function new() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.newSession string:"$1"
}

function start() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.startSession string:"$1"
}

function end() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.endSession string:"$1"
}

function pause() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.pauseSession string:"$1"
}

function play() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  ${DBUS_INTERFACE}.playSession string:"$1" 
}

function check() {
  dbus-send --type=method_call --print-reply ${DBUS_BUS} --dest=${DBUS_SERVICE} \
  ${DBUS_OBJECT_PATH} \
  org.freedesktop.DBus.Introspectable.Introspect >/dev/null 2>&1
  if [[ $? -ne 0 ]]; then
    return 1
  fi
  return 0;
}


TESTNET=10.1.3
PASS=0
FAIL=0

function fail() {
  echo "$@";
  exit 1;
}

function report() {
  echo "$(($PASS+$FAIL)) tests. PASSED=$PASS, FAILED=$FAIL"
}

function COMPARE() {
  [[ "$1" != "$2" ]] && echo "$1 != $2" && FAIL=$(($FAIL+1)) && return;
  PASS=$(($PASS+1))
}

function GREP() {
  ! echo "$1" | grep -q "$2" && echo "$1 != $2" && FAIL=$(($FAIL+1)) && return;
  PASS=$(($PASS+1))
}

function VERIFY() {
if [[ -n $1 ]]; then
  PASS=$(($PASS+1))
else
  FAIL=$(($FAIL+1))
fi
}
check || fail "Could not connect to DBus service"
GREP "`new ${TESTNET}.32`" "int32 0"
new ${TESTNET}.32
new ${TESTNET}.32
show ${TESTNET}.32
start ${TESTNET}.32
show ${TESTNET}.32
start ${TESTNET}.32
start ${TESTNET}.32
start ${TESTNET}.33
show ${TESTNET}.33
list
report
