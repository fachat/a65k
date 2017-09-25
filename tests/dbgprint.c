/****************************************************************************

    print output
    Copyright (C) 2012,2017 Andre Fachat

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


static void print_debug_arith_int(const ilist_t *anodes, int indlen);

void do_print(const char *pattern, ...) {
	va_list ap;
	va_start(ap, pattern);

	char buf[BUF_LEN];

	int r = vsnprintf(buf, BUF_LEN, pattern, ap);
	if (r < 0 || r > BUF_LEN) {
		// error
		log_error("Error printing %d\n", r);
		return;
	}
	log_warn("%s", buf);
}


void print_debug_stmt(const statement_t *stmt) {

	do_print("--------------- STMT ---------------");

	if (stmt->lineno >= 0) {
		do_print("LINE: %d:", stmt->lineno);
	}
	do_print("BLK:%d ctx:%d (CPU: %s, isa:$%x), seg=%s", 
		stmt->blk->blockno, stmt->ctx->ctxno, stmt->ctx->cpu->name, stmt->ctx->cpu->isa,
		stmt->ctx->segment->name);

	if (stmt->label != NULL) {
		const label_t *l = stmt->label;
		do_print("LAB: name:%s ctx:%d pos:%s:%d", 
			l->name, l->ctx->ctxno, l->position->filename, l->position->lineno);
	}
	if (stmt->setlabel != NULL) {
		const ilist_t *a = stmt->setlabel;
		// TODO
		do_print("SET:");
		print_debug_arith_int(a, 4);
	}

	if (stmt->op != NULL) {
		const operation_t *o = stmt->op;
		do_print("OPR: name:%s isa:$%x isrel:%d acw:%d idxw:%d", 
			o->name, o->isa, o->abs_is_rel, o->check_ac_w, o->check_idx_w);

		do_print("MOD: BASE: %d, SYN: %d, UM: %d, NF: %d, RS: %d, LE: %d", 
			stmt->base, stmt->syn, stmt->um_prefix, stmt->nf_prefix, stmt->rs_prefix, stmt->le_prefix);
	}
	if (stmt->type == S_LABDEF) {
		do_print("LDEF: %s", tokenizer_op_details(stmt->assign)->print);
	}
	if (stmt->type == S_PCDEF) {
		do_print("PCDEF: %s", tokenizer_op_details(stmt->assign)->print);
	}
	if (stmt->type == S_LABEQPC) {
		do_print("LABEQPC: %s", "=*");
	}

	if (stmt->param != NULL) {
		const ilist_t *a = stmt->param;
		do_print("PAR: ");
		print_debug_arith_int(a, 4);
	}

	if (stmt->pseudo) {
		do_print("PSEUDO: %s", stmt->pseudo->name);

		if (stmt->pparams) {
			do_print("PARS:");
			list_iterator_t *iter = list_iterator(stmt->pparams);
			while (list_iterator_has_next(iter)) {
				ilist_t *parlist = list_iterator_next(iter);
				print_debug_arith_int(parlist, 4);
			}
		}
	}

	if (stmt->comment) {
		do_print("COMMENT: %s", stmt->comment);
	}	
}

static inline char prop(int o) {  return isprint(o) ? o : ' '; }
static inline int max(int a, int b) { return (a > b) ? a : b; }
static const char* spaces = "                                                                         ";

static void print_debug_arith_int(const ilist_t *anodes, int indlen) {
	const char* indent = spaces + max(0, strlen(spaces) - indlen);

	do_print("%sA:len=%d", indent, anodes->len);
	for (int i = 0; i < anodes->len; i++) {
		const anode_t *n = ilist_get(anodes, i);
		switch(n->type) {
		case A_BRACKET:
			do_print("  %stype=%c, modifier=%d(%c), op=%d(%c) btype=%d (%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op), n->val.subv.type, prop(n->val.subv.type));
			print_debug_arith_int(n->val.subv.value, indlen + 2);
			break;
		case A_VALUE:
			do_print("  %stype=%c, modifier=%d (%c), op=%d(%c), val=(%c)%d/%x", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op),
				n->val.intv.type, n->val.intv.value, n->val.intv.value);
			break;
		case A_INDEX:
			do_print("  %stype=%c, modifier=%d (%c), op=%d(%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
			break;
		case A_LABEL:
			do_print("  %stype=%c, modifier=%d (%c), op=%d(%c), label=%s (resolved=%d)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op),
				n->val.lab.name, n->val.lab.label ? 1 : 0);
			break;
		case A_UNARY:
			do_print("  %stype=%c, modifier=%d(%c), op=%d(%c), unary=%d(%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op),
				n->val.unary.op, prop(n->val.unary.op));
			print_debug_arith_int(n->val.unary.value, indlen + 2);
			break;
		default:
			do_print("  UNHANDLED: %stype=%c, modifier=%d (%c), op=%d(%c)", 
				indent, n->type, n->modifier, prop(n->modifier), n->op, prop(n->op));
		}
	}
}

void print_debug_arith(const ilist_t *anodes) {

	print_debug_arith_int(anodes, 2);

}



