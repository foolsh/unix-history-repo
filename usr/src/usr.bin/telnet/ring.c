/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)ring.c	8.1 (Berkeley) %G%";
#endif /* not lint */

/*
 * This defines a structure for a ring buffer.
 *
 * The circular buffer has two parts:
 *(((
 *	full:	[consume, supply)
 *	empty:	[supply, consume)
 *]]]
 *
 */

#include	<stdio.h>
#include	<errno.h>

#ifdef	size_t
#undef	size_t
#endif

#include	<sys/types.h>
#ifndef	FILIO_H
#include	<sys/ioctl.h>
#endif
#include	<sys/socket.h>

#include	"ring.h"
#include	"general.h"

/* Internal macros */

#if	!defined(MIN)
#define	MIN(a,b)	(((a)<(b))? (a):(b))
#endif	/* !defined(MIN) */

#define	ring_subtract(d,a,b)	(((a)-(b) >= 0)? \
					(a)-(b): (((a)-(b))+(d)->size))

#define	ring_increment(d,a,c)	(((a)+(c) < (d)->top)? \
					(a)+(c) : (((a)+(c))-(d)->size))

#define	ring_decrement(d,a,c)	(((a)-(c) >= (d)->bottom)? \
					(a)-(c) : (((a)-(c))-(d)->size))


/*
 * The following is a clock, used to determine full, empty, etc.
 *
 * There is some trickiness here.  Since the ring buffers are initialized
 * to ZERO on allocation, we need to make sure, when interpreting the
 * clock, that when the times are EQUAL, then the buffer is FULL.
 */
static u_long ring_clock = 0;


#define	ring_empty(d) (((d)->consume == (d)->supply) && \
				((d)->consumetime >= (d)->supplytime))
#define	ring_full(d) (((d)->supply == (d)->consume) && \
				((d)->supplytime > (d)->consumetime))





/* Buffer state transition routines */

    ring_init(ring, buffer, count)
Ring *ring;
    unsigned char *buffer;
    int count;
{
    memset((char *)ring, 0, sizeof *ring);

    ring->size = count;

    ring->supply = ring->consume = ring->bottom = buffer;

    ring->top = ring->bottom+ring->size;

#ifdef	ENCRYPTION
    ring->clearto = 0;
#endif	/* ENCRYPTION */

    return 1;
}

/* Mark routines */

/*
 * Mark the most recently supplied byte.
 */

    void
ring_mark(ring)
    Ring *ring;
{
    ring->mark = ring_decrement(ring, ring->supply, 1);
}

/*
 * Is the ring pointing to the mark?
 */

    int
ring_at_mark(ring)
    Ring *ring;
{
    if (ring->mark == ring->consume) {
	return 1;
    } else {
	return 0;
    }
}

/*
 * Clear any mark set on the ring.
 */

    void
ring_clear_mark(ring)
    Ring *ring;
{
    ring->mark = 0;
}

/*
 * Add characters from current segment to ring buffer.
 */
    void
ring_supplied(ring, count)
    Ring *ring;
    int count;
{
    ring->supply = ring_increment(ring, ring->supply, count);
    ring->supplytime = ++ring_clock;
}

/*
 * We have just consumed "c" bytes.
 */
    void
ring_consumed(ring, count)
    Ring *ring;
    int count;
{
    if (count == 0)	/* don't update anything */
	return;

    if (ring->mark &&
		(ring_subtract(ring, ring->mark, ring->consume) < count)) {
	ring->mark = 0;
    }
#ifdef	ENCRYPTION
    if (ring->consume < ring->clearto &&
		ring->clearto <= ring->consume + count)
	ring->clearto = 0;
    else if (ring->consume + count > ring->top &&
		ring->bottom <= ring->clearto &&
		ring->bottom + ((ring->consume + count) - ring->top))
	ring->clearto = 0;
#endif	/* ENCRYPTION */
    ring->consume = ring_increment(ring, ring->consume, count);
    ring->consumetime = ++ring_clock;
    /*
     * Try to encourage "ring_empty_consecutive()" to be large.
     */
    if (ring_empty(ring)) {
	ring->consume = ring->supply = ring->bottom;
    }
}



