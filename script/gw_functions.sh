#!/bin/bash

# Simple wrapper on iptables to allow a filtered set of hosts on a LAN access to a gateway.
#

IPTABLES=/sbin/iptables
DB_FILE=/tmp/iptables-dump
INBOUND_TCP_ALLOW="ssh,http,https,5900,5901,domain,111,2049"
INBOUND_UDP_ALLOW="domain,111,2049,bootpc,bootps"
# Swap for a wifi GW and setup AP.
EXTIF="wlan0"
INTIF="eth0"
DEFAULT_BANDWIDTH=100  # Packets/s
DEFAULT_QUOTA=500
#APP=`basename $0`


function error()
{
  echo "An error occured: $1"
  exit 1;
}


# Surely there is a "make everything f off!" command!
function gw_set_iptables_default_policy()
{
  iptables -P INPUT ACCEPT
  iptables -F INPUT
  iptables -P OUTPUT ACCEPT
  iptables -F OUTPUT
  iptables -P FORWARD DROP
  iptables -F FORWARD
  iptables -t mangle -F
  iptables -t mangle -X
  iptables -t nat -F
  iptables -t nat -X
  iptables -t filter -F
  iptables -t filter -X
  iptables -t raw -F
  iptables -t raw -X
  # Reset all IPTABLES counters
  iptables -Z
}


# Basic firewall for the gateway server itself.
# Nothing to do with actual gateway.
function gw_set_iptables_input_firewall()
{
  iptables -P INPUT DROP 
  # Firewall 
  iptables -A INPUT -m comment --comment "firewall-iptables firewall rules"
  # We can talk to ourselves freely.
  # Allow all outgoing connections we start.
  # Allowing ssh, http, https, vnc - youll have to come and edit this as your firewall rules!
  # TODO: allow ICMP from me to me
  iptables -A INPUT -i lo -j ACCEPT
  iptables -A INPUT -p icmp -j ACCEPT 
  iptables -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
  # iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 22 -j ACCEPT
  [[ -n $INBOUND_TCP_ALLOW ]] && iptables -A INPUT -p tcp -m multiport --destination-port $INBOUND_TCP_ALLOW -j ACCEPT
  [[ -n $INBOUND_UDP_ALLOW ]] && iptables -A INPUT -p udp -m multiport --destination-port $INBOUND_UDP_ALLOW -j ACCEPT  
  # NFS! What port will satisfy you?!
  iptables -A INPUT --src 192.168.0.0/24 -j ACCEPT
}


function gw_restore()
{
  iptables -t mangle -F
  iptables -t mangle -X
  iptables -t nat -F
  iptables -t nat -X
  iptables -t filter -F
  iptables -t filter -X
  iptables -t raw -F
  iptables -t raw -X
}


# Init iptables for this app.
# Uses mark to mark, though connmark would be more could not make it work together with marks need for BW.
# Instead of per client quota for now doing one hash limit.
# Very annoying that I cant just DROP packets in PREROUTING. Marking is ugly.
# Alllowed and disallowed traffic have same access to local.
# mangle->PREROUTING -j ACCEPT means packets dont hit mangle->FORWARD! nat->PREROUTING -j ACCEPT does not have same behaviour! This is fucked up.
function gw_init()
{
  gw_restore
  gw_set_iptables_default_policy
  # INPUT F.W.
  gw_set_iptables_input_firewall
  iptables -t mangle -N hosts_allowed
  iptables -t filter -N hosts_bandwidth 
  iptables -t filter -N hosts_quota
  # PREROUTING
  iptables -t mangle -A PREROUTING -j hosts_allowed
  iptables -t nat -A PREROUTING -p udp --dport 53 -m mark ! --mark 1/1 -j LOG
  iptables -t nat -A PREROUTING -p udp --dport 53 -m mark ! --mark 1/1 -j REDIRECT
  iptables -t nat -A PREROUTING -p tcp -m multiport --dports 80,443 -m mark ! --mark 1/1 -j REDIRECT --to-port 80
  # FORWARD
  iptables -A FORWARD -j hosts_bandwidth
  iptables -A FORWARD -j hosts_quota
  iptables -A FORWARD -m mark --mark 3 -j ACCEPT # marks 1 (allowed) and 2 (less than limit).
  iptables -A FORWARD -i $EXTIF -o $INTIF -m state --state ESTABLISHED,RELATED -j ACCEPT
  iptables -t nat -A POSTROUTING -o $EXTIF -j MASQUERADE
  gw_update_hosts_db
  #iptables -t filter -N hosts_quota
}


# Add allow,bw,quota rules.
# Have to use marking due to limits of iptables rules.
# quota mech is extremely innaccurate. Actual quota managed externally.
# iptables -A hosts_allowed -j ACCEPT
# iptables -A hosts_allowed -s 192.168.2.0/24 -j ACCEPT
#
function gw_add_host()
{
  [[ -n "$1" ]] || error "Invalid parameters"
  echo "$1" | egrep -o -q "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}" || error "Invalid address"
  ip="$1"
  bw=${2:-$DEFAULT_BANDWIDTH}
  qo=${3:-$DEFAULT_QUOTA}
  
  if [[ -z `gw_get_host_entry "$1"` ]]
  then
    iptables -t mangle -A hosts_allowed -s "$1" -j MARK --set-mark 1/1
    iptables -t filter -A hosts_bandwidth -s "$1" -m limit --limit "$bw" --limit-burst 10 -j MARK --set-mark 2/2
    iptables -t filter -A hosts_quota -s "$1" -m statistic --mode nth --every 670 -j LOG --log-prefix "GW_SESSION_1MB_HACK " --log-level "info"
  fi
  gw_update_hosts_db
}


function gw_remove_host()
{
  [[ -n "$1" ]] || error "Invalid parameters"
  echo "$1" | egrep -o -q "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}" || error "Invalid address"
  
  index=`gw_get_host_entry_num "$1"`
  if [[ -n $index ]]
  then
    iptables -t mangle -D hosts_allowed $index
    iptables -t filter -D hosts_bandwidth $index
    iptables -t filter -D hosts_quota $index
  fi
  gw_update_hosts_db
}  


function gw_get_host_entry()
{
  grep "^\-A hosts_allowed" $DB_FILE | grep "$1" -n -m 1 | cut -f1 -d":"
}


function gw_get_host_entry_num()
{
  grep "^\-A hosts_allowed" $DB_FILE | grep "$1" -n -m 1 | cut -f1 -d":"
}


function gw_update_hosts_db()
{
  iptables-save -t mangle > $DB_FILE
}
