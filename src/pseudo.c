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

#include "err.h"
#include "pseudo.h"
#include "hashmap.h"
#include "arith.h"
#include "array_list.h"
#include "parser.h"
#include "mem.h"

static err_t arith_list_parse(tokenizer_t *tok, statement_t *stmt) {

	err_t rv = E_OK;
	list_t *pparams = array_list_init(8);
	stmt->pparams = pparams;

	ilist_t *nodep;
	while ((rv = arith_parse(tok, 0, (const ilist_t**)&nodep)) == E_OK) {
		list_add(pparams, nodep);

		if (tok->type != T_TOKEN || tok->vals.op != OP_COMMA) {
			break;
		}
		if (!tokenizer_next(tok, 0)) {
			break;
		}
	}
	return rv;
}


static pseudo_t pseudos[] = {
	{ "byt",	arith_list_parse,	NULL,		NULL },
	{ "byte",	arith_list_parse,	NULL,		NULL },
	{ "word",	arith_list_parse,	NULL,		NULL },
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
			rv = p->parse(tok, stmt);
		}
	} else {
		rv = E_SYNTAX;
	}

	mem_free(name);

	return rv;
}



