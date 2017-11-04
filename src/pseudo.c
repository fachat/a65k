/****************************************************************************

    Manage pseudo opcodes
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


#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "pseudo.h"
#include "hashmap.h"
#include "expr.h"
#include "array_list.h"
#include "parser.h"
#include "mem.h"

static inline void error_parse_syntax(const tokenizer_t *tok, const char *name) {
        toklog_error(tok, "Could not parse parameter for pseudo opcode '%s'", name);
}

static err_t pseudo_list_parse(const pseudo_t *pseudo, tokenizer_t *tok, statement_t *stmt) {

	err_t rv = E_OK;
	list_t *pparams = array_list_init(8);
	stmt->pparams = pparams;

	int allow_string = !strcmp(pseudo->name, "asc");

	ilist_t *nodep;
	while ((rv = arith_parse(tok, stmt->blk, 0, (const ilist_t**)&nodep, allow_string)) == E_OK) {
		list_add(pparams, nodep);

		if (tok->type != T_TOKEN || tok->vals.op != OP_COMMA) {
			// break if not comma (separator)
			break;
		}
		if (!tokenizer_next(tok, 0)) {
			break;
		}
	}
	if (rv) {
		error_parse_syntax(tok, pseudo->name);
	}
	return rv;
}

static err_t pseudo_param_parse(const pseudo_t *pseudo, tokenizer_t *tok, statement_t *stmt) {

	err_t rv = E_OK;
	list_t *pparams = array_list_init(8);
	stmt->pparams = pparams;

	ilist_t *nodep;
	if ((rv = arith_parse(tok, stmt->blk, 0, (const ilist_t**)&nodep, 0)) == E_OK) {
		list_add(pparams, nodep);
	}

	if (rv) {
		error_parse_syntax(tok, pseudo->name);
	}
	return rv;
}

static err_t pseudo_end_parse(const pseudo_t *pseudo, tokenizer_t *tok, statement_t *stmt) {

	(void) tok;
	(void) pseudo;
	(void) stmt;

	return E_END;
}

static pseudo_t pseudos[] = {
	{ "end",	pseudo_end_parse,	NULL,		NULL },
	{ "if",		pseudo_param_parse,	NULL,		NULL },
	{ "goto",	pseudo_param_parse,	NULL,		NULL },
	{ "got",	pseudo_param_parse,	NULL,		NULL },
	{ "asc",	pseudo_list_parse,	NULL,		NULL },
	{ "byt",	pseudo_list_parse,	NULL,		NULL },
	{ "byte",	pseudo_list_parse,	NULL,		NULL },
	{ "word",	pseudo_list_parse,	NULL,		NULL },
	{ "(",		NULL,			NULL,		NULL },
	{ ")",		NULL,			NULL,		NULL },
};

static hash_t *pseudomap = NULL;


static const char *name_from_pseudo(const void *pseudo) {
	return ((pseudo_t*)pseudo)->name;
}

// init the pseudo opcodes
void pseudo_module_init(void) {

	int n = sizeof(pseudos)/sizeof(pseudo_t);
	pseudomap = hash_init_stringkey_nocase(n, n/2+1, name_from_pseudo);

	for (int i = 0; i < n; i++) {
		if (hash_put(pseudomap, &pseudos[i]) != NULL) {
			// TODO error, double entry should not happen
			exit(1);
		}
	}
}

err_t parse_pseudo(tokenizer_t *tok, statement_t *stmt) {

	err_t rv = E_OK;

	char *name = NULL;
	if (tok->type == T_NAME) {
		name = mem_alloc_strn(tok->line + tok->ptr, tok->len);
	} else
	if (tok->type == T_BRACKET) {
		name = mem_alloc_str(tokenizer_op_details(tok->vals.op)->print);
	}

	pseudo_t *p = hash_get(pseudomap, name);
	if (p != NULL) {

		stmt->pseudo = p;
		tokenizer_next(tok, 0);

		if (p->parse) {
			rv = p->parse(p, tok, stmt);
		}
	} else {
		error_syntax(tok, "Expect pseudo opcode after '.'");
		rv = E_SYNTAX;
	}

	mem_free(name);

	return rv;
}



