/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/i386/ibcs2/syscalls.master,v 1.17 2004/02/06 20:20:07 jhb Exp 
 */

#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <i386/ibcs2/ibcs2_types.h>
#include <i386/ibcs2/ibcs2_signal.h>
#include <i386/ibcs2/ibcs2_proto.h>

#define AS(name) (sizeof(struct name) / sizeof(register_t))

/* The casts are bogus but will do for now. */
struct sysent ibcs2_sysent[] = {
	{ 0, (sy_call_t *)nosys },			/* 0 = syscall */
	{ SYF_MPSAFE | AS(sys_exit_args), (sy_call_t *)sys_exit },	/* 1 = exit */
	{ SYF_MPSAFE | 0, (sy_call_t *)fork },		/* 2 = fork */
	{ AS(ibcs2_read_args), (sy_call_t *)ibcs2_read },	/* 3 = ibcs2_read */
	{ SYF_MPSAFE | AS(write_args), (sy_call_t *)write },	/* 4 = write */
	{ AS(ibcs2_open_args), (sy_call_t *)ibcs2_open },	/* 5 = ibcs2_open */
	{ SYF_MPSAFE | AS(close_args), (sy_call_t *)close },	/* 6 = close */
	{ AS(ibcs2_wait_args), (sy_call_t *)ibcs2_wait },	/* 7 = ibcs2_wait */
	{ AS(ibcs2_creat_args), (sy_call_t *)ibcs2_creat },	/* 8 = ibcs2_creat */
	{ AS(link_args), (sy_call_t *)link },		/* 9 = link */
	{ AS(ibcs2_unlink_args), (sy_call_t *)ibcs2_unlink },	/* 10 = ibcs2_unlink */
	{ AS(ibcs2_execv_args), (sy_call_t *)ibcs2_execv },	/* 11 = ibcs2_execv */
	{ AS(ibcs2_chdir_args), (sy_call_t *)ibcs2_chdir },	/* 12 = ibcs2_chdir */
	{ AS(ibcs2_time_args), (sy_call_t *)ibcs2_time },	/* 13 = ibcs2_time */
	{ AS(ibcs2_mknod_args), (sy_call_t *)ibcs2_mknod },	/* 14 = ibcs2_mknod */
	{ AS(ibcs2_chmod_args), (sy_call_t *)ibcs2_chmod },	/* 15 = ibcs2_chmod */
	{ AS(ibcs2_chown_args), (sy_call_t *)ibcs2_chown },	/* 16 = ibcs2_chown */
	{ AS(obreak_args), (sy_call_t *)obreak },	/* 17 = obreak */
	{ AS(ibcs2_stat_args), (sy_call_t *)ibcs2_stat },	/* 18 = ibcs2_stat */
	{ AS(ibcs2_lseek_args), (sy_call_t *)ibcs2_lseek },	/* 19 = ibcs2_lseek */
	{ SYF_MPSAFE | 0, (sy_call_t *)getpid },	/* 20 = getpid */
	{ AS(ibcs2_mount_args), (sy_call_t *)ibcs2_mount },	/* 21 = ibcs2_mount */
	{ AS(ibcs2_umount_args), (sy_call_t *)ibcs2_umount },	/* 22 = ibcs2_umount */
	{ SYF_MPSAFE | AS(ibcs2_setuid_args), (sy_call_t *)ibcs2_setuid },	/* 23 = ibcs2_setuid */
	{ SYF_MPSAFE | 0, (sy_call_t *)getuid },	/* 24 = getuid */
	{ AS(ibcs2_stime_args), (sy_call_t *)ibcs2_stime },	/* 25 = ibcs2_stime */
	{ SYF_MPSAFE | AS(ptrace_args), (sy_call_t *)ptrace },	/* 26 = ptrace */
	{ AS(ibcs2_alarm_args), (sy_call_t *)ibcs2_alarm },	/* 27 = ibcs2_alarm */
	{ AS(ibcs2_fstat_args), (sy_call_t *)ibcs2_fstat },	/* 28 = ibcs2_fstat */
	{ SYF_MPSAFE | 0, (sy_call_t *)ibcs2_pause },	/* 29 = ibcs2_pause */
	{ AS(ibcs2_utime_args), (sy_call_t *)ibcs2_utime },	/* 30 = ibcs2_utime */
	{ AS(ibcs2_stty_args), (sy_call_t *)ibcs2_stty },	/* 31 = ibcs2_stty */
	{ AS(ibcs2_gtty_args), (sy_call_t *)ibcs2_gtty },	/* 32 = ibcs2_gtty */
	{ AS(ibcs2_access_args), (sy_call_t *)ibcs2_access },	/* 33 = ibcs2_access */
	{ SYF_MPSAFE | AS(ibcs2_nice_args), (sy_call_t *)ibcs2_nice },	/* 34 = ibcs2_nice */
	{ AS(ibcs2_statfs_args), (sy_call_t *)ibcs2_statfs },	/* 35 = ibcs2_statfs */
	{ 0, (sy_call_t *)sync },			/* 36 = sync */
	{ SYF_MPSAFE | AS(ibcs2_kill_args), (sy_call_t *)ibcs2_kill },	/* 37 = ibcs2_kill */
	{ AS(ibcs2_fstatfs_args), (sy_call_t *)ibcs2_fstatfs },	/* 38 = ibcs2_fstatfs */
	{ SYF_MPSAFE | AS(ibcs2_pgrpsys_args), (sy_call_t *)ibcs2_pgrpsys },	/* 39 = ibcs2_pgrpsys */
	{ AS(ibcs2_xenix_args), (sy_call_t *)ibcs2_xenix },	/* 40 = ibcs2_xenix */
	{ SYF_MPSAFE | AS(dup_args), (sy_call_t *)dup },	/* 41 = dup */
	{ SYF_MPSAFE | 0, (sy_call_t *)pipe },		/* 42 = pipe */
	{ AS(ibcs2_times_args), (sy_call_t *)ibcs2_times },	/* 43 = ibcs2_times */
	{ SYF_MPSAFE | AS(profil_args), (sy_call_t *)profil },	/* 44 = profil */
	{ AS(ibcs2_plock_args), (sy_call_t *)ibcs2_plock },	/* 45 = ibcs2_plock */
	{ SYF_MPSAFE | AS(ibcs2_setgid_args), (sy_call_t *)ibcs2_setgid },	/* 46 = ibcs2_setgid */
	{ SYF_MPSAFE | 0, (sy_call_t *)getgid },	/* 47 = getgid */
	{ AS(ibcs2_sigsys_args), (sy_call_t *)ibcs2_sigsys },	/* 48 = ibcs2_sigsys */
	{ AS(ibcs2_msgsys_args), (sy_call_t *)ibcs2_msgsys },	/* 49 = ibcs2_msgsys */
	{ AS(ibcs2_sysi86_args), (sy_call_t *)ibcs2_sysi86 },	/* 50 = ibcs2_sysi86 */
	{ 0, (sy_call_t *)nosys },			/* 51 = ibcs2_acct */
	{ AS(ibcs2_shmsys_args), (sy_call_t *)ibcs2_shmsys },	/* 52 = ibcs2_shmsys */
	{ AS(ibcs2_semsys_args), (sy_call_t *)ibcs2_semsys },	/* 53 = ibcs2_semsys */
	{ AS(ibcs2_ioctl_args), (sy_call_t *)ibcs2_ioctl },	/* 54 = ibcs2_ioctl */
	{ AS(ibcs2_uadmin_args), (sy_call_t *)ibcs2_uadmin },	/* 55 = ibcs2_uadmin */
	{ 0, (sy_call_t *)nosys },			/* 56 = nosys */
	{ AS(ibcs2_utssys_args), (sy_call_t *)ibcs2_utssys },	/* 57 = ibcs2_utssys */
	{ AS(fsync_args), (sy_call_t *)fsync },		/* 58 = fsync */
	{ AS(ibcs2_execve_args), (sy_call_t *)ibcs2_execve },	/* 59 = ibcs2_execve */
	{ SYF_MPSAFE | AS(umask_args), (sy_call_t *)umask },	/* 60 = umask */
	{ AS(chroot_args), (sy_call_t *)chroot },	/* 61 = chroot */
	{ AS(ibcs2_fcntl_args), (sy_call_t *)ibcs2_fcntl },	/* 62 = ibcs2_fcntl */
	{ SYF_MPSAFE | AS(ibcs2_ulimit_args), (sy_call_t *)ibcs2_ulimit },	/* 63 = ibcs2_ulimit */
	{ 0, (sy_call_t *)nosys },			/* 64 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 65 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 66 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 67 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 68 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 69 = reserved for unix/pc */
	{ 0, (sy_call_t *)nosys },			/* 70 = obsolete rfs_advfs */
	{ 0, (sy_call_t *)nosys },			/* 71 = obsolete rfs_unadvfs */
	{ 0, (sy_call_t *)nosys },			/* 72 = obsolete rfs_rmount */
	{ 0, (sy_call_t *)nosys },			/* 73 = obsolete rfs_rumount */
	{ 0, (sy_call_t *)nosys },			/* 74 = obsolete rfs_rfstart */
	{ 0, (sy_call_t *)nosys },			/* 75 = obsolete rfs_sigret */
	{ 0, (sy_call_t *)nosys },			/* 76 = obsolete rfs_rdebug */
	{ 0, (sy_call_t *)nosys },			/* 77 = obsolete rfs_rfstop */
	{ 0, (sy_call_t *)nosys },			/* 78 = rfs_rfsys */
	{ AS(ibcs2_rmdir_args), (sy_call_t *)ibcs2_rmdir },	/* 79 = ibcs2_rmdir */
	{ AS(ibcs2_mkdir_args), (sy_call_t *)ibcs2_mkdir },	/* 80 = ibcs2_mkdir */
	{ AS(ibcs2_getdents_args), (sy_call_t *)ibcs2_getdents },	/* 81 = ibcs2_getdents */
	{ 0, (sy_call_t *)nosys },			/* 82 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 83 = nosys */
	{ AS(ibcs2_sysfs_args), (sy_call_t *)ibcs2_sysfs },	/* 84 = ibcs2_sysfs */
	{ AS(ibcs2_getmsg_args), (sy_call_t *)ibcs2_getmsg },	/* 85 = ibcs2_getmsg */
	{ AS(ibcs2_putmsg_args), (sy_call_t *)ibcs2_putmsg },	/* 86 = ibcs2_putmsg */
	{ AS(ibcs2_poll_args), (sy_call_t *)ibcs2_poll },	/* 87 = ibcs2_poll */
	{ 0, (sy_call_t *)nosys },			/* 88 = nosys */
	{ AS(ibcs2_secure_args), (sy_call_t *)ibcs2_secure },	/* 89 = ibcs2_secure */
	{ AS(ibcs2_symlink_args), (sy_call_t *)ibcs2_symlink },	/* 90 = ibcs2_symlink */
	{ AS(ibcs2_lstat_args), (sy_call_t *)ibcs2_lstat },	/* 91 = ibcs2_lstat */
	{ AS(ibcs2_readlink_args), (sy_call_t *)ibcs2_readlink },	/* 92 = ibcs2_readlink */
	{ 0, (sy_call_t *)nosys },			/* 93 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 94 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 95 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 96 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 97 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 98 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 99 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 100 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 101 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 102 = nosys */
	{ SYF_MPSAFE | AS(sigreturn_args), (sy_call_t *)sigreturn },	/* 103 = sigreturn */
	{ 0, (sy_call_t *)nosys },			/* 104 = nosys */
	{ 0, (sy_call_t *)ibcs2_isc },			/* 105 = ibcs2_isc */
	{ 0, (sy_call_t *)nosys },			/* 106 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 107 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 108 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 109 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 110 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 111 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 112 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 113 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 114 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 115 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 116 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 117 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 118 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 119 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 120 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 121 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 122 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 123 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 124 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 125 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 126 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 127 = nosys */
};
