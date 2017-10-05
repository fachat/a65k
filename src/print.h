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

// definitions for print output columns
typedef enum {
	PRT_LINENO		= 0,		// line number
	PRT_ADDR		= 1,		// address of line
	PRT_DATA		= 2,		// bytes generated by line
	PRT_LABEL		= 3,		// label definition
	PRT_OPERATION		= 4,		// operation
	PRT_PARAM		= 5,		// parameter
	PRT_COMMENT		= 6,		// comment
	PRT_END			= 7		// unused, defined for len
} print_col_t;

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
	// column widths for fixed column width
	print_col_t		current_col;
	int			col_width[PRT_END];
} printer_t;

// init print module
void print_module_init();

// initialize printer output channel
printer_t *print_init();

// general print; 
// Note: switching col_no automatically goes to this column, but
// only in ascending order. Printing with same col_no is allowed
// and simply appends.
void print(printer_t *ptr, print_col_t col_no, const char *pattern, ...);

// print a statement formatted
void print_formatted_stmt(printer_t *cfg, const statement_t *stmt);

int print_getlen(printer_t *prt);

void print_out(printer_t *prt);

void print_clr(printer_t *prt);

void print_close(printer_t *prt);

#endif

