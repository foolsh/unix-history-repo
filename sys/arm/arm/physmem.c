/*-
 * Copyright (c) 2014 Ian Lepore <ian@freebsd.org>
 * All rights excluded.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_ddb.h"

/*
 * Routines for describing and initializing anything related to physical memory.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <vm/vm.h>
#include <machine/physmem.h>

/*
 * These structures are used internally to keep track of regions of physical
 * ram, and regions within the physical ram that need to be excluded.  An
 * exclusion region can be excluded from crash dumps, from the vm pool of pages
 * that can be allocated, or both, depending on the exclusion flags associated
 * with the region.
 */
#define	MAX_HWCNT	10
#define	MAX_EXCNT	10

struct region {
	vm_offset_t	addr;
	vm_size_t	size;
	uint32_t	flags;
};

static struct region hwregions[MAX_HWCNT];
static struct region exregions[MAX_EXCNT];

static size_t hwcnt;
static size_t excnt;

/*
 * These "avail lists" are globals used to communicate physical memory layout to 
 * other parts of the kernel.  Within the arrays, each value is the starting
 * address of a contiguous area of physical address space.  The values at even
 * indexes are areas that contain usable memory and the values at odd indexes
 * are areas that aren't usable.  Each list is terminated by a pair of zero
 * entries.
 *
 * dump_avail tells the dump code what regions to include in a crash dump, and
 * phys_avail is the way we hand all the remaining physical ram we haven't used
 * in early kernel init over to the vm system for allocation management.
 *
 * We size these arrays to hold twice as many available regions as we allow for
 * hardware memory regions, to allow for the fact that exclusions can split a
 * hardware region into two or more available regions.  In the real world there
 * will typically be one or two hardware regions and two or three exclusions.
 *
 * Each available region in this list occupies two array slots (the start of the
 * available region and the start of the unavailable region that follows it).
 */
#define	MAX_AVAIL_REGIONS	(MAX_HWCNT * 2)
#define	MAX_AVAIL_ENTRIES	(MAX_AVAIL_REGIONS * 2)

vm_paddr_t phys_avail[MAX_AVAIL_ENTRIES + 2]; /* +2 to allow for a pair  */
vm_paddr_t dump_avail[MAX_AVAIL_ENTRIES + 2]; /* of zeroes to terminate. */

/* This is the total number of hardware pages, excluded or not. */
long realmem;

/* The address at which the kernel was loaded.  Set early in initarm(). */
vm_offset_t arm_physmem_kernaddr;

/*
 * Print the contents of the physical and excluded region tables using the
 * provided printf-like output function (which will be either printf or
 * db_printf).
 */
static void
physmem_dump_tables(int (*prfunc)(const char *, ...))
{
	int flags, i;
	uintmax_t addr, size;
	const unsigned int mbyte = 1024 * 1024;

	prfunc("Physical memory chunk(s):\n");
	for (i = 0; i < hwcnt; ++i) {
		addr = hwregions[i].addr;
		size = hwregions[i].size;
		prfunc("  0x%08jx - 0x%08jx, %5ju MB (%7ju pages)\n", addr,
		    addr + size - 1, size / mbyte, size / PAGE_SIZE);
	}

	prfunc("Excluded memory regions:\n");
	for (i = 0; i < excnt; ++i) {
		addr  = exregions[i].addr;
		size  = exregions[i].size;
		flags = exregions[i].flags;
		prfunc("  0x%08jx - 0x%08jx, %5ju MB (%7ju pages) %s %s\n",
		    addr, addr + size - 1, size / mbyte, size / PAGE_SIZE,
		    (flags & EXFLAG_NOALLOC) ? "NoAlloc" : "",
		    (flags & EXFLAG_NODUMP)  ? "NoDump" : "");
	}
}

/*
 * Print the contents of the static mapping table.  Used for bootverbose.
 */
void
arm_physmem_print_tables()
{

	physmem_dump_tables(printf);
}

/*
 * Walk the list of hardware regions, processing it against the list of
 * exclusions that contain the given exflags, and generating an "avail list".
 *
 * Updates the kernel global 'realmem' with the sum of all pages in hw regions.
 *
 * Returns the number of pages of non-excluded memory added to the avail list.
 */
