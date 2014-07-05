#!/bin/bash
# Used for testing.
cd `dirname $0`
source gw_functions.sh
gw_reset_iptables $*
