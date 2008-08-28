#	$Id: HP-UX.11.x,v 8.26 2003/11/21 01:05:09 lijian Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

# +z is to generate position independant code
define(`confCClibsmi', `cc -Ae +Z')
define(`confCC', `cc -Ae')
define(`confMAPDEF', `-DNDBM -DNIS -DMAP_REGEX')
define(`confENVDEF', `-DV4FS -DHPUX11')
define(`confSM_OS_HEADER', `sm_os_hp')
define(`confOPTIMIZE',`+O2')
define(`confLIBS', `-ldbm -lnsl')
define(`confSHELL', `/usr/bin/sh')
define(`confINSTALL', `${BUILDBIN}/install.sh')
define(`confSBINGRP', `mail')
define(`confEBINDIR', `/usr/sbin')

define(`confMTCCOPTS', `-D_POSIX_C_SOURCE=199506L +z')
define(`confMTLDOPTS', `-lpthread')
define(`confLD', `ld')
define(`confLDOPTS_SO', `-b')
define(`confCCOPTS_SO', `')

dnl Don't indent or put any tab/space in this file.
dnl Tab/space here causes make syntax error 
ifelse(confBLDVARIANT, `DEBUG',
dnl Debug build
`
define(`confOPTIMIZE',`-g')
',
dnl Optimized build
confBLDVARIANT, `OPTIMIZED',
`
define(`confOPTIMIZE',`+O2')
',
dnl Purify build
confBLDVARIANT, `PURIFY',
`
define(`confOPTIMIZE',`-g')
',
dnl default
`
define(`confOPTIMIZE',`+O2')
')