static long
regions_to_avail(vm_paddr_t *avail, uint32_t exflags)
{
	size_t acnt, exi, hwi;
	vm_paddr_t end, start, xend, xstart;
	long availmem;
	const struct region *exp, *hwp;

	realmem = 0;
	availmem = 0;
	acnt = 0;
	for (hwi = 0, hwp = hwregions; hwi < hwcnt; ++hwi, ++hwp) {
		start = hwp->addr;
		end   = hwp->size + start;
		realmem += arm32_btop(end - start);
		for (exi = 0, exp = exregions; exi < excnt; ++exi, ++exp) {
			xstart = exp->addr;
			xend   = exp->size + xstart;
			/*
			 * If the excluded region ends before this hw region,
			 * continue checking with the next excluded region.
			 */
			if (xend <= start)
				continue;
			/*
			 * If the excluded region begins after this hw region
			 * we're done because both lists are sorted.
			 */
			if (xstart >= end)
				break;
			/*
			 * If the excluded region completely covers this hw
			 * region, shrink this hw region to zero size.
			 */
			if ((start >= xstart) && (end <= xend)) {
				start = xend;
				end = xend;
				break;
			}
			/*
			 * If the excluded region falls wholly within this hw
			 * region without abutting or overlapping the beginning
			 * or end, create an available entry from the leading
			 * fragment, then adjust the start of this hw region to
			 * the end of the excluded region, and continue checking
			 * the next excluded region because another exclusion
			 * could affect the remainder of this hw region.
			 */
			if ((xstart > start) && (xend < end)) {
				avail[acnt++] = start;
				avail[acnt++] = xstart;
				availmem += arm32_btop(xstart - start);
				start = xend;
				continue;
			}
			/*
			 * If excluded region partially overlaps this region,
			 * trim the excluded portion off the appropriate end.
			 */
			if ((xstart >= start) && (xstart <= end)) {
				end = xstart;
			} else if ((xend >= start) && (xend <= end)) {
				start = xend;
			}
		}
		/*
		 * If the trimming actions above left a non-zero size, create an
		 * available entry for it.
		 */
		if (end > start) {
			avail[acnt++] = start;
			avail[acnt++] = end;
			availmem += arm32_btop(end - start);
		}
		if (acnt >= MAX_AVAIL_ENTRIES)
			panic("Not enough space in the dump/phys_avail arrays");
	}

	return (availmem);
}

/*
 * Insertion-sort a new entry into a regions list; sorted by start address.
 */
static void
insert_region(struct region *regions, size_t rcnt, vm_offset_t addr,
    vm_size_t size, uint32_t flags)
{
	size_t i;
	struct region *ep, *rp;

	ep = regions + rcnt;
	for (i = 0, rp = regions; i < rcnt; ++i, ++rp) {
		if (addr < rp->addr) {
			bcopy(rp, rp + 1, (ep - rp) * sizeof(*rp));
			break;
		}
	}
	rp->addr  = addr;
	rp->size  = size;
	rp->flags = flags;
}

/*
 * Add a hardware memory region.
 */
void
arm_physmem_hardware_region(vm_offset_t pa, vm_size_t sz)
{
	vm_offset_t adj;

	/*
	 * Filter out the page at PA 0x00000000.  The VM can't handle it, as
	 * pmap_extract() == 0 means failure.
	 */
	if (pa == 0) {
		pa  = PAGE_SIZE;
		sz -= PAGE_SIZE;
	}

	/*
	 * Round the starting address up to a page boundary, and truncate the
	 * ending page down to a page boundary.
	 */
	adj = round_page(pa) - pa;
	pa  = round_page(pa);
	sz  = trunc_page(sz - adj);

	if (hwcnt < nitems(hwregions))
		insert_region(hwregions, hwcnt++, pa, sz, 0);
}

/*
 * Add an exclusion region.
 */
void arm_physmem_exclude_region(vm_offset_t pa, vm_size_t sz, uint32_t exflags)
{
	vm_offset_t adj;

	/*
	 * Truncate the starting address down to a page boundary, and round the
	 * ending page up to a page boundary.
	 */
	adj = pa - trunc_page(pa);
	pa  = trunc_page(pa);
	sz  = round_page(sz + adj);

	if (excnt < nitems(exregions))
		insert_region(exregions, excnt++, pa, sz, exflags);
}

/*
 * Process all the regions added earlier into the global avail lists.
 */
void
arm_physmem_init_kernel_globals(void)
{

	regions_to_avail(dump_avail, EXFLAG_NODUMP);
	physmem = regions_to_avail(phys_avail, EXFLAG_NOALLOC);
}

#ifdef DDB
#include <ddb/ddb.h>

DB_SHOW_COMMAND(physmem, db_show_physmem)
{

	physmem_dump_tables(db_printf);
}

#endif /* DDB */

