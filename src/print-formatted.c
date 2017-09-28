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


static inline char prop(int o) {  return isprint(o) ? o : ' '; }
static inline int max(int a, int b) { return (a > b) ? a : b; }

static void print_arith_int(printer_t *prt, const ilist_t *anodes) {

	for (int i = 0; i < anodes->len; i++) {
		const anode_t *n = ilist_get(anodes, i);
		switch(n->type) {
		case A_BRACKET:
			print(prt, "  type=%c, modifier=%d(%c), op=%d(%c) btype=%d (%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op), n->val.subv.type, prop(n->val.subv.type));
			print_arith_int(prt, n->val.subv.value);
			break;
		case A_VALUE:
			switch(n->val.intv.type) {
			case LIT_DECIMAL:
				print(prt, "%d", n->val.intv.value);
				break;
			case LIT_OCTAL:
				print(prt, "%o", n->val.intv.value);
				break;
			case LIT_BINARY:
				// TODO: binary
				print(prt, "%o", n->val.intv.value);
				break;
			case LIT_HEX:
				print(prt, "$%x", n->val.intv.value);
				break;
			case LIT_OCTAL_C:
				print(prt, "0%o", n->val.intv.value);
				break;
			case LIT_HEX_C:
				print(prt, "0x%x", n->val.intv.value);
				break;
			case LIT_CHAR:
				print(prt, "'%c'", n->val.intv.value);
				break;
			case LIT_TWOCHAR:
				print(prt, "'%c%c'", n->val.intv.value & 0xff, (n->val.intv.value >> 8) & 0xff);
				break;
			case LIT_NONE:
				print(prt, "-");
				break;
			}
			break;
		case A_INDEX:
			print(prt, "  type=%c, modifier=%d (%c), op=%d(%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
			break;
		case A_LABEL:
			print(prt, "%s", n->val.lab.name);
			break;
		default:
			print(prt, "  UNHANDLED: type=%c, modifier=%d (%c), op=%d(%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
		}
	}
}


void print_formatted_stmt(printer_t *prt, const statement_t *stmt) {

	const print_config_t *cfg = prt->cfg;
	int l = 0;

	if (cfg->lineno) {
		if (stmt->lineno >= 0) {
			print(prt, "% 5d ", stmt->lineno);
		} else {
			print(prt, "      ");
		}
		l += 6;
	}

	if (stmt->label != NULL) {
		print(prt, "% -10s ", stmt->label->name);
	} else {
		print(prt, "           ");
	}
	l += 11;

	ilist_t *a = NULL;
	operation_t *o = NULL;
	switch(stmt->type) {
	case S_PCDEF:
		break;
	case S_LABEQPC:
		print(prt, "=*");
		break;
	case S_LABDEF:
		print(prt, "%s", tokenizer_op_details(stmt->assign)->print);
		a = stmt->param;
		print_arith_int(prt, a);
		break;
	case S_OPCODE:
		o = stmt->op;
		print(prt, "%s ", o->name);
		print(prt, "%s", op_syn_details(stmt->syn)->pre);
		if (stmt->param) {
			print_arith_int(prt, stmt->param);
		}
		print(prt, "%s", op_syn_details(stmt->syn)->post);
		break;
	case S_PSEUDO:
		break;
	case S_NONE:
		break;
	default:
		break;
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
		if (print_getlen(prt) > l) {
			print_setcol(prt, 30);
		}
		print(prt, "; %s", stmt->comment);
	}	

	if (print_getlen(prt) > l) {
		print_out(prt);
	} else {
		print_clr(prt);
	}
}


