#	$Id: NCR.MP-RAS.3.x,v 8.20 2002/03/21 23:59:25 gshapiro Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

define(`confMAPDEF', `-DNDBM -DMAP_REGEX')
define(`confENVDEF', `-DNCR_MP_RAS3')
define(`confOPTIMIZE', `-O2')
define(`confLIBS', `-lsocket -lnsl -lelf -lc89')
define(`confMBINDIR', `/usr/ucblib')
define(`confSBINDIR', `/usr/ucbetc')
define(`confUBINDIR', `/usr/ucb')
define(`confEBINDIR', `/usr/ucblib')
define(`confSTDIR', `/var/ucblib')
define(`confINSTALL', `/usr/ucb/install')
define(`confDEPEND_TYPE', `NCR')
