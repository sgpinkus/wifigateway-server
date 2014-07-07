#!/bin/bash
# Simple wrapper on iptables to allow a filtered set of hosts on a LAN access to a gateway.
# @todo Don't blow away rules and dont set a firewall.
#

IPTABLES=/sbin/iptables
INBOUND_TCP_ALLOW="ssh,http,https,5900,5901,domain,111,2049"
INBOUND_UDP_ALLOW="domain,111,2049,bootpc,bootps"
DEFAULT_BANDWIDTH=100  # Packets/s
DEFAULT_QUOTA=500
QUOTA_LOG_PREFIX="GW_SESSION_1MB_HACK"
EXTIF=
INTIF=
NETWORK=
NETMASK=


function error()
{
  echo "ERROR: $1"
  exit 1;
}


# Surely there is a "make everything f off!" command! Yeah `reboot`.
# Dont use this.
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


# Reset ip-tables. Don't use this.
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


function gw_reset_iptables()
{
  gw_restore
  gw_set_iptables_default_policy
}


function gw_check_mod()
{
  modprobe ip_tables || error "No iptables"
  modprobe iptable_nat || error "No iptables nat not loaded"
  modprobe ip_conntrack || error "No iptables conntrack loaded"
}


# Basic firewall for the gateway server itself. Nothing to do with actual gateway.
# Dont use this.
function gw_set_iptables_input_firewall()
{
  iptables -t filter -N gw_firewall
  # Firewall
  iptables -A gw_firewall -m comment --comment "gw firewall"
  # We can talk to ourselves freely.
  # Allow all outgoing connections we start.
  # Allowing ssh, http, https, vnc - youll have to come and edit this as your firewall rules!
  # TODO: allow ICMP from me to me
  iptables -A gw_firewall -i lo -j ACCEPT
  iptables -A gw_firewall -p icmp -j ACCEPT
  iptables -A gw_firewall -m state --state RELATED,ESTABLISHED -j ACCEPT
  # iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 22 -j ACCEPT
  [[ -n ${INBOUND_TCP_ALLOW} ]] && iptables -A gw_firewall -p tcp -m multiport --destination-port ${INBOUND_TCP_ALLOW} -j ACCEPT
  [[ -n ${INBOUND_UDP_ALLOW} ]] && iptables -A gw_firewall -p udp -m multiport --destination-port ${INBOUND_UDP_ALLOW} -j ACCEPT
  # NFS! What port will satisfy you?!
  #iptables -A gw_firewall --src ${NETWORK}/${NETMASK} -j ACCEPT
  iptables -A gw_firewall -j DROP
  iptables -A INPUT -j gw_firewall
}


# Init iptables for this app.
# Uses `mark` to mark, though connmark would be better, could not make it work together with marks I need for quota and bandwidth.
# Instead of per client quota for now doing one hash limit. Annoying I cant just DROP packets in PREROUTING. Marking is ugly.
# Alowed and disallowed traffic have same access to local by default.
# Note mangle->PREROUTING -j ACCEPT means packets dont hit mangle->FORWARD! nat->PREROUTING -j ACCEPT does not have same behaviour.
# @input EXTIF The interface connected to the outside
# @input INTIF The interface clients are on.
function gw_init()
{
  [[ -n "$@" ]] && export $@
  [[ -n "${EXTIF}" ]] || error "No external interface defined"
  [[ -n "${INTIF}" ]] || error "No internal interface defined"
  gw_reset_iptables
  # INPUT F.W.
  gw_set_iptables_input_firewall
  # Init chains used to implement gateway.
  iptables -t nat -N gw_nat_prerouting
  iptables -t filter -N gw_filter_forward
  iptables -t nat -N gw_nat_postrouting

  iptables -t mangle -N gw_hosts_allowed
  iptables -t filter -N gw_hosts_bandwidth
  iptables -t filter -N gw_hosts_quota
  # PREROUTING
  # gw_hosts_allowed marks allowed packets. This has to be done in pre routing(?).
  # Allow marked packets through, redirect unmarked packets on DNS, or HTTP(S) to this host, drop everything else.
  iptables -t mangle -I PREROUTING -j gw_hosts_allowed
  iptables -t nat -I PREROUTING -j gw_nat_prerouting
  iptables -t nat -A gw_nat_prerouting -p udp --dport 53 -m mark ! --mark 1/1 -j LOG
  iptables -t nat -A gw_nat_prerouting -p udp --dport 53 -m mark ! --mark 1/1 -j REDIRECT
  iptables -t nat -A gw_nat_prerouting -p tcp -m multiport --dports 80,443 -m mark ! --mark 1/1 -j REDIRECT
  # FORWARD
  # Pipe the packets that got here through the bw and quota chains. These update on a per host basis.
  iptables -t filter -I FORWARD -j gw_filter_forward
  iptables -t filter -A gw_filter_forward -j gw_hosts_bandwidth
  iptables -t filter -A gw_filter_forward -j gw_hosts_quota
  iptables -t filter -A gw_filter_forward -m mark --mark 3 -j ACCEPT # i.e. binary 3: marks 1 (allowed) and 2 (less than limit).
  iptables -t filter -A gw_filter_forward -i ${EXTIF} -o ${INTIF} -m state --state ESTABLISHED,RELATED -j ACCEPT
  # POSTROUTING - NAT.
  iptables -t nat -I POSTROUTING -j gw_nat_postrouting
  iptables -t nat -A gw_nat_postrouting -o ${EXTIF} -j MASQUERADE
  #iptables -t filter -N gw_hosts_quota
}


