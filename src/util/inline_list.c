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


#include <stdio.h>

#include "types.h"
#include "inline_list.h"


static type_t ilist_memtype = {
	"ilist",
	sizeof(ilist_t)
};

ilist_t *inline_list_init(int bucket_size, const type_t *type, void (*init_item)(void*)) {

	ilist_t *ilist = mem_alloc(&ilist_memtype);

	ilist->cap = bucket_size;
	ilist->len = 0;
	ilist->type = type;
	ilist->init_item = init_item;
	ilist->values = mem_alloc_n(bucket_size, type);

	return ilist;
}


