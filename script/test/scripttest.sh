#!/bin/bash

TESTNET=10.1.3.
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

./gw_init.sh "EXTIF=eth0 INTIF=eth0.1"
COMPARE 0 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35; do
  ./gw_add_host.sh ${TESTNET}${i}
done
COMPARE 4 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35 36; do
  ./gw_add_host.sh ${TESTNET}${i}
done
COMPARE 5 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 36 34 32; do
  ./gw_remove_host.sh ${TESTNET}${i}
done
COMPARE 2 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35 36; do
  ./gw_remove_host.sh ${TESTNET}${i}
done
COMPARE 0 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`

report