# Flush then delete all our chains. Order ~important.
function gw_clean_up()
{
  iptables -t mangle -D PREROUTING -j gw_hosts_allowed
  iptables -t nat -D PREROUTING -j gw_nat_prerouting
  iptables -t filter -D FORWARD -j gw_filter_forward
  iptables -t nat -D POSTROUTING -j gw_nat_postrouting
  iptables -t mangle -F gw_hosts_allowed
  iptables -t nat -F gw_nat_prerouting
  iptables -t filter -F gw_filter_forward
  iptables -t nat -F gw_nat_postrouting
  iptables -t mangle -X gw_hosts_allowed
  iptables -t nat -X gw_nat_prerouting
  iptables -t filter -X gw_filter_forward
  iptables -t nat -X gw_nat_postrouting
  iptables -t filter -D INPUT -j gw_firewall 2>/dev/null
  iptables -t filter -F gw_firewall 2>/dev/null
  iptables -t filter -X gw_firewall 2>/dev/null
}


# Add allow,bw,quota rules.
# Have to use marking due to limits of iptables rules.
# Quota mech is innaccurate because it uses packets size. Quota limit is managed externally.
# @input $1 ip address
# @input $2 default bandwidth limit. Optional
# @input $3 default quota. Optional Currently not implemented.
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
    iptables -t mangle -A gw_hosts_allowed -s "$1" -j MARK --set-mark 1/1
    iptables -t filter -A gw_hosts_bandwidth -s "$1" -m limit --limit "$bw" --limit-burst 10 -j MARK --set-mark 2/2
    iptables -t filter -A gw_hosts_quota -s "$1" -m statistic --mode nth --every 670 --packet 0 -j LOG --log-prefix "${QUOTA_LOG_PREFIX} " --log-level "info"
  fi
}


# Remove host by ip address.
#
function gw_remove_host()
{
  [[ -n "$1" ]] || error "Invalid parameters"
  echo "$1" | egrep -o -q "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}" || error "Invalid address"

  index=`gw_get_host_entry_num "$1"`
  if [[ -n $index ]]
  then
    iptables -t mangle -D gw_hosts_allowed $index
    iptables -t filter -D gw_hosts_bandwidth $index
    iptables -t filter -D gw_hosts_quota $index
  fi
}


# Get host entry by ip address.
#
function gw_get_host_entry()
{
  iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | grep "$1" -n -m 1 | cut -f1 -d":"
}


# Get host index by ip address.
#
function gw_get_host_entry_num()
{
  iptables-save -t mangle | grep "^\-A gw_hosts_allowed" | grep "$1" -n -m 1 | cut -f1 -d":"
}


function gw_dump_hosts_allowed()
{
  iptables-save -t mangle | grep "^\-A gw_hosts_allowed"
}
