/*-
 * Copyright (c) 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley
 * by Pace Willisson (pace@blitz.com).  The Rock Ridge Extension
 * Support code is derived from software contributed to Berkeley
 * by Atsushi Murai (amurai@spec.co.jp).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)iso.h	8.2 (Berkeley) 1/23/94
 * $Id: iso.h,v 1.4 1995/01/16 17:03:29 joerg Exp $
 */

#define ISODCL(from, to) (to - from + 1)

struct iso_volume_descriptor {
	char type[ISODCL(1,1)]; /* 711 */
	char id[ISODCL(2,6)];
	char version[ISODCL(7,7)];
	char unused[ISODCL(8,8)];
	char type_sierra[ISODCL(9,9)]; /* 711 */
	char id_sierra[ISODCL(10,14)];
	char version_sierra[ISODCL(15,15)];
	char data[ISODCL(16,2048)];
};

/* volume descriptor types */
#define ISO_VD_PRIMARY 1
#define ISO_VD_END 255

#define ISO_STANDARD_ID "CD001"
#define ISO_ECMA_ID	"CDW01"

#define ISO_SIERRA_ID	"CDROM"

struct iso_primary_descriptor {
	char type			[ISODCL (  1,	1)]; /* 711 */
	char id				[ISODCL (  2,	6)];
	char version			[ISODCL (  7,	7)]; /* 711 */
	char unused1			[ISODCL (  8,	8)];
	char system_id			[ISODCL (  9,  40)]; /* achars */
	char volume_id			[ISODCL ( 41,  72)]; /* dchars */
	char unused2			[ISODCL ( 73,  80)];
	char volume_space_size		[ISODCL ( 81,  88)]; /* 733 */
	char unused3			[ISODCL ( 89, 120)];
	char volume_set_size		[ISODCL (121, 124)]; /* 723 */
	char volume_sequence_number	[ISODCL (125, 128)]; /* 723 */
	char logical_block_size		[ISODCL (129, 132)]; /* 723 */
	char path_table_size		[ISODCL (133, 140)]; /* 733 */
	char type_l_path_table		[ISODCL (141, 144)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (145, 148)]; /* 731 */
	char type_m_path_table		[ISODCL (149, 152)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (153, 156)]; /* 732 */
	char root_directory_record	[ISODCL (157, 190)]; /* 9.1 */
	char volume_set_id		[ISODCL (191, 318)]; /* dchars */
	char publisher_id		[ISODCL (319, 446)]; /* achars */
	char preparer_id		[ISODCL (447, 574)]; /* achars */
	char application_id		[ISODCL (575, 702)]; /* achars */
	char copyright_file_id		[ISODCL (703, 739)]; /* 7.5 dchars */
	char abstract_file_id		[ISODCL (740, 776)]; /* 7.5 dchars */
	char bibliographic_file_id	[ISODCL (777, 813)]; /* 7.5 dchars */
	char creation_date		[ISODCL (814, 830)]; /* 8.4.26.1 */
	char modification_date		[ISODCL (831, 847)]; /* 8.4.26.1 */
	char expiration_date		[ISODCL (848, 864)]; /* 8.4.26.1 */
	char effective_date		[ISODCL (865, 881)]; /* 8.4.26.1 */
	char file_structure_version	[ISODCL (882, 882)]; /* 711 */
	char unused4			[ISODCL (883, 883)];
	char application_data		[ISODCL (884, 1395)];
	char unused5			[ISODCL (1396, 2048)];
};
#define ISO_DEFAULT_BLOCK_SIZE		2048

