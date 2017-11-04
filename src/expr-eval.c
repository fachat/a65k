/****************************************************************************

    arithmetic operations and expressions
    Copyright (C) 2015,2017 Andre Fachat

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


#include "types.h"
#include "mem.h"
#include "tokenizer.h"
#include "expr.h"
#include "err.h"


/**
 * this method evaluates the expression given to it in the nodelist
 */

err_t expr_eval(ilist_t *nodelist, eval_t *result) {

	eval_init(result);

	int n = ilist_len(nodelist);
	for (int i = 0; i < n; i++) {

		anode_t *node = ilist_get(nodelist, i);

		switch(node->type) {
		case A_VALUE:
			result->value = node->val.intv.value;
			result->len = eval_len_from_value(result->value);
			result->type = EV_CONST;
			break;
		case A_BRACKET:
		case A_INDEX:
		case A_LABEL:
		case A_UNARY:
		default:
			printf("error");
			break;
		}

		switch (node->modifier) {
		case AMOD_NONE:
			break;
		case AMOD_LOW:
			result->value &=0xff;
			result->len = 1;
			break;
		case AMOD_HIGH:
			result->value = (result->value >> 8) & 0xff;
			result->len = 1;
			break;
		}
	}

	return E_OK;
}

