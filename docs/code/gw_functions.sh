#!/bin/bash

# Simple wrapper on iptables to allow a filtered set of hosts on a LAN access to a gateway.
#

IPTABLES=/sbin/iptables
DB_FILE=/tmp/iptables-dump
INBOUND_TCP_ALLOW="ssh,http,https,5900,5901,domain,111,2049"
INBOUND_UDP_ALLOW="domain,111,2049,bootpc,bootps"
EXTIF="wlan0"
INTIF="eth0"
#APP=`basename $0`


function error()
{
  echo "An error occured: $1"
  exit 1;
}


# Surely there is a "make everything fuck off!" command!
#
function set_iptables_default_policy()
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


function  set_iptables_input_firewall()
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
  # Fucking NFS! What port will satisfy you?!
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
# Uses connmark to mark allowed over mark as its more efficient - mark once in nat.
# Instead of per client quota for now doing one hash limit.
# Very annoying that I cant just DROP packets in PREROUTING. Marking is ugly.
# Alllowed and disallowed traffic have same access to local.
#
function gw_init()
{
  gw_restore
  set_iptables_default_policy
  # INPUT F.W.
  set_iptables_input_firewall
  # PREROUTING
  iptables -t mangle -A PREROUTING -m connmark --mark 1 -j ACCEPT
  iptables -t nat -N hosts_allowed
  iptables -t nat -A PREROUTING -j hosts_allowed
  iptables -t nat -A PREROUTING -m connmark --mark 1 -j ACCEPT  # Check again.
  iptables -t nat -A PREROUTING -p tcp -m multiport --dports 80,443 -j REDIRECT --to-port 80
  iptables -t nat -A PREROUTING -j CONNMARK --set-mark 2 # Mark all else for later DROP.
  # FORWARD
  iptables -A FORWARD -m connmark --mark 1 -j ACCEPT # -i $INTIF -o $EXTIF -j ACCEPT
  iptables -A FORWARD -i $EXTIF -o $INTIF -m state --state ESTABLISHED,RELATED -j ACCEPT
  iptables -t nat -A POSTROUTING -o $EXTIF -j MASQUERADE
  update_hosts_db
  #iptables -t filter -N hosts_quota
}


# iptables -A hosts_allowed -j ACCEPT
# iptables -A hosts_allowed -s 192.168.2.0/24 -j ACCEPT
#
function add_host()
{
  [[ -n "$1" ]] || error "Invalid parameters"
  
  if [[ -z `get_host_entry "$1"` ]]
  then
    iptables -t nat -A hosts_allowed -s "$1" -j CONNMARK --set-mark 1
  fi
  update_hosts_db
}


function remove_host()
{
  [[ -n "$1" ]] || error "Invalid parameters"
  
  index=`get_host_entry_num "$1"`
  if [[ -n $index ]]
  then
    echo "Deleting $index"
    iptables -t nat -D hosts_allowed $index
  fi
  update_hosts_db
}  


function get_host_entry()
{
  grep "^\-A hosts_allowed" /tmp/iptables-dump | grep "$1" -n | cut -f1 -d":"
}


function get_host_entry_num()
{
  grep "^\-A hosts_allowed" /tmp/iptables-dump | grep "$1" -n | cut -f1 -d":"
}


function update_hosts_db()
{
  iptables-save > $DB_FILE
}
