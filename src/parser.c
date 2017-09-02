/****************************************************************************

    parser
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

#define	DEBUG

#include <stdio.h>

#include "log.h"
#include "mem.h"
#include "list.h"
#include "array_list.h"
#include "hashmap.h"
#include "position.h"

#include "cpu.h"
#include "segment.h"
#include "context.h"
#include "parser.h"
#include "label.h"
#include "block.h"
#include "tokenizer.h"
#include "operation.h"
#include "errors.h"
#include "arith.h"
#include "parser-config.h"

typedef struct {
	block_t			*blk;

	list_t			*statements;
} parser_t;


typedef enum {
	P_INIT,		// start of line, accept label definitions, operations and pseudos
	P_OP,		// has parsed label, accept "=" or operation
	P_PARAM,	// after operation, parse parameters to the operation
} pstate_t;


static const type_t statement_memtype = {
	"statement_t",
	sizeof(statement_t)
};

static const type_t parser_memtype = {
	"parser_t",
	sizeof(parser_t)
};

static parser_t *p = NULL;

void parser_module_init(void) {

	p = mem_alloc(&parser_memtype);
	p->blk = NULL;
	p->statements = array_list_init(10000);
	
	parser_config_init();
}

static statement_t *new_statement(const context_t *ctx) {
	statement_t *stmt = mem_alloc(&statement_memtype);
	stmt->blk = p->blk;
	stmt->ctx = ctx;
	stmt->op = NULL;
	stmt->setlabel = NULL;
	stmt->label = NULL;
	stmt->syn = SY_IMP;
	return stmt;
}

static void statement_push(statement_t *stmt) {
	list_add(p->statements, stmt);
}


void parser_reset() {
	// only for tests - ignore that we would have to mfree stuff before
	p->statements = array_list_init(10000);
}

/**
 * parse the operation parameter incl. addressing mode
 */
err_t param_parse(tokenizer_t *tok, statement_t *stmt) {

	if (tok->type == T_TOKEN && tok->vals.op == OP_HASH) {

		stmt->syn = SY_IMM;

		if (tokenizer_next(tok, 0)) {
			return arith_parse(tok, 0, (const ilist_t**)&stmt->param);	
		}
		return E_OK;
	} else {
		err_t rv = arith_parse(tok, 1, (const ilist_t**)&stmt->param);	
		if (rv == E_OK && stmt->param->len > 0) {

			stmt->syn = SY_ABS;

			anode_t *outer = NULL;
			anode_t *inner = NULL;
			anode_t *base = NULL;
			asub_type brtype = 0;
			anode_t *last = NULL;

			// unwrap brackets and/or index tokens (',x')

			// 1st unwrap ending indexes, just ",y", ",z"  
			last = ilist_last(stmt->param);
			if (last && last->type == A_INDEX && (last->op == OP_YIND || last->op == OP_ZIND)) {
				outer = ilist_pop(stmt->param);
			}

			// 2nd unwrap brackets
			anode_t *first = ilist_get(stmt->param, 0);
			if (first->type == A_BRACKET && ilist_len(stmt->param) == 1) {
				// outer node is a bracket, and it is the only one
				brtype = first->val.subv.type; 
				ilist_t *basenode = first->val.subv.value;
				stmt->param = basenode;

				anode_t *first = ilist_get(stmt->param, 0);
				if (brtype == AB_RND && first->type == A_BRACKET && first->val.subv.type == AB_RND && ilist_len(stmt->param) == 1) {
					brtype = AB_DBLRND;
					ilist_t *basenode = first->val.subv.value;
					stmt->param = basenode;
				}
			}

			// 3rd from resulting unwrap ,x index
			last = ilist_last(stmt->param);
			if (last && last->type == A_INDEX && last->op == OP_XIND) {
				inner = ilist_pop(stmt->param);
			}

			// 4th finally unwrapp base (,s / ,b) 
			last = ilist_last(stmt->param);
			if (last && last->type == A_INDEX) {
				base = ilist_pop(stmt->param);
				stmt->base = base->op;
			}

			// then compute syntax identifier
			// note that base is independent of syntax
			if (inner != NULL) {
				if (outer != NULL) {
					rv = E_SYNTAX;
				}
				switch (brtype) {
				case 0:
					stmt->syn = SY_ABSX;
					break;
				case AB_RND:
					stmt->syn = SY_XIND;
					break;
				case AB_RCT:
					stmt->syn = SY_XINDL;
					break;
				case AB_DBLRCT:
					stmt->syn = SY_XINDQ;
					break;
				case AB_DBLRND:
					stmt->syn = SY_XINDD;
					break;
				default:
					// borken
					rv = E_SYNTAX;
					break;
				}
			} else if (outer != NULL && outer->op == OP_YIND) {
				switch (brtype) {
				case 0:
					stmt->syn = SY_ABSY;
					break;
				case AB_RND:
					stmt->syn = SY_INDY;
					break;
				case AB_RCT:
					stmt->syn = SY_INDYL;
					break;
				case AB_DBLRCT:
					stmt->syn = SY_INDYQ;
					break;
				case AB_DBLRND:
					stmt->syn = SY_INDYD;
					break;
				default:
					// borken
					rv = E_SYNTAX;
					break;
				}
			} else if (outer != NULL && outer->op == OP_ZIND) {
				switch (brtype) {
				case 0:
					stmt->syn = SY_ABSZ;
					break;
				case AB_RND:
					stmt->syn = SY_INDZ;
					break;
				case AB_RCT:
					stmt->syn = SY_INDZL;
					break;
				case AB_DBLRCT:
					stmt->syn = SY_INDZQ;
					break;
				case AB_DBLRND:
					stmt->syn = SY_INDZD;
					break;
				default:
					// borken
					rv = E_SYNTAX;
					break;
				}
				
			
			}
		}
		return rv;
	}
}

