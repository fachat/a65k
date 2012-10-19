/****************************************************************************

    list handling
    Copyright (C) 2012 Andre Fachat

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

#include "linked_list.h"
#include "mem.h"

// static struct inits

static type_t list_memtype = {
	"linked_list",
	sizeof(linked_list_t)
};

static type_t list_item_memtype = {
	"linked_list_item",
	sizeof(linked_list_item_t)
};

static type_t list_iterator_memtype = {
	"linked_list_iterator",
	sizeof(linked_list_iterator_t)
};


static void            	llist_add(list_t *list, void *data);
static list_iterator_t 	*llist_iterator(list_t *list);
static void            	*llist_next(list_iterator_t *iter);
static bool_t          	llist_has_next(list_iterator_t *iter);
static void 		llist_free(list_iterator_t *iter);
static void            	*llist_pop(list_t *list);
static void            	*llist_get_last(list_t *list);

static list_type_t list_type = {
	llist_add,
	llist_iterator,
	llist_next,
	llist_has_next,
	llist_free,
	llist_pop,
	llist_get_last
};

// init linked list
list_t *linked_list_init() {

	linked_list_t *list = mem_alloc(&list_memtype);

	list_init((list_t*)list, &list_type);

	list->first = NULL;
	list->last = NULL;

	return (list_t*) list;
}

static void llist_free(list_iterator_t *iter) {
	mem_free(iter);
}

static void list_item_free(linked_list_item_t *item) {
	mem_free(item);
}

static void llist_add(list_t *_list, void *data) {

	linked_list_t *list = (linked_list_t*)_list;

	linked_list_item_t *item = mem_alloc(&list_item_memtype);

	item->prev = list->last;
	item->next = NULL;
	item->data = data;

	if (list->last) {
		list->last->next = item;
	}
	list->last = item;
	if (!list->first) {
		list->first = item;
	}
}

static void *llist_get_last(list_t *_list) {

	linked_list_t *list = (linked_list_t*)_list;

	linked_list_item_t *last = list->last;

	return last == NULL ? NULL : last->data;
}

static void *llist_pop(list_t *_list) {

	linked_list_t *list = (linked_list_t*)_list;

	linked_list_item_t *last = list->last;
	if (list->last != NULL) {
		linked_list_item_t *newlast = (linked_list_item_t*) last->prev;
		list->last = newlast;
		if (newlast != NULL) {
			newlast->next = NULL;
		} else {
			// newlast is NULL, i.e. was single item only, which is now gone
			list->first = NULL;
		}
		void *value = last->data;
		
		list_item_free(last);

		return value;
	}
	return NULL;
}


// forward iterator
list_iterator_t *llist_iterator(list_t *_list) {

	linked_list_t *list = (linked_list_t*) _list;

	linked_list_iterator_t *iter = mem_alloc(&list_iterator_memtype);

	list_iterator_init((list_iterator_t*)iter, _list);
	iter->current = list->first;

	return (list_iterator_t*) iter;
}


static void *llist_next(list_iterator_t *_iter) {

	linked_list_iterator_t *iter = (linked_list_iterator_t*)_iter;

	linked_list_item_t *item = iter->current;
	if (item != NULL) {
		iter->current = item->next;
		return item->data;
	}
	return NULL;
}


static bool_t llist_has_next(list_iterator_t *iter) {

	return ((linked_list_iterator_t*)iter)->current != NULL;
}
 

