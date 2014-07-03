# Project Plan
A first phase of development is planned. Later phases will add; centralized user management, login, user accounts, credit/bw limit options, a payment GW option.

First phase of implementation is as follows:

 1. Purchase a compat wireless card and configure hostapd or similar, and dhcpd.
  1.1. Alt: use eth0 as the controlled lan and wlan0 as the gateway - simple.
 2. Implement the basic IP tables rules to block all, except a redirect to host:80, and allow from a db of IP addrs.
 3. At this point the user must do something to enable access. Implement the dummy action of entering a non zero user name and checking an I agree check box.
 4. Register the users MAC and allow access.
 5. Implement javascript to incrementally update credential via a dummy ping to an end point and the corresponding timeout and refresh of IPTables rule.
