/****************************************************************************

    parser
    Copyright (C) 2015-2017 Andre Fachat

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
#include <ctype.h>

#include "err.h"

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
	P_COLON,	// after either label or line number, where a colon can be
	P_END,		// expect end of line
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

	stmt->type = S_NONE;
	stmt->blk = p->blk;
	stmt->ctx = ctx;
	stmt->op = NULL;
	stmt->label = NULL;
	stmt->syn = SY_IMP;

	// negative line number is not set
	stmt->lineno = -1;

	stmt->rs_prefix = RS_NOT_SET;
	stmt->le_prefix = LE_NOT_SET;
	stmt->um_prefix = 0;
	stmt->nf_prefix = 0;

	stmt->comment = NULL;

	return stmt;
}

static statement_t *new_statement_in_line(const context_t *ctx, statement_t *prev) {

	statement_t *stmt = new_statement(ctx);

	stmt->lineno = prev->lineno;

	return stmt;
}


static void statement_push(const context_t *ctx, statement_t *stmt) {

	// statements in parse order
        list_add(p->statements, stmt);

	// statements per segment/region
	segment_push_statement(ctx->segment, stmt);
}


void parser_reset() {
        // only for tests - ignore that we would have to mfree stuff before
        p->statements = array_list_init(10000);
}

static inline void error_double_prefix(const tokenizer_t *tok, char prefix, char conflict) {
        toklog_error(tok, "Double prefix '%c' conflicting with previous '%c'", prefix, conflict);
}

static inline void warn_double_prefix(const tokenizer_t *tok, char prefix) {
        toklog_warn(tok, "Double prefix '%c'", prefix);
}

static inline void warn_operation_not_for_cpu(const tokenizer_t *tok, const char *op_name, const char *cpu_name) {
        toklog_warn(tok, "Operation name %s is not available for CPU %s, assuming label!", op_name, cpu_name);
}

static inline void error_assign_without_label(const tokenizer_t *tok) {
	toklog_error(tok, "%s", "Assign without label");
}


/**
 * parse the operation parameter incl. addressing mode
 */
