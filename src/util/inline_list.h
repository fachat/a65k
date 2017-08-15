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


#ifndef INLINE_LIST_H
#define INLINE_LIST_H

#include <stdio.h>

#include "types.h"
#include "mem.h"


typedef struct {
	int		cap;		// capacity of list
	int		len;		// number of items in list
	const type_t	*type;		// type info for array element
					// function to init a new item
	void 		(*init_item)(void*);
					// list of inlined items
	void		*values;
} ilist_t;

ilist_t *inline_list_init(int bucket_size, const type_t *type, void (*init_item)(void*));

static inline int ilist_len(ilist_t *il) {
	return il->len;
}

static inline void* ilist_add(ilist_t *ilist) {
	
	ilist->len++;
	if (ilist->len >= ilist->cap) {
		ilist->cap = ilist->cap * 2;
		ilist->values = mem_realloc_n(ilist->cap, ilist->type, ilist->values);
	}
	char *ptr = (char*) ilist->values;

	return (void*) &ptr[ilist->type->sizeoftype * (ilist->len-1)];
}

static inline void* ilist_get(ilist_t *ilist, int itemno) {

	if (ilist->len >= itemno) {
		return NULL;
	}
	char *ptr = (char*) ilist->values;
	return (void*) &ptr[ilist->type->sizeoftype * itemno];
}

#endif

