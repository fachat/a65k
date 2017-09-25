/****************************************************************************

    print formatted output
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

#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "print-config.h"
#include "print.h"
#include "arith.h"

#define	BUF_LEN		2048

static void print_arith_int(const ilist_t *anodes, int indlen);


void print_formatted_stmt(const statement_t *stmt, const print_config_t *cfg) {


	if (cfg->lineno) {
		if (stmt->lineno >= 0) {
			print("% 5d ", stmt->lineno);
		} else {
			print("      ");
		}
	}

	if (stmt->label != NULL) {
		const label_t *l = stmt->label;
		print("% -10s ", l->name);
	} else {
		print("            ");
	}
#if 0
	if (stmt->setlabel != NULL) {
		const ilist_t *a = stmt->setlabel;
		// TODO
		print("SET:");
		print_debug_arith_int(a, 4);
	}
#endif
	if (stmt->op != NULL) {
		const operation_t *o = stmt->op;
		print("%s", o->name);
	} else {
		print("   ");
	}
#if 0
	if (stmt->param != NULL) {
		const ilist_t *a = stmt->param;
		print("PAR: ");
		print_debug_arith_int(a, 4);
	}

	if (stmt->pseudo) {
		print("PSEUDO: %s", stmt->pseudo->name);

		if (stmt->pparams) {
			print("PARS:");
			list_iterator_t *iter = list_iterator(stmt->pparams);
			while (list_iterator_has_next(iter)) {
				ilist_t *parlist = list_iterator_next(iter);
				print_debug_arith_int(parlist, 4);
			}
		}
	}
#endif
	if (stmt->comment) {
		print("COMMENT: %s", stmt->comment);
	}	

	print("\n");
}

static inline char prop(int o) {  return isprint(o) ? o : ' '; }
static inline int max(int a, int b) { return (a > b) ? a : b; }
static const char* spaces = "                                                                         ";

static void print_arith_int(const ilist_t *anodes, int indlen) {
	const char* indent = spaces + max(0, strlen(spaces) - indlen);

	print("%sA:len=%d", indent, anodes->len);
	for (int i = 0; i < anodes->len; i++) {
		const anode_t *n = ilist_get(anodes, i);
		switch(n->type) {
		case A_BRACKET:
			print("  %stype=%c, modifier=%d(%c), op=%d(%c) btype=%d (%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op), n->val.subv.type, prop(n->val.subv.type));
			print_arith_int(n->val.subv.value, indlen + 2);
			break;
		case A_VALUE:
			print("  %stype=%c, modifier=%d (%c), op=%d(%c), val=(%c)%d/%x", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op),
				n->val.intv.type, n->val.intv.value, n->val.intv.value);
			break;
		case A_INDEX:
			print("  %stype=%c, modifier=%d (%c), op=%d(%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
			break;
		default:
			print("  UNHANDLED: %stype=%c, modifier=%d (%c), op=%d(%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
		}
	}
}


