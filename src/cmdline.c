
/****************************************************************************

    commandline parsing
    Copyright (C) 2016-2017 Andre Fachat

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

#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "hashmap.h"
#include "array_list.h"
#include "cmdline.h"
#include "err.h"
#include "infiles.h"
#include "mem.h"

// hash from param name to cmdline_t sruct for quick find
static hash_t *params = NULL;
// list of parameters for help output
static list_t *paramlist = NULL;

static const char *prg_name = NULL;

static err_t usage(int flag, void* extra) {
	
	(void) flag;
	(void) extra;

	printf("Usage: %s [options] in-filename1 [in-filename2 ...]\n"
		"Cross-assembler for 65xx/R65C02/65816/65CE02/65002\n"
		"\n", prg_name); 

	list_iterator_t *iter = list_iterator(paramlist);
	cmdline_t *param = NULL;
	while ( (param = list_iterator_next(iter)) ) {
		switch(param->type) {
		case PARTYPE_FLAG:
			printf("  -%s\n\t%s\n", param->name, param->description);
			break;
		case PARTYPE_PARAM:
			printf("  -%s=<value>\n\t%s\n", param->name, param->description);
			break;
		case PARTYPE_ENUM:
			printf("  -%s=<value>\n\t%s\n", param->name, param->description);
			param_enum_t *options = param->values();
			int i = 0;
			while (options[i].value) {
				printf("\t'%s'\t%s\n", options[i].value, options[i].description);
				i++;
			}
			break;
		}
	}
	
	return E_ABORT;
}

static const cmdline_t help[] = {
	{ "?", PARTYPE_FLAG, NULL, usage, NULL, "Show this help", NULL },
	{ "help", PARTYPE_FLAG, NULL, usage, NULL, "Show this help", NULL },
};

static type_t param_memtype = {
	"param_enum_t",
	sizeof(param_enum_t)
};

static const char *key_from_param(const void *entry) {
	return ((cmdline_t*)entry)->name;
}

// TODO init the cmdline parser with the name as which the program was called
// for example "a65k", but also "xa65"
void cmdline_module_init() {

	params = hash_init_stringkey(50, 25, key_from_param);
	paramlist = array_list_init(20);

	cmdline_register_mult(help, sizeof(help)/sizeof(cmdline_t));
}

// allocate an array of param_enum_t structs for use as param option
param_enum_t *cmdline_pval_alloc(int n) {

	return mem_alloc_n(n, &param_memtype);
}


// template method where extra_param is the pointer to an int variable to set
err_t cmdline_set_flag(int flag, void *extra_param) {
	*((int*)extra_param) = flag;
	return E_OK;
}

void cmdline_register_mult(const cmdline_t *param, int num ) {
	for (int i = 0; i < num; i++) {
		cmdline_register(&param[i]);
	}
}

void cmdline_register(const cmdline_t *param) {
	
	if (hash_put(params, (void*)param) != NULL) {
		// must not happen
		exit(1);
	}
	list_add(paramlist, (void*)param);
}

err_t cmdline_parse(int argc, char *argv[]) {

	err_t rv = E_OK;

	prg_name = argv[0];

        int i = 1;
        while (i < argc && !rv) {
                if (argv[i][0] == '-') {
			char *val = NULL;
			int flag = 1;
			const char *name = argv[i]+1;

			char *end = index(name, '=');
			if (end != NULL) {
				val = end + 1;
				end[0] = 0;
			}

			// lookup option
			cmdline_t *opt = hash_get(params, name);
			if (opt == NULL) {
				// check "no-" flag option
				if (name == strstr(name, "no-")) {
					name = name+3;
					flag = 0;
					opt = hash_get(params, name);
					if (opt != NULL && opt->type != PARTYPE_FLAG) {
						opt = NULL;
					}
				}
			}
			
			param_enum_t *values = NULL;
			if (opt != NULL) {
				switch (opt->type) {
				case PARTYPE_FLAG:
					rv = opt->setflag(flag, opt->extra_param);
					break;
				case PARTYPE_PARAM:
					// TODO: error if param is missing
					rv = opt->setfunc(val, opt->extra_param);
					break;
				case PARTYPE_ENUM:
					values = opt->values();
					int i = 0;
					while (values[i].value) {
						if (!strcmp(values[i].value, val)) {
							opt->setfunc(val, opt->extra_param);
							break;
						}
						i++;
					}
					if (!values[i].value) {
						// TODO error option value not found
					}
					break;
				}
			} else {
				// TODO error cmdline parameter not found
			}
                } else {
			// register file with snapshot (clone) of current parser configuration
                        infiles_register(argv[i], parser_current_config());
                }   
		i++;
        }   
	return rv;
}




