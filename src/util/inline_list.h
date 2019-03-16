/****************************************************************************

    list handling
    Copyright (C) 2017 Andre Fachat

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

****************************************************************************/

/*
   A list that stores the entries in an array.
   (this is different from arraylist, that only stores pointers to the entry)

   Because this requires that a pointer to the memory for the object must
   be returned by the list, not added to it, the API changes and
   inline_list does not implement the general list interface.
*/


#ifndef INLINE_LIST_H
#define INLINE_LIST_H

#include <stdio.h>

#include "types.h"
#include "mem.h"


typedef struct {
	int		cap;		// capacity of list
	int		len;		// number of items in list
	int		popped;		// flag, if true add is not allowed anymore
	const type_t	*type;		// type info for array element
					// function to init a new item
	void 		(*init_item)(void*);
					// list of inlined items
	void		*values;
} ilist_t;

ilist_t *inline_list_init(int bucket_size, const type_t *type, void (*init_item)(void*));

static inline int ilist_len(const ilist_t *il) {
	return il->len;
}

static inline void* ilist_get_internal(const ilist_t *ilist, int itemno) {
	char *ptr = (char*) ilist->values;
	return (void*) &ptr[ilist->type->sizeoftype * itemno];
}

static inline void* ilist_add(ilist_t *ilist) {

	if (ilist->popped) {
		// we have popped stuff, so we cannot overwrite it!
		return NULL;
	}
	
	ilist->len++;
	if (ilist->len >= ilist->cap) {
		ilist->cap = ilist->cap * 2;
		ilist->values = mem_realloc_n(ilist->cap, ilist->type, ilist->values);
	}
	return ilist_get_internal(ilist, ilist->len - 1);
}

static inline void* ilist_pop(ilist_t *ilist) {

	if (ilist->len > 0) {
		ilist->popped = 1;
		ilist->len --;
		return ilist_get_internal(ilist, ilist->len);
	} else {
		// TODO: Error
		return NULL;
	}
}

static inline void* ilist_get(const ilist_t *ilist, int itemno) {

	if (ilist->len <= itemno) {
		return NULL;
	}
	return ilist_get_internal(ilist, itemno);
}

static inline void* ilist_last(const ilist_t *ilist) {

	if (ilist->len == 0) {
		return NULL;
	}
	return ilist_get_internal(ilist, ilist->len - 1);
}

#endif

