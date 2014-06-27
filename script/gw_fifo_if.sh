#!/bin/bash
# Simple FIFO If to IPTables add/rem sessions.

source gw_functions.sh
FIFO=/tmp/gw_fifo

trap "gw_restore" EXIT
[[ -e $FIFO ]] && rm $FIFO
mkfifo $FIFO -m 0777

# While true read cmd from FIFO
function go()
{
  while true
  do
    read cmd 0<$FIFO
    do_cmd $cmd
  done
}

# Exec backend function which manip IPTables rules.
function do_cmd()
{
  #echo $1, $2, $3, $4
  case $1 in
    "add_host" )
      echo add_host $2 $3 $4
      gw_add_host $2 $3 $4
    ;;
    "remove_host" )
      echo remove_host $2
      gw_remove_host $2
    ;;
      * )
      echo "Unknown cmd"
    ;;
  esac
}

gw_init
go


