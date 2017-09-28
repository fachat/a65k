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


#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

#include "parser.h"
#include "print-config.h"

#define	BUF_LEN		4096

typedef struct {
	FILE			*outfile;
	const char		*filename;
	const print_config_t	*cfg;
	// output buffer
	char			buf[BUF_LEN];
	int			buflen;
	// runtime flags
	// last line was empty
	int			was_empty;
} printer_t;

// init print module
void print_module_init();

// initialize printer output channel
printer_t *print_init();

// general print
void print(printer_t *ptr, const char *pattern, ...);

// print a statement formatted
void print_formatted_stmt(printer_t *cfg, const statement_t *stmt);

int print_getlen(printer_t *prt);

void print_setcol(printer_t *prt, int col);

void print_out(printer_t *prt);

void print_clr(printer_t *prt);

void print_close(printer_t *prt);

#endif