struct iso_sierra_primary_descriptor {
	char unknown1			[ISODCL (  1,	8)]; /* 733 */
	char type			[ISODCL (  9,	9)]; /* 711 */
	char id				[ISODCL ( 10,  14)];
	char version			[ISODCL ( 15,  15)]; /* 711 */
	char unused1			[ISODCL ( 16,  16)];
	char system_id			[ISODCL ( 17,  48)]; /* achars */
	char volume_id			[ISODCL ( 49,  80)]; /* dchars */
	char unused2			[ISODCL ( 81,  88)];
	char volume_space_size		[ISODCL ( 89,  96)]; /* 733 */
	char unused3			[ISODCL ( 97, 128)];
	char volume_set_size		[ISODCL (129, 132)]; /* 723 */
	char volume_sequence_number	[ISODCL (133, 136)]; /* 723 */
	char logical_block_size		[ISODCL (137, 140)]; /* 723 */
	char path_table_size		[ISODCL (141, 148)]; /* 733 */
	char type_l_path_table		[ISODCL (149, 152)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (153, 156)]; /* 731 */
	char unknown2			[ISODCL (157, 160)]; /* 731 */
	char unknown3			[ISODCL (161, 164)]; /* 731 */
	char type_m_path_table		[ISODCL (165, 168)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (169, 172)]; /* 732 */
	char unknown4			[ISODCL (173, 176)]; /* 732 */
	char unknown5			[ISODCL (177, 180)]; /* 732 */
	char root_directory_record	[ISODCL (181, 214)]; /* 9.1 */
	char volume_set_id		[ISODCL (215, 342)]; /* dchars */
	char publisher_id		[ISODCL (343, 470)]; /* achars */
	char preparer_id		[ISODCL (471, 598)]; /* achars */
	char application_id		[ISODCL (599, 726)]; /* achars */
	char copyright_id		[ISODCL (727, 790)]; /* achars */
	char creation_date		[ISODCL (791, 806)]; /* ? */
	char modification_date		[ISODCL (807, 822)]; /* ? */
	char expiration_date		[ISODCL (823, 838)]; /* ? */
	char effective_date		[ISODCL (839, 854)]; /* ? */
	char file_structure_version	[ISODCL (855, 855)]; /* 711 */
	char unused4			[ISODCL (856, 2048)];
};

struct iso_directory_record {
	char length			[ISODCL (1, 1)]; /* 711 */
	char ext_attr_length		[ISODCL (2, 2)]; /* 711 */
	unsigned char extent		[ISODCL (3, 10)]; /* 733 */
	unsigned char size		[ISODCL (11, 18)]; /* 733 */
	char date			[ISODCL (19, 25)]; /* 7 by 711 */
	char flags			[ISODCL (26, 26)];
	char file_unit_size		[ISODCL (27, 27)]; /* 711 */
	char interleave			[ISODCL (28, 28)]; /* 711 */
	char volume_sequence_number	[ISODCL (29, 32)]; /* 723 */
	char name_len			[ISODCL (33, 33)]; /* 711 */
	char name			[0];
};
/* can't take sizeof(iso_directory_record), because of possible alignment
   of the last entry (34 instead of 33) */
#define ISO_DIRECTORY_RECORD_SIZE	33

struct iso_extended_attributes {
	unsigned char owner		[ISODCL (1, 4)]; /* 723 */
	unsigned char group		[ISODCL (5, 8)]; /* 723 */
	unsigned char perm		[ISODCL (9, 10)]; /* 9.5.3 */
	char ctime			[ISODCL (11, 27)]; /* 8.4.26.1 */
	char mtime			[ISODCL (28, 44)]; /* 8.4.26.1 */
	char xtime			[ISODCL (45, 61)]; /* 8.4.26.1 */
	char ftime			[ISODCL (62, 78)]; /* 8.4.26.1 */
	char recfmt			[ISODCL (79, 79)]; /* 711 */
	char recattr			[ISODCL (80, 80)]; /* 711 */
	unsigned char reclen		[ISODCL (81, 84)]; /* 723 */
	char system_id			[ISODCL (85, 116)]; /* achars */
	char system_use			[ISODCL (117, 180)];
	char version			[ISODCL (181, 181)]; /* 711 */
	char len_esc			[ISODCL (182, 182)]; /* 711 */
	char reserved			[ISODCL (183, 246)];
	unsigned char len_au		[ISODCL (247, 250)]; /* 723 */
};

/* CD-ROM Format type */
enum ISO_FTYPE	{ ISO_FTYPE_DEFAULT, ISO_FTYPE_9660, ISO_FTYPE_RRIP,
		  ISO_FTYPE_ECMA, ISO_FTYPE_HIGH_SIERRA };

