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


static cmdline_t params[] = {
	{ "parse-initial-binary", PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.initial_binary, 
		"if set, allow parsing (and ignoring) address and hex data values before the actual label and operation" },
};

void parser_config_init() {

	pconfig.initial_binary = 0;
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

