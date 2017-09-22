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

static inline void error_double_modifier(const tokenizer_t *tok, const char *newop, const char *oldop) {
        toklog_error(tok, "Found multiple modifiers: got %s, was already %s", newop, oldop);
}

static inline void error_unary_no_val(const tokenizer_t *tok) {
        toklog_error(tok, "%s", "Did not find value after unary operator");
}

static inline void error_bracket_mismatch(const tokenizer_t *tok, op_t was, op_t expected) {
        toklog_error(tok, "%s", "Opening and closing brackets don't match - got '%s' but expected '%s'.",
			tokenizer_op_details(was)->print, tokenizer_op_details(expected)->print);
}

static err_t arith_parse_int(tokenizer_t *tok, 
			const block_t *blk, 
			int allow_index, 
			const ilist_t **ext_anode, 
			op_t closing, 
			int val_only, 
			int allow_mod,
			int allow_string) {

	err_t rv = E_OK;

	ilist_t *list = inline_list_init(10, &anode_memtype, (void(*)(void*))anode_init);

	anode_t *anode = ilist_add(list);

	// when true, next token should be a value
	expect_t expect = EXP_VAL;

	//op_t unary = OP_NONE;

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
					arith_parse_int(tok, blk, allow_index, (const ilist_t**) &anode->val.subv.value, OP_NONE, 0, 0, allow_string);

					if (tok->type != T_BRACKET) {
						rv = E_SYNTAX;
					}
				} else {
					rv = E_SYNTAX;
				}
				expect = EXP_OP;
				anode = ilist_add(list);
				allow_mod = 0;
				break;
			case T_NAME:
				anode->type = A_LABEL;
				anode->val.lab.name = mem_alloc_strn(tok->line + tok->ptr, tok->len);
				anode->val.lab.label = block_find_label(blk, anode->val.lab.name);
				expect = EXP_OP;
				anode = ilist_add(list);
				allow_mod = 0;
				break;
			case T_LITERAL:
				// new node for literal value
				anode->type = A_VALUE;
				anode->val.intv.type = tok->vals.literal.type;
				anode->val.intv.value = tok->vals.literal.value;
				expect = EXP_OP;
				anode = ilist_add(list);
				allow_mod = 0;
				break;
			case T_TOKEN:
				// modifier?
				if (allow_mod && tokenizer_op_details(tok->vals.op)->is_modifier) {
					// conveniently mapped
					anode->modifier = tok->vals.op;
					allow_mod = 0;
					break;
				}
				if (tokenizer_op_details(tok->vals.op)->is_unary) {
					anode->val.unary.op = tok->vals.op;
					if (tokenizer_next(tok, allow_index)) {
						arith_parse_int(tok, blk, allow_index, (const ilist_t**) &anode->val.unary.value, OP_NONE, 1, 0, allow_string);
						expect = EXP_OP;
						anode->type = A_UNARY;
						anode = ilist_add(list);
						allow_mod = 0;
					} else {
						error_unary_no_val(tok);
						rv = E_SYNTAX;
					}
					break;
				}
				rv = E_SYNTAX;
				break;
			case T_STRING:
				if (allow_string) {
					anode->type = A_STRING;
					anode->val.strv.type = tok->vals.string.type;
					anode->val.strv.value = mem_alloc_strn(tok->line + tok->vals.string.ptr, 
											tok->vals.string.len);
					anode->val.strv.type = tok->vals.string.len;
					anode = ilist_add(list);
					expect = EXP_OP;
					break;
				} else {
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
						error_bracket_mismatch(tok, closing, tok->vals.op);
						rv = E_SYNTAX;
						goto exit;
					}
					
				}
			}
			goto exit;
			break;
		}	
	}
	while ((rv == E_OK) && (expect == EXP_VAL || !val_only) && tokenizer_next(tok, allow_index));
exit:
	if (anode->op == OP_NONE) {
		ilist_pop(list);
	}
	*ext_anode = list;

	if ((!rv) && (!tok->is_valid)) {
		rv = E_SYNTAX;
	}
	//printf("return at tok: type=%c len=%d -> %s (n=%p, subv.type='%c')\n", 
	//	tok->type, tok->len, tok->line+tok->ptr, anode, anode->val.subv.type);

	return rv;
}


err_t arith_parse(tokenizer_t *tok, const block_t *blk, int allow_index, const ilist_t **ext_anode, int allow_string) {

	return arith_parse_int(tok, blk, allow_index, ext_anode, OP_NONE, 0, 1, allow_string);
}


