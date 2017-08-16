/****************************************************************************

    arithmetic operations and expressions
    Copyright (C) 2015 Andre Fachat

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
#include "arith.h"


void arith_parse(tokenizer_t *tok, int allow_index, const ilist_t **ext_anode) {

	ilist_t *list = inline_list_init(10, &anode_memtype, (void(*)(void*))anode_init);

	anode_t *anode = ilist_add(list);

	// when true, next token should be a value
	int expect_val = 1;

	op_t unary = OP_NONE;

	//while (tokenizer_next(tok, allow_index)) {
	do {
	
		if (expect_val) {
			switch(tok->type) {
			case T_INIT:
				break;
			case T_BRACKET:
				// TODO
/*
				// open up a bracket
				new_anode = anode_init(A_BRACKET, anode);
				// bracket type
				new_anode->op = tok->vals.op;
				new_anode->op = unary;
				if (anode != NULL) {
					anode->child = new_anode;
				}
				unary = OP_NONE;
				anode = new_anode;
				new_anode = NULL;
				expect_val = 1;
*/
				break;
			case T_NAME:
				// TODO
				break;
			case T_LITERAL:
				// new node for literal value
				anode->type = A_VALUE;
				anode->val.intv.type = tok->vals.literal.type;
				anode->val.intv.value = tok->vals.literal.value;
				expect_val = 0;
				break;
			case T_TOKEN:
				// modifier?
				if (tokenizer_op_details(tok->vals.op)->is_modifier) {
					if (anode->modifier != AMOD_NONE) {
						// TODO log error
					} else {
						// conveniently mapped
						anode->modifier = tok->vals.op;
					}
					break;
				}
				// unary operator, like inversion, negative
/*
				if (unary != OP_NONE) {
					new_anode = anode_init(A_VALUE, anode);
					new_anode->val.intv.type = LIT_NONE;
					new_anode->op = unary;
					unary = OP_NONE;
					anode = new_anode;
					new_anode = NULL;
				}
				unary = tok->vals.op;
*/
				break;
			case T_STRING:
			case T_ERROR:
				// TODO syntax error
			case T_END:
				break;
			}		
		} else {
			switch(tok->type) {
			case T_INIT:
			case T_BRACKET:
				// closing bracket
			case T_NAME:
			case T_TOKEN:
				anode->op = tok->vals.op;
				// identify arithmetic tokens
				// TODO check for comment tokens etc
				expect_val = 1;
				break;
			case T_STRING:
			case T_LITERAL:
			case T_ERROR:
			case T_END:
				break;
			}
		}	
	}
	while (tokenizer_next(tok, allow_index));

	*ext_anode = list;
}



