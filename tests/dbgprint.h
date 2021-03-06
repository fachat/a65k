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


#ifndef DBGPRINT_H
#define DBGPRINT_H

#include "expr.h"
#include "parser.h"
#include "print-config.h"

void do_print(const char *pattern, ...);

void print_debug_stmt(const statement_t *stmt);

void print_debug_expr(const ilist_t *anodes);

void print_debug_eval(const eval_t *eval_result);

#endif

