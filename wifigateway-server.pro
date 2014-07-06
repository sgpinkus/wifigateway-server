######################################################################
# Automatically generated by qmake (2.01a) Sun Jun 29 08:54:31 2014
######################################################################

CONFIG += ordered
TEMPLATE = subdirs

# Directories
SUBDIRS += filewatcher \
  runcommand \
  gateway.pro

prefix=/usr/local/
exec_prefix=$$prefix
bindir=$${exec_prefix}/bin

target.path = $$bindir
target.files += wifigateway-server
scripts.path = $${bindir}/wifigateway-script/
scripts.files += wifigateway-script/*.sh
scripts.files += wifigateway-script/*.php
etc.path = /etc/
etc.files = conf/*
INSTALLS += target scripts etc
message(Script files: $${scripts.files})
message(Etc files: $${etc.files})

test.target = test
test.commands += make -C runcommand/test/ check;
test.commands += make -C filewatcher/test/ check;
test.commands += make -C test/ check;
test.commands += run-parts wifigateway-script/test/ --regex=".*testscript.*"
test.depends = .test_dummy
QMAKE_EXTRA_TARGETS += test .test_dummy