err_t parser_push(const context_t *ctx, const line_t *line) {

	err_t rv = E_OK;

	const parser_config_t *cfg = line->parsercfg;
	position_t *pos = line->position;

	// is the first block already set?
	if (p->blk == NULL) {
		p->blk = block_init(NULL, pos);
	}

	statement_t *stmt = new_statement(ctx);

	const operation_t *op = NULL;
	const char *name = NULL;
	label_t *label = NULL;

	// allow the tokenizer to fold comma into ",x" etc addressing mode tokens
	int allow_index = 0;

	// tokenize the line
	pstate_t state = P_INIT;
	tokenizer_t *tok = tokenizer_create(line->line);
	while ((rv == E_OK) && tokenizer_next(tok, allow_index)) {
		switch(state) {
		case P_OP:
			if (tok->type == T_TOKEN && tok->vals.op == OP_COLON) {
				// accept after label
				// continue to next 
				stmt->type = S_LABEQPC;
				statement_push(stmt);
				stmt = new_statement(ctx);
				state = P_INIT;
				break;
			}
			if (tok->type == T_TOKEN && tok->vals.op == OP_ASSIGN) {
				// after label, that's a label value definition
				stmt->type = S_LABDEF;
				// next define the label from param
				state = P_PARAM;
				break;
			}
			// fall-through!
		case P_INIT:
			switch(tok->type) {
			case T_NAME:
				name = mem_alloc_strn(tok->line + tok->ptr, tok->len);
				op = operation_find(name);
				if (op != NULL) {
					// check if the operation is compatible with the current CPU
					if (0 == (ctx->cpu->isa & op->isa)) {
						// TODO: config for either no message or error
						warn_operation_not_for_cpu(pos, name, ctx->cpu->name);
						op = NULL;
					}
				}
				if (op == NULL) {
					// label
					// TODO: redefinition?
					label = label_init(ctx, name, pos);
					if (state == P_OP) {
						// we already had a label
						stmt->type = S_LABEQPC;
						statement_push(stmt);
						stmt = new_statement(ctx);
					}
					stmt->label = label;
					// expect operation next (but accept labels too)
					state = P_OP;
				} else {
					// operation
					stmt->op = op;
					state = P_PARAM;
				}
				break;
			default:
				// syntax error
				error_syntax(pos);
				rv = E_SYNTAX;
				break;
			}
			break;
		case P_PARAM:
			// parse parameters
			rv = param_parse(tok, stmt);
			break;
		default:
			error_syntax(pos);
			goto end;
			break;
		};
	}
	statement_push(stmt);
end:
	tokenizer_free(tok);

	return rv;
}

list_iterator_t *parser_get_statements(void) {

	return list_iterator(p->statements);
}

