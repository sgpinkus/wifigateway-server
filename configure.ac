AC_INIT(wifigateway-server, 0.1, sam@localhost)
AM_INIT_AUTOMAKE
AC_PROG_MAKE_SET
#AC_PROG_INSTALL
AC_CONFIG_FILES(files.h:files.h.in)
AC_CONFIG_FILES(conf/init.d/wifigatewaysrv:conf/init.d/wifigatewaysrv.in)
AC_CONFIG_COMMANDS([qmake],[qmake -makefile -nocache QMAKE_STRIP=: PREFIX=$prefix],[prefix=$prefix])
CWD=`eval echo \`pwd\``
AC_SUBST(CWD, "$CWD")
AC_OUTPUT