#ifndef	ISOFSMNT_ROOT
#define	ISOFSMNT_ROOT	0
#endif

struct iso_mnt {
	int im_flags;

	struct mount *im_mountp;
	dev_t im_dev;
	struct vnode *im_devvp;

	int logical_block_size;
	int im_bshift;
	int im_bmask;

	int volume_space_size;
	char im_fsmnt[50];
	struct netexport im_export;

	char root[ISODCL (157, 190)];
	int root_extent;
	int root_size;
	enum ISO_FTYPE	iso_ftype;

	int rr_skip;
	int rr_skip0;
};

#define VFSTOISOFS(mp)	((struct iso_mnt *)((mp)->mnt_data))

#define iso_blkoff(imp, loc) ((loc) & (imp)->im_bmask)
#define iso_lblkno(imp, loc) ((loc) >> (imp)->im_bshift)
#define iso_blksize(imp, ip, lbn) ((imp)->logical_block_size)
#define iso_lblktosize(imp, blk) ((blk) << (imp)->im_bshift)
#define iso_lblktodaddr(imp, lbn) btodb(iso_lblktosize(imp, lbn))
#define iso_dblkinc(imp, lbn) ((lbn) + iso_lblktodaddr(imp, 1))
#define iso_dblkno(imp, loc) iso_lblktodaddr(imp, iso_lblkno(imp, loc))

int cd9660_mount __P((struct mount *,
	    char *, caddr_t, struct nameidata *, struct proc *));
int cd9660_start __P((struct mount *, int, struct proc *));
int cd9660_unmount __P((struct mount *, int, struct proc *));
int cd9660_root __P((struct mount *, struct vnode **));
int cd9660_quotactl __P((struct mount *, int, uid_t, caddr_t, struct proc *));
int cd9660_statfs __P((struct mount *, struct statfs *, struct proc *));
int cd9660_sync __P((struct mount *, int, struct ucred *, struct proc *));
int cd9660_vget __P((struct mount *, ino_t, struct vnode **));
int cd9660_fhtovp __P((struct mount *, struct fid *, struct mbuf *,
	    struct vnode **, int *, struct ucred **));
int cd9660_vptofh __P((struct vnode *, struct fid *));
int cd9660_init __P(());

struct iso_node;
int iso_blkatoff __P((struct iso_node *ip, long offset, struct buf **bpp));
int iso_iget __P((struct iso_node *xp, ino_t ino, int relocated,
		  struct iso_node **ipp, struct iso_directory_record *isodir));
int iso_iput __P((struct iso_node *ip));
int iso_ilock __P((struct iso_node *ip));
int iso_iunlock __P((struct iso_node *ip));
int cd9660_mountroot __P((void));

extern int (**cd9660_vnodeop_p)();

extern inline int
isonum_711(p)
	unsigned char *p;
{
	return *p;
}

extern inline int
isonum_712(p)
	char *p;
{
	return *p;
}

extern inline int
isonum_721(p)
	unsigned char *p;
{
	return *p|((char)p[1] << 8);
}

extern inline int
isonum_722(p)
	unsigned char *p;
{
	return ((char)*p << 8)|p[1];
}

extern inline int
isonum_723(p)
	unsigned char *p;
{
	return isonum_721(p);
}

extern inline int
isonum_731(p)
	unsigned char *p;
{
	return *p|(p[1] << 8)|(p[2] << 16)|(p[3] << 24);
}

extern inline int
isonum_732(p)
	unsigned char *p;
{
	return (*p << 24)|(p[1] << 16)|(p[2] << 8)|p[3];
}

extern inline int
isonum_733(p)
	unsigned char *p;
{
	return isonum_731(p);
}

int isofncmp __P((unsigned char *, int, unsigned char *, int));
void isofntrans __P((unsigned char *, int, unsigned char *, unsigned short *,
		     int, int));

/*
 * Associated files have a leading '='.
 */
#define	ASSOCCHAR	'='
