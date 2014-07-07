Overview
========
Provides a simple daemon that controls Internet access for peers connecting to an access point or LAN. Peers connecting to the access point are redirected to a web page where they sign on and are given access to the Internet. Intended to work with hostapd, and dnsmasq but does not configure those softwares for you.

The setup consists of two components:

 * The backend that manages client sessions.
 * The frontend which the client uses to request access, and monitor their session (wifigateway-web).

This repository is the backend.

Basically this system doesnt care how a client gets an IP address or authenticates, but there needs to be a DHCP server and AP server on the same host.

See http://www.thekelleys.org.uk/dnsmasq/doc.html, http://wireless.kernel.org/en/users/Documentation/hostapd.

Installation
============

    cd wifigateway-server && dpkg-buildbackage -b && cd -
    sudo gdebi wifigateway-server*.deb --option=APT::Install-Recommends=true
    # Now configure wifgateway interfaces, hostapd, and dnsmasq. See sample configs.

Config
======
See /etc/wifigateway/server.ini

Notes
=====
This kind of a toy. Theres definitely smarter more secure (/secure) solutions, like RADIUS.
