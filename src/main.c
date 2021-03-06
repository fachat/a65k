/****************************************************************************

    65k processor assembler
    Copyright (C) 2012-2017 Andre Fachat

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

/*
 * main program
 */


#include "main.h"
#include "mem.h"
#include "log.h"
#include "position.h"
#include "config.h"
#include "cpu.h"
#include "segment.h"
#include "context.h"
#include "infiles.h"
#include "parser.h"
#include "instruction.h"
#include "cmdline.h"
#include "print.h"
#include "tokenizer.h"
#include "err.h"


static bool_t is_lint = false;

static err_t main_set_mode(const char *value, void *extra, int ival) {
	(void) extra;

	is_lint = ival == 1;

	printf("set mode: %s -> %d\n", value, ival);

	return E_OK;
}

static param_enum_t modes[] = {
	{ "ass", 	"Assembler mode (default)" },
	{ "lint",	"Just format the code and print it on stdout" },
	{ NULL },
};

static param_enum_t *main_get_modes() {
	return modes;
}

static cmdline_t main_options[] = {
	{ "mode", 	NULL, 	PARTYPE_ENUM, 	main_set_mode, 	NULL, NULL, 
		"Set the operation mode:", main_get_modes },
};


/**
 * print prints out the structures 
 */
static void print_output() {

	printer_t *prt = print_init();

	if (prt != NULL) {
		list_iterator_t *stmts = parser_get_statements();
		while (list_iterator_has_next(stmts)) {

			statement_t *stmt = list_iterator_next(stmts);	

			print_formatted_stmt(prt, stmt);
		}
	}
}

/**
 * pass1 pulls in lines from the infiles component,
 * and pushes them through the next steps
 */
static err_t parse() {

	err_t rv = E_OK;
	int err_cnt = 10;

	const cpu_t *cpu = cpu_by_name(config()->initial_cpu_name);
	segment_t *segment = segment_new(NULL, "_initial", SEG_ANY, cpu->type, false);
	context_init(segment, cpu);

	line_t *line;

	while (rv != E_END && (rv == E_OK || err_cnt > 0)) {
	
		line = infiles_readline();

		err_t rvx = parser_push(context(), line);

		if (rvx != E_OK && rvx != E_END) {
			err_cnt--;
		}
		if (rvx != E_OK) {
			rv = rvx;
		}
	}
	return rv;
}


static void main_init() {

	// memory handling
	mem_module_init();
	// configuration
	config_module_init();
	// cmdline
	cmdline_module_init();

	// register command line options
	cmdline_register_mult(main_options, sizeof(main_options)/sizeof(cmdline_t));

	// cpu info
	cpu_module_init();

	// input files
	infiles_module_init();
	// instructions
	instruction_module_init();
	// segments
	segment_module_init();
	// pseudo opcodes
	pseudo_module_init();
	// parser
	parser_module_init();
	// print output
	print_module_init();
	// tokenizer
	tokenizer_module_init();

}

int main(int argc, char *argv[]) {

	// initialize modules

	main_init();

	// parse command line parameters
	err_t e;
	if ( (e = cmdline_parse(argc, argv)) ) {
		return e;
	} 

	// parse files into AST
	parse();

	// first pass

	// second pass

	// print output
	//debug_output();
	print_output();
}

