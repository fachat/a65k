/****************************************************************************

    print output
    Copyright (C) 2012 Andre Fachat

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


#include "print.h"

#define	BUF_LEN		2048

void print_debug_stmt(statement_t *stmt) {

	char buf[BUF_LEN];
	int p = 0;

	int r = snprintf(buf + p, BUF_LEN - p, "B:%p lab:%p op:%p par:%p", 
		(void*)stmt, (void*)stmt->label, (void*)stmt->op, (void*)stmt->param);
	if (r < 0 || (p + r) > BUF_LEN) {
		// error
		log_error("Error printing %d\n", r);
		return;
	}
	log_warn("%s", buf);

	if (stmt->label != NULL) {
		p = 0;
		const label_t *l = stmt->label;
		int r = snprintf(buf + p, BUF_LEN - p, "L:%p name:%s ctx:%p pos:%s:%d", 
			(void*)l, l->name, (void*)l->ctx, l->position->filename, l->position->lineno);
		if (r < 0 || (p + r) > BUF_LEN) {
			// error
			log_error("Error printing %d\n", r);
			return;
		}
		log_warn("%s", buf);
	}

	if (stmt->op != NULL) {
		p = 0;
		const operation_t *o = stmt->op;
		int r = snprintf(buf + p, BUF_LEN - p, "O:%p name:%s isa:$%x isrel:%d acw:%d idxw:%d", 
			(void*)o, o->name, o->isa, o->abs_is_rel, o->check_ac_w, o->check_idx_w);
		if (r < 0 || (p + r) > BUF_LEN) {
			// error
			log_error("Error printing %d\n", r);
			return;
		}
		log_warn("%s", buf);
	}

	if (stmt->param != NULL) {
		p = 0;
		const anode_t *a = stmt->param;
		int r = snprintf(buf + p, BUF_LEN - p, "A:%p type:%d op:%d val:$%lx/%ld", 
			(void*)a, a->type, a->op, a->val.intv.value, a->val.intv.value);
		if (r < 0 || (p + r) > BUF_LEN) {
			// error
			log_error("Error printing %d\n", r);
			return;
		}
		log_warn("%s", buf);
	}
	
}


//void print_canon_stmt(statement_t *stmt) {
//}


