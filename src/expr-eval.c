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
 * calculate result of an operation back into result
 * 	result <- result <op> nextres
 */
static err_t expr_op(eval_t *result, op_t op, eval_t *nextres) {

	switch(op) {
	}

	return E_SYNTAX;
}

/**
 * this method evaluates the expression given to it in the nodelist
 */
static err_t expr_eval_int(const ilist_t *nodelist, int *startp, eval_t *result, int prio) {

	eval_init(result);

	eval_t sub;
	err_t sub_err = E_OK;

	int p = *startp;
	int l = ilist_len(nodelist);

	anode_t *node = ilist_get(nodelist, p);

	switch(node->type) {
	case A_VALUE:
		result->value = node->val.intv.value;
		result->len = EVAL_MAX_LEN;
		result->type = EV_CONST;
		break;
	case A_BRACKET:
		eval_init(&sub);
		// TODO check len (or use external eval?)
		sub_err = expr_eval_int(node->val.subv.value, 0, &sub, 0);
		break;
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

	char p0 = prio_of_operator(node->op);
	p++;
	if (p0 > prio) {
		// prio of followup op is higher than previous op
		if (p >= l) {
			// evaluate next node
			eval_t nextres;
			sub_err = expr_eval_int(nodelist, &p, &nextres, p0);
			if (sub_err == E_OK) {
				// evaluate operation
				sub_err = expr_op(result, node->op, &nextres);
			}
		}
		
	}
	
	return E_OK;
}

err_t expr_eval(const ilist_t *nodelist, eval_t *result) {

	int p = 0;
	int n = ilist_len(nodelist);
	if (n > 0) {
		return expr_eval_int(nodelist, &p, result, 0);
	}
	return E_ABORT;
}


