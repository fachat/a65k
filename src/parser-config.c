/****************************************************************************

    parser
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

#include <string.h>

#include "mem.h"
#include "err.h"
#include "parser-config.h"
#include "cmdline.h"


static type_t parser_config_memtype = {
        "parser_config_t",
        sizeof(parser_config_t)
};

static parser_config_t pconfig;

static err_t set_initial_binary(int flag, void *params) {
	(void) params;
	pconfig.initial_binary = flag;
	if (flag) {
		pconfig.initial_lineno = 0;
	}
	return E_OK;
}
static err_t set_initial_lineno(int flag, void *params) {
	(void) params;
	pconfig.initial_lineno = flag;
	if (flag) {
		pconfig.initial_binary = 0;
	}
	return E_OK;
}


static cmdline_t params[] = {
	{ "parse-initial-binary", PARTYPE_FLAG, NULL, set_initial_binary, NULL, 
		"parse address and hex data values before the actual label and operation "
		"(disables parse-initial-lineno)" },
	{ "parse-initial-lineno", PARTYPE_FLAG, NULL, set_initial_lineno, NULL, 
		"parse a BASIC-style line number before the actual label and operation " 
		"(disables parse-initial-binary)" },
	{ "parse-follow-includes", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.follow_includes, 
		"Follow include statements (default in assembler, off by default in lint mode)" },
	{ "parse-xa-preproc", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.xa_preprocessor, 
		"parse XA65 preprocessor statements (default off)" },
	{ "parse-convert-xapp", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.convert_xapp,
		"Convert XA65 preprocessor statements into standard pseudo-operations (lint mode only)" },
	{ "parse-colon-in-comments", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.colon_in_comments,
		"if set, allow colon in comments; by default colon starts new statement." },
	{ "parse-c-style", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.cstyle_allowed,
		"allow C-style hex and octal values (default off)" },
};

void parser_config_init() {

	pconfig.initial_binary = 0;
	pconfig.initial_lineno = 1;
	pconfig.follow_includes = 1;
	pconfig.xa_preprocessor = 0;
	pconfig.convert_xapp = 1;
	pconfig.colon_in_comments = 0;
	pconfig.cstyle_allowed = 0;	
	

	//printf("sizeof(params)=%ld\n", sizeof(params));

	int n = sizeof(params) / sizeof(cmdline_t);
	for (int i = 0; i < n; i++) {
		cmdline_register(&params[i]);
	}
}

parser_config_t* parser_current_config() {

	parser_config_t *clonedconf = mem_alloc(&parser_config_memtype);

	memcpy(clonedconf, &pconfig, sizeof(pconfig));

	return clonedconf;
}

