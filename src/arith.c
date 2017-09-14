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
#include "arith.h"

typedef enum {
	EXP_VAL,
	EXP_OP,
	EXP_IND,
	EXP_END
} expect_t;

static err_t arith_parse_int(tokenizer_t *tok, int allow_index, const ilist_t **ext_anode, op_t closing) {

	err_t rv = E_OK;

	ilist_t *list = inline_list_init(10, &anode_memtype, (void(*)(void*))anode_init);

	anode_t *anode = ilist_add(list);

	// when true, next token should be a value
	expect_t expect = EXP_VAL;

	//op_t unary = OP_NONE;

	//while (tokenizer_next(tok, allow_index)) {
	do {
		//printf("loop at: type=%c len=%d op=%d -> %s (n=%p, subv.type='%c')\n", 
		//	tok->type, tok->len, tok->vals.op, tok->line+tok->ptr, anode, anode->val.subv.type);
		switch(expect) {
		case EXP_VAL:
			switch(tok->type) {
			case T_INIT:
				break;
			case T_BRACKET:
				anode->type = A_BRACKET;
				anode->val.subv.type = tok->vals.op;
				if (tokenizer_next(tok, allow_index)) {
					// NOTE cast necessary due to stupid standards nitpicking by gcc 
					// https://stackoverflow.com/questions/28701376/incompatible-pointer-types-and-constness
					arith_parse(tok, allow_index, (const ilist_t**) &anode->val.subv.value);

					if (tok->type != T_BRACKET) {
						rv = E_SYNTAX;
					}
				}
				expect = EXP_OP;
				anode = ilist_add(list);
				break;
			case T_NAME:
				// TODO
				break;
			case T_LITERAL:
				// new node for literal value
				anode->type = A_VALUE;
				anode->val.intv.type = tok->vals.literal.type;
				anode->val.intv.value = tok->vals.literal.value;
				expect = EXP_OP;
				anode = ilist_add(list);
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
				rv = E_SYNTAX;
				break;
			case T_STRING:
				if (tok->vals.string.len == 1) {
					anode->type = A_VALUE;
					anode->val.intv.type = LIT_CHAR;
					anode->val.intv.value = 0xff&tok->line[tok->vals.string.ptr];
					expect = EXP_OP;
					anode = ilist_add(list);
					break;
				} else
				if (tok->vals.string.len == 2) {
					anode->type = A_VALUE;
					anode->val.intv.type = LIT_TWOCHAR;
					// TODO: byte order
					anode->val.intv.value = (tok->line[tok->vals.string.ptr] << 8) + (tok->line[tok->vals.string.ptr+1]&0xff);
					expect = EXP_OP;
					anode = ilist_add(list);
					break;
				}
			case T_ERROR:
				rv = E_SYNTAX;
			case T_END:
				break;
			}
			break;
		case EXP_OP:	
			switch(tok->type) {
			case T_BRACKET:
				// closing bracket
				goto end;
				break;
			case T_TOKEN:
				if (tokenizer_op_details(tok->vals.op)->is_index) {
					anode->type = A_INDEX;
					anode->op = tok->vals.op;
					expect = EXP_IND;
					break;
				}
				if (tokenizer_op_details(tok->vals.op)->is_binary) {
					anode->op = tok->vals.op;
					expect = EXP_VAL;
					break;
				}
				goto end;
			case T_INIT:
			case T_NAME:
			case T_STRING:
			case T_LITERAL:
			case T_ERROR:
				rv = E_SYNTAX;
			case T_END:
				break;
			}
			break;
		case EXP_IND:
			if (tokenizer_op_details(tok->vals.op)->is_index) {
				anode = ilist_add(list);
				anode->type = A_INDEX;
				anode->op = tok->vals.op;
				expect = EXP_IND;
				break;
			}
			// fall-through
		case EXP_END:
end:
			if (closing) {
				if (tok->type == T_BRACKET) {
					if (tok->vals.op != closing) {
						rv = E_ARITH_CLOSING;
						goto exit;
					}
					
				}
			}
			goto exit;
			break;
		}	
	}
	while ((rv == E_OK) && tokenizer_next(tok, allow_index));
exit:
	if (anode->op == OP_NONE) {
		ilist_pop(list);
	}
	*ext_anode = list;

	//printf("return at tok: type=%c len=%d -> %s (n=%p, subv.type='%c')\n", 
	//	tok->type, tok->len, tok->line+tok->ptr, anode, anode->val.subv.type);

	return rv;
}


err_t arith_parse(tokenizer_t *tok, int allow_index, const ilist_t **ext_anode) {

	return arith_parse_int(tok, allow_index, ext_anode, OP_NONE);
}


