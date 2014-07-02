#!/bin/bash

TESTNET=10.1.3.
PASS=0
FAIL=0

function fail() {
  echo "$@";
  exit 1;
}

function VERIFY() {
  [[ "$1" == "$2" ]] || fail "$1 != $2";
  PASS=$(($PASS+1))
}


./gw_init.sh "EXTIF=eth0 INTIF=eth0.1"
VERIFY 0 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35; do
  ./gw_add_host.sh ${TESTNET}${i}
done
VERIFY 4 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35 36; do
  ./gw_add_host.sh ${TESTNET}${i}
done
VERIFY 5 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 36 34 32; do
  ./gw_remove_host.sh ${TESTNET}${i}
done
VERIFY 2 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`
for i in 32 33 34 35 36; do
  ./gw_remove_host.sh ${TESTNET}${i}
done
VERIFY 0 `iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | wc -l`

echo "$(($PASS+$FAIL)) tests. PASSED=$PASS, FAILED=$FAIL"