/* Buffer state query routines */


/* Number of bytes that may be supplied */
    int
ring_empty_count(ring)
    Ring *ring;
{
    if (ring_empty(ring)) {	/* if empty */
	    return ring->size;
    } else {
	return ring_subtract(ring, ring->consume, ring->supply);
    }
}

/* number of CONSECUTIVE bytes that may be supplied */
    int
ring_empty_consecutive(ring)
    Ring *ring;
{
    if ((ring->consume < ring->supply) || ring_empty(ring)) {
			    /*
			     * if consume is "below" supply, or empty, then
			     * return distance to the top
			     */
	return ring_subtract(ring, ring->top, ring->supply);
    } else {
				    /*
				     * else, return what we may.
				     */
	return ring_subtract(ring, ring->consume, ring->supply);
    }
}

/* Return the number of bytes that are available for consuming
 * (but don't give more than enough to get to cross over set mark)
 */

    int
ring_full_count(ring)
    Ring *ring;
{
    if ((ring->mark == 0) || (ring->mark == ring->consume)) {
	if (ring_full(ring)) {
	    return ring->size;	/* nothing consumed, but full */
	} else {
	    return ring_subtract(ring, ring->supply, ring->consume);
	}
    } else {
	return ring_subtract(ring, ring->mark, ring->consume);
    }
}

/*
 * Return the number of CONSECUTIVE bytes available for consuming.
 * However, don't return more than enough to cross over set mark.
 */
    int
ring_full_consecutive(ring)
    Ring *ring;
{
    if ((ring->mark == 0) || (ring->mark == ring->consume)) {
	if ((ring->supply < ring->consume) || ring_full(ring)) {
	    return ring_subtract(ring, ring->top, ring->consume);
	} else {
	    return ring_subtract(ring, ring->supply, ring->consume);
	}
    } else {
	if (ring->mark < ring->consume) {
	    return ring_subtract(ring, ring->top, ring->consume);
	} else {	/* Else, distance to mark */
	    return ring_subtract(ring, ring->mark, ring->consume);
	}
    }
}

/*
 * Move data into the "supply" portion of of the ring buffer.
 */
    void
ring_supply_data(ring, buffer, count)
    Ring *ring;
    unsigned char *buffer;
    int count;
{
    int i;

    while (count) {
	i = MIN(count, ring_empty_consecutive(ring));
	memcpy(ring->supply, buffer, i);
	ring_supplied(ring, i);
	count -= i;
	buffer += i;
    }
}

#ifdef notdef

/*
 * Move data from the "consume" portion of the ring buffer
 */
    void
ring_consume_data(ring, buffer, count)
    Ring *ring;
    unsigned char *buffer;
    int count;
{
    int i;

    while (count) {
	i = MIN(count, ring_full_consecutive(ring));
	memcpy(buffer, ring->consume, i);
	ring_consumed(ring, i);
	count -= i;
	buffer += i;
    }
}
#endif

#ifdef	ENCRYPTION
    void
ring_encrypt(ring, encryptor)
    Ring *ring;
    void (*encryptor)();
{
    unsigned char *s, *c;

    if (ring_empty(ring) || ring->clearto == ring->supply)
	return;

    if (!(c = ring->clearto))
	c = ring->consume;

    s = ring->supply;

    if (s <= c) {
	(*encryptor)(c, ring->top - c);
	(*encryptor)(ring->bottom, s - ring->bottom);
    } else
	(*encryptor)(c, s - c);

    ring->clearto = ring->supply;
}

    void
ring_clearto(ring)
    Ring *ring;
{
    if (!ring_empty(ring))
	ring->clearto = ring->supply;
    else
	ring->clearto = 0;
}
#endif	/* ENCRYPTION */
