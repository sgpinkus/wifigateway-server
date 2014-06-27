#!/bin/bash
# 
# If to iptables.
#

source gw_functions.sh
FIFO=/tmp/fifo

trap "gw_restore" EXIT
rm $FIFO
mkfifo $FIFO -m 0777

function go()
{
  while true
  do
    read cmd 0<$FIFO
    do_cmd $cmd
  done
}

function do_cmd()
{
  echo $1, $2, $3, $4
  case $1 in
    "add_host" )
      echo add_host $2
      add_host $2
    ;;
    "remove_host" )
      echo remove_host $2
      remove_host $2
    ;;
      * )
      echo "Unknown cmd"
    ;;
  esac
}

gw_init
go


