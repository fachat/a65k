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
		if (n->modifier) {
			print(prt, PRT_PARAM, "%c", n->modifier);
		}
		if (n->op) {
			print(prt, PRT_PARAM, "%s", tokenizer_op_details(n->op)->print);
		}
		switch(n->type) {
		case A_BRACKET:
			//print(prt, PRT_PARAM, "  type=%c, modifier=%d(%c), op=%d(%c) btype=%d (%c)", 
			//	n->type, n->modifier, prop(n->modifier), n->op, prop(n->op), n->val.subv.type, prop(n->val.subv.type));
			print(prt, PRT_PARAM, "%s", tokenizer_op_details(n->val.subv.type));
			print_arith_int(prt, n->val.subv.value);
			print(prt, PRT_PARAM, "%s", tokenizer_op_details(closing_op(n->val.subv.type)));
			break;
		case A_VALUE:
			switch(n->val.intv.type) {
			case LIT_DECIMAL:
				print(prt, PRT_PARAM, "%d", n->val.intv.value);
				break;
			case LIT_OCTAL:
				print(prt, PRT_PARAM, "%o", n->val.intv.value);
				break;
			case LIT_BINARY:
				// TODO: binary
				print(prt, PRT_PARAM, "%o", n->val.intv.value);
				break;
			case LIT_HEX:
				print(prt, PRT_PARAM, "$%x", n->val.intv.value);
				break;
			case LIT_OCTAL_C:
				print(prt, PRT_PARAM, "0%o", n->val.intv.value);
				break;
			case LIT_HEX_C:
				print(prt, PRT_PARAM, "0x%x", n->val.intv.value);
				break;
			case LIT_CHAR:
				print(prt, PRT_PARAM, "'%c'", n->val.intv.value);
				break;
			case LIT_TWOCHAR:
				print(prt, PRT_PARAM, "'%c%c'", n->val.intv.value & 0xff, (n->val.intv.value >> 8) & 0xff);
				break;
			case LIT_NONE:
				print(prt, PRT_PARAM, "-");
				break;
			}
			break;
		case A_INDEX:
			// can be ignored, as already printed by .op above
			break;
		case A_LABEL:
			print(prt, PRT_PARAM, "%s", n->val.lab.name);
			break;
		case A_UNARY:
			print(prt, PRT_PARAM, "%s", tokenizer_op_details(n->val.unary.op)->print);
			print_arith_int(prt, n->val.unary.value);
			break;
		default:
			print(prt, PRT_PARAM, "  UNHANDLED: type=%c, modifier=%d (%c), op=%d(%c)", 
				n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
		}
	}
}


void print_formatted_stmt(printer_t *prt, const statement_t *stmt) {

	const print_config_t *cfg = prt->cfg;

	if (cfg->lineno) {
		if (stmt->lineno >= 0) {
			print(prt, PRT_LINENO, "%5d", stmt->lineno);
		}
	}

	if (stmt->label != NULL) {
		print(prt, PRT_LABEL, "% -10s", stmt->label->name);
	}

	ilist_t *a = NULL;
	const instruction_t *o = NULL;
	switch(stmt->type) {
	case S_PCDEF:
		break;
	case S_LABEQPC:
		print(prt, PRT_OPERATION, "=*");
		break;
	case S_LABDEF:
		print(prt, PRT_OPERATION, "%s", tokenizer_op_details(stmt->assign)->print);
		a = stmt->param;
		print_arith_int(prt, a);
		break;
	case S_OPCODE:
		o = stmt->op;
		print(prt, PRT_OPERATION, "%s", o->name);
		if (stmt->um_prefix || stmt->nf_prefix || stmt->rs_prefix || stmt->le_prefix) {
			print(prt, PRT_OPERATION, ".");
			if (stmt->um_prefix) {
				print(prt, PRT_OPERATION, "%c", prefix_um_char(stmt->um_prefix));
			}
			if (stmt->nf_prefix) {
				print(prt, PRT_OPERATION, "%c", prefix_nf_char(stmt->nf_prefix));
			}
			if (stmt->le_prefix) {
				print(prt, PRT_OPERATION, "%c", prefix_le_char(stmt->le_prefix));
			}
			if (stmt->rs_prefix) {
				print(prt, PRT_OPERATION, "%c", prefix_rs_char(stmt->rs_prefix));
			}
		}
		print(prt, PRT_PARAM, "%s", op_syn_details(stmt->syn)->pre);
		if (stmt->param) {
			print_arith_int(prt, stmt->param);
			if (stmt->base) {
				print(prt, PRT_PARAM, "%s", tokenizer_op_details(stmt->base)->print);
			}
		}
		print(prt, PRT_PARAM, "%s", op_syn_details(stmt->syn)->post);
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
		print(prt, PRT_COMMENT, "; %s", stmt->comment);
	}	

	print_out(prt);
}


