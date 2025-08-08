# Stupid Wifigateway Server
*This toy insecure captive-portal that I wrote as a learning experience. Dont use this.*

Provides a simple daemon that controls Internet access for peers connecting to an access point or LAN. Peers connecting to the access point are redirected to a web page where they must "sign on". Once signed on they are given access to the Internet for a session. Communication between the captive portal and this backend is over DBus.

 * The backend that manages portal client session state and sets up per client packet rules based on the session state.
 * The frontend allows client to request access, and monitor their session (wifigateway-web).

Main part of this repository is the backend. A sample frontend is also in the repo under [./web/](./wifigateway-web/).

Assumes WiFi access point with DHCP is configured on the same host (use [hostapd][hostapd] and [dnsmasq][dnsmasq]). Does not configure those softwares for you (some default configurations in docs). This system does not care how a client gets an IP address or authenticates onto the WLAN but assumes this is happens somehow.

# Installation

    git clone https://github.com/sam-at-github/wifigateway-server
    cd wifigateway-server && dpkg-buildbackage -b && cd -
    sudo gdebi wifigateway-server*.deb --option=APT::Install-Recommends=true
    # Now configure wifgateway interfaces, hostapd, and dnsmasq. See sample configs.

# Config
See /etc/wifigateway/server.ini

[dnsmasq]: http://www.thekelleys.org.uk/dnsmasq/doc.html
[hostapd]: http://wireless.kernel.org/en/users/Documentation/hostapd