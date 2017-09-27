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


static inline char prop(int o) {  return isprint(o) ? o : ' '; }
static inline int max(int a, int b) { return (a > b) ? a : b; }

static void print_arith_int(const ilist_t *anodes) {

	for (int i = 0; i < anodes->len; i++) {
		const anode_t *n = ilist_get(anodes, i);
		switch(n->type) {
		case A_BRACKET:
			print("  type=%c, modifier=%d(%c), op=%d(%c) btype=%d (%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op), n->val.subv.type, prop(n->val.subv.type));
			print_arith_int(n->val.subv.value);
			break;
		case A_VALUE:
			switch(n->val.intv.type) {
			case LIT_DECIMAL:
				print("%d", n->val.intv.value);
				break;
			case LIT_OCTAL:
				print("%o", n->val.intv.value);
				break;
			case LIT_BINARY:
				// TODO: binary
				print("%o", n->val.intv.value);
				break;
			case LIT_HEX:
				print("$%x", n->val.intv.value);
				break;
			case LIT_OCTAL_C:
				print("0%o", n->val.intv.value);
				break;
			case LIT_HEX_C:
				print("0x%x", n->val.intv.value);
				break;
			case LIT_CHAR:
				print("'%c'", n->val.intv.value);
				break;
			case LIT_TWOCHAR:
				print("'%c%c'", n->val.intv.value & 0xff, (n->val.intv.value >> 8) & 0xff);
				break;
			case LIT_NONE:
				print("-");
				break;
			}
			break;
		case A_INDEX:
			print("  type=%c, modifier=%d (%c), op=%d(%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
			break;
		case A_LABEL:
			print("%s", n->val.lab.name);
			break;
		default:
			print("  UNHANDLED: type=%c, modifier=%d (%c), op=%d(%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
		}
	}
}


void print_formatted_stmt(const statement_t *stmt, const print_config_t *cfg) {

	int l = 0;

	if (cfg->lineno) {
		if (stmt->lineno >= 0) {
			print("% 5d ", stmt->lineno);
		} else {
			print("      ");
		}
		l += 6;
	}

	if (stmt->label != NULL) {
		print("% -10s ", stmt->label->name);
	} else {
		print("           ");
	}
	l += 11;

	ilist_t *a = NULL;
	operation_t *o = NULL;
	switch(stmt->type) {
	case S_PCDEF:
		break;
	case S_LABEQPC:
		print("=*");
		break;
	case S_LABDEF:
		print("%s", tokenizer_op_details(stmt->assign)->print);
		a = stmt->param;
		print_arith_int(a);
		break;
	case S_OPCODE:
		o = stmt->op;
		print("%s ", o->name);
		print("%s", op_syn_details(stmt->syn)->pre);
		if (stmt->param) {
			print_arith_int(stmt->param);
		}
		print("%s", op_syn_details(stmt->syn)->post);
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
		if (print_getlen() > l) {
			print_setcol(30);
		}
		print("; %s", stmt->comment);
	}	

	if (print_getlen() > l) {
		print_out();
	} else {
		print_clr();
	}
}


