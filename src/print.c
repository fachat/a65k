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
#include <errno.h>

#include "print-config.h"
#include "print.h"
#include "arith.h"

// filler spaces
static const char spaces[] = "                                        ";

// column widths for fixed width output, index corresponds to printer_col_t
static const int widths[] = { 
	6, 	// PRT_LINENO
	5,	// PRT_ADDR
	9,	// PRT_DATA
	11,	// PRT_LABEL
	4,	// PRT_OPERATION
	20,	// PRT_PARAM
	30	// PRT_COMMENT
};

void print_module_init() {

	print_config_init();
}

static type_t printer_memtype = {
        "printer_t",
        sizeof(printer_t)
};

// initialize printer output channel
printer_t *print_init() {

	print_config_t *cfg = print_current_config();

	const char *filename = cfg->filename;
	FILE *f = NULL;
	if (filename != NULL && strlen(filename) > 0) {
		f = fopen(filename, "w");
		if (f == NULL) {
			log_error("Could not open output file '%s' -> %s", filename, strerror(errno));
			return NULL;
		}
	}

	printer_t *prt = mem_alloc(&printer_memtype);

	prt->outfile = f;
	prt->cfg = cfg;
	prt->current_col = PRT_LINENO;

	int p = 0;
	for (int i = PRT_LINENO; i < PRT_END; i++) {
		prt->col_width[i] = p;
		p += widths[i];
	}

	print_clr(prt);

	prt->was_empty = 0;

	return prt;
}

static int print_getlen(printer_t *prt) {
	return prt->buflen;
}

static void print_setcol(printer_t *prt, int col) {
	
	int buflen = print_getlen(prt);

	int todo = col - buflen;
	int slen = strlen(spaces);

	if (todo <= 0) {
		todo = 1;
	}

	while (todo > 0) {
		int step = slen - todo;
		if (step < 0) {
			step = 0;
		}
		print(prt, prt->current_col, "%s", spaces + step);
		todo -= slen + step;
	}
}


void print(printer_t *prt, print_col_t col_no, const char *pattern, ...) {

	if (col_no != prt->current_col) {
		// target fixed width column
		if (col_no == PRT_COMMENT && prt->current_col == PRT_LINENO) {
			// shift comment to left if only comment on line
			col_no = PRT_LABEL;
		}
		int trg = prt->col_width[col_no];
		print_setcol(prt, trg);
		prt->current_col = col_no;
	}

        va_list ap;
        va_start(ap, pattern);

	int buflen = print_getlen(prt);

        int r = vsnprintf(prt->buf + buflen, BUF_LEN - buflen, pattern, ap);
        if (r < 0 || r > BUF_LEN) {
                // error
                log_error("Error printing %d\n", r);
                return;
        }
	prt->buflen += r;
}

void print_out(printer_t *prt) {

	if (prt->buflen == 0) {
		if (prt->cfg->collapse_empty && prt->was_empty) {
			return;
		}
		prt->was_empty = 1;
	} else {
		prt->was_empty = 0;
	}

	if (prt->outfile == NULL) {
        	printf("%s\n", prt->buf);
	} else {
        	fprintf(prt->outfile, "%s\n", prt->buf);
	}
	print_clr(prt);
}

void print_clr(printer_t *prt) {
	prt->buf[0] = 0;
	prt->buflen = 0;
	prt->current_col = PRT_LINENO;
}

void print_close(printer_t *prt) {

	if (prt->outfile) {
		fclose(prt->outfile);
	}
}


