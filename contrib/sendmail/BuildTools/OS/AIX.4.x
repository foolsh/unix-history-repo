#	@(#)AIX.4.x	8.8	(Berkeley)	8/6/1998
define(`confMAPDEF', `-DNDBM -DNIS -DMAP_REGEX')
define(`confENVDEF', `-D_AIX4 ')
define(`confOPTIMIZE', `-O3 -qstrict')
define(`confLIBS', `-ldbm')
define(`confSTDIR', `/etc')
define(`confHFDIR', `/usr/lib')
define(`confINSTALL', `/usr/ucb/install')
define(`confEBINDIR', `/usr/lib')
define(`confSBINGRP', `system')
