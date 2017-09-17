/****************************************************************************

    printer config
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

#include <string.h>

#include "mem.h"
#include "err.h"
#include "print-config.h"
#include "cmdline.h"


static type_t print_config_memtype = {
        "print_config_t",
        sizeof(print_config_t)
};

static print_config_t pconfig;


static cmdline_t print_params[] = {
	{ "print-lineno", NULL, PARTYPE_FLAG, NULL, cmdline_set_flag, &pconfig.lineno, 
		"print (or leave space for) line numbers", NULL },
};

void print_config_init() {

	pconfig.lineno = 0;
	
	int n = sizeof(print_params) / sizeof(cmdline_t);
	cmdline_register_mult(print_params, n);
}

print_config_t* print_current_config() {

	print_config_t *clonedconf = mem_alloc(&print_config_memtype);

	memcpy(clonedconf, &pconfig, sizeof(pconfig));

	return clonedconf;
}

