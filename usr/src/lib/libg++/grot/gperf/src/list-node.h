/* This may look like C code, but it is really -*- C++ -*- */

/* Data and function members for defining values and operations of a list node.

   Copyright (C) 1989 Free Software Foundation, Inc.
   written by Douglas C. Schmidt (schmidt@ics.uci.edu)

This file is part of GNU GPERF.

GNU GPERF is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU GPERF is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU GPERF; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#pragma once
#include "std-err.h"
#include "vectors.h"

struct List_Node : private Std_Err, private Vectors
{ 
  List_Node  *link;              /* TRUE if key has an identical KEY_SET as another key. */
  List_Node  *next;              /* Points to next element on the list. */
  char       *key;               /* Each keyword string stored here. */
  char       *rest;              /* Additional information for building hash function. */
  char       *char_set;          /* Set of characters to hash, specified by user. */
  int         length;            /* Length of the key. */
  int         hash_value;        /* Hash value for the key. */
  int         occurrence;        /* A metric for frequency of key set occurrences. */
  int         index;             /* Position of this node relative to other nodes. */

              List_Node (char *key, int len);
  static void set_sort (char *base, int len);
};