err_t parse_param(tokenizer_t *tok, statement_t *stmt) {

	if (tok->type == T_TOKEN && tok->vals.op == OP_HASH) {

		stmt->syn = SY_IMM;

		if (tokenizer_next(tok, 0)) {
			return arith_parse_integer(tok, stmt->blk, 0, (const ilist_t**)&stmt->param);	
		}
		return E_OK;
	} else {
		err_t rv = arith_parse_integer(tok, stmt->blk, 1, (const ilist_t**)&stmt->param);	
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
				if (brtype == AB_RND 
						&& first->type == A_BRACKET 
						&& first->val.subv.type == AB_RND 
						&& ilist_len(stmt->param) == 1) {
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

			// 4th finally unwrapp base (,s / ,b / ,p) 
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

static err_t parse_prefix(tokenizer_t *tok, statement_t *stmt) {

	err_t rv = E_OK;

	for (int i = 0; i < tok->len && !rv; i++) {
		char c = tolower(tok->line[tok->ptr + i]);
		switch(c) {
		case 'u':
			if (stmt->um_prefix) {
				warn_double_prefix(tok, c);
			}
			stmt->um_prefix = 1;
			break;
		case 'n':
			if (stmt->nf_prefix) {
				warn_double_prefix(tok, c);
			}
			stmt->nf_prefix = 1;
			break;
		case 'b':
			if (stmt->rs_prefix != RS_NOT_SET) {
				error_double_prefix(tok, c, prefix_rs_char(stmt->rs_prefix));
				rv = E_SYNTAX;
			}
			stmt->rs_prefix = RS_BYTE;
			break;
		case 'w':
			if (stmt->rs_prefix != RS_NOT_SET) {
				error_double_prefix(tok, c, prefix_rs_char(stmt->rs_prefix));
				rv = E_SYNTAX;
			}
			stmt->rs_prefix = RS_WORD;
			break;
		case 'l':
			if (stmt->rs_prefix != RS_NOT_SET) {
				error_double_prefix(tok, c, prefix_rs_char(stmt->rs_prefix));
				rv = E_SYNTAX;
			}
			stmt->rs_prefix = RS_LONG;
			break;
		case 'q':
			if (stmt->rs_prefix != RS_NOT_SET) {
				error_double_prefix(tok, c, prefix_rs_char(stmt->rs_prefix));
				rv = E_SYNTAX;
			}
			stmt->rs_prefix = RS_QUAD;
			break;
		case 'e':
			if (stmt->le_prefix != LE_NOT_SET) {
				error_double_prefix(tok, c, prefix_le_char(stmt->le_prefix));
				rv = E_SYNTAX;
			}
			stmt->le_prefix = LE_E;
			break;
		case 's':
			if (stmt->le_prefix != LE_NOT_SET) {
				error_double_prefix(tok, c, prefix_le_char(stmt->le_prefix));
				rv = E_SYNTAX;
			}
			stmt->le_prefix = LE_S;
			break;
		case '0':
			if (stmt->le_prefix != LE_NOT_SET) {
				error_double_prefix(tok, c, prefix_le_char(stmt->le_prefix));
				rv = E_SYNTAX;
			}
			stmt->le_prefix = LE_0;
			break;
		case '1':
			if (stmt->le_prefix != LE_NOT_SET) {
				error_double_prefix(tok, c, prefix_le_char(stmt->le_prefix));
				rv = E_SYNTAX;
			}
			stmt->le_prefix = LE_1;
			break;
		default:
			rv = E_SYNTAX;
			break;
		}
	}
	return rv;
}

err_t parser_push(const context_t *ctx, const line_t *line) {
	err_t rv = E_OK;

	if (line == NULL) {
		return E_END;
	}

	const parser_config_t *cfg = line->parsercfg;
	position_t *pos = line->position;

	// is the first block already set?
	if (p->blk == NULL) {
		p->blk = block_init(NULL, pos);
	}

	statement_t *stmt = new_statement(ctx);

	const instruction_t *op = NULL;
	const char *name = NULL;
	label_t *label = NULL;

	// tokenize the line
	tokenizer_t *tok = tokenizer_create(line->position, line->line, cfg->cstyle_allowed);

	tokenizer_next(tok, 0);

	while ((rv == E_OK) && tok->type != T_END && tok->is_valid) {

		while (tok->type == T_TOKEN && tok->vals.op == OP_COLON) {
			// ignore colon at beginning of line
			tokenizer_next(tok, 0);
		}

		// initial line number
		if (tok->type == T_LITERAL) {
			if (cfg->initial_lineno && (tok->vals.literal.type == LIT_DECIMAL)) {
				// line number parsing
				stmt->lineno = tok->vals.literal.value;

				tokenizer_next(tok, 0);
			} 

			if (tok->type == T_TOKEN && tok->vals.op == OP_COLON) {
				// ignore colon directly after line number
				tokenizer_next(tok, 0);
			}
		}

		while (stmt->op == NULL && tok->type == T_NAME) {
			name = mem_alloc_strn(tok->line + tok->ptr, tok->len);
			op = instruction_find(name);
			if (op != NULL) {
				// check if the operation is compatible with the current CPU
				if (0 == (ctx->cpu->isa & op->isa)) {
					// TODO: config for either no message or error
					warn_operation_not_for_cpu(tok, name, ctx->cpu->name);
					op = NULL;
				}
			}
			if (op == NULL) {
				// label
				// TODO: redefinition?
				label = label_init(ctx, name, pos);
				if (stmt->label) {
					// we already had a label
					stmt->type = S_LABEQPC;
					statement_push(ctx, stmt);
					stmt = new_statement_in_line(ctx, stmt);
				}
				stmt->label = label;
		
				tokenizer_next(tok, 0);
				if (tok->type == T_END) {
					stmt->type = S_LABEQPC;
				} else
				if (tok->type == T_TOKEN && tok->vals.op == OP_COLON) {
					// if we have a label with a colon, ignore colon
					tokenizer_next(tok, 0);
				}
			} else {
				// operation
				stmt->type = S_OPCODE;
				stmt->op = op;
				tokenizer_next(tok, 0);
				if (tok->type == T_TOKEN && tok->vals.op == OP_DOT) {
					// parse prefix
					if (tokenizer_next_prefix(tok) && (tok->type == T_NAME)) {
						rv = parse_prefix(tok, stmt);
					} else {
						error_syntax(tok, "Expecting prefix notation after '.'");
						rv = E_SYNTAX;
					}
					if (rv) {
						break;
					}
					tokenizer_next(tok, 0);
				}
	
				if (tok->type != T_TOKEN 
					|| (tok->vals.op != OP_SEMICOLON
						&& (tok->vals.op != OP_DOUBLESLASH || cfg->cstyle_allowed)
						&& (tok->vals.op != OP_COLON))) {
	
					// parse parameters
					rv = parse_param(tok, stmt);
	
					if (rv != E_OK) {
						break;
					}
				}
			}
		}

		if (!stmt->op) {
			if (tok->type == T_TOKEN && tok->vals.op == OP_DOT) {
				if (tokenizer_next(tok, 0)) {
					rv = parse_pseudo(tok, stmt);
					stmt->type = S_PSEUDO;
				} else {
					rv = E_SYNTAX;
				}
				if (rv) {
					break;
				}
			} else
			if (tok->type == T_TOKEN && tokenizer_op_details(tok->vals.op)->is_assign) {
				if (stmt->label == NULL && tok->vals.op != OP_ASSIGNMULT) {
					error_assign_without_label(tok);
					rv = E_SYNTAX;
					break;
				}
				if (stmt->label == NULL && tok->vals.op == OP_ASSIGNMULT) {
					stmt->type = S_PCDEF;
				} else {
					stmt->type = S_LABDEF;
				}
				stmt->assign = tok->vals.op;
				tokenizer_next(tok, 0);
				// parse parameters
				rv = parse_param(tok, stmt);
				if (rv) {
					break;
				}
			} else
			if (tok->type != T_END
				&& tok->vals.op != OP_SEMICOLON
				&& (tok->vals.op != OP_DOUBLESLASH || cfg->cstyle_allowed)
				&& (tok->vals.op != OP_COLON)) {
				rv = E_SYNTAX;
				error_syntax(tok, "Expecting opcode");
				break;
			}
		}

		if (tok->type == T_TOKEN) {
			if ((tok->vals.op == OP_SEMICOLON
				|| (tok->vals.op == OP_DOUBLESLASH && cfg->cstyle_allowed))) {

				if (tokenizer_next_comment(tok, cfg->colon_in_comments)) {
					stmt->comment = mem_alloc_strn(tok->line+tok->ptr, tok->len);

					tokenizer_next(tok, 0);
				}
			} 
		}

		if (tok->type == T_TOKEN) {
			if (tok->vals.op == OP_COLON) {
	
				statement_push(ctx, stmt);
				stmt = new_statement_in_line(ctx, stmt);

				tokenizer_next(tok, 0);
			} else {
				rv = E_SYNTAX;
				error_syntax(tok, "Expecting comment or colon");
			}
		}
	}

	if (rv == E_OK || rv == E_END) {
		statement_push(ctx, stmt);
	}
	tokenizer_free(tok);

	return rv;
}


list_iterator_t *parser_get_statements(void) {

	return list_iterator(p->statements);
}

