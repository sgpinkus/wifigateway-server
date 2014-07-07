Overview
========
Provides a daemon that controls Internet access for peers connecting to an AP. Wifi peers connecting to the access point are redirected to a web page where they sign on and are given access to the Internet. You still need an Internet connection, WiFi card, host Wifi AP server/implementation, and DHCP server.

Installation
============
The gateway consists of two components:
  
 * The backend that manages client sessions.
 * The frontend which the client uses to request access and monitor a session.

This gateway has some additional software deps, since its not packaged you must ensure this software is installed. Basically this system does not care how a client gets its IP address but there needs to be a DHCP server and AP server on the same host.
  
    # Get and configure dependent S.W:
    apt-get install dnsmasq apache2 php5 php5-cli hostapd hostap-utils
    <enable dnsmasq to handle dhcp on wlan0 only>
    # You need to configure this host as an AP, you also need a compat wireless card.
    <configure hostapd>
    # Build C++ backend
    cd code/ && qmake && make
    sudo ./web_client_gateway &
    # Deploy the web script
    cp -a web/* $webroot/
    
