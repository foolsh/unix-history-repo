#	$Id: NeXT.3.x,v 8.19 2002/03/21 23:59:25 gshapiro Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

PUSHDIVERT(1)
# NEXTSTEP 3.1 and 3.2 only support m68k and i386
#ARCH=  -arch m68k -arch i386 -arch hppa -arch sparc
#ARCH=  -arch m68k -arch i386
#ARCH=   ${RC_CFLAGS}
# For new sendmail Makefile structure, this must go in the ENVDEF and LDOPTS
POPDIVERT
define(`confSM_OS_HEADER', `sm_os_next')
define(`confCCOPTS', `-posix')
define(`confMAPDEF', `-DNDBM -DNIS -DNETINFO')
define(`confENVDEF', `-DNeXT -Wno-precomp -pipe ${RC_CFLAGS}')
define(`confLDOPTS', `${RC_CFLAGS} -posix')
define(`confLIBS', `-ldbm')
define(`confINSTALL_RAWMAN')
define(`confMANROOT', `/usr/man/cat')
define(`confMANROOTMAN', `/usr/man/man')
define(`confMANOWN', `root')
define(`confMANGRP', `wheel')
define(`confUBINOWN', `root')
define(`confUBINGRP', `wheel')
define(`confSBINOWN',  `root')
define(`confSBINGRP',  `wheel')
define(`confEBINDIR', `/usr/etc')
define(`confMBINDIR', `/usr/lib')
define(`confSBINDIR', `/usr/etc')
define(`confUBINDIR', `/usr/ucb')
define(`confINSTALL', `${BUILDBIN}/install.sh')
define(`confRANLIBOPTS', `-c')
