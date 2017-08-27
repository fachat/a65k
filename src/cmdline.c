
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
#include "cmdline.h"
#include "err.h"
#include "infiles.h"

static hash_t *params = NULL;

static const char *key_from_param(const void *entry) {
	return ((cmdline_t*)entry)->name;
}

// init the cmdline parser with the name as which the program was called
// for example "a65k", but also "xa65"
void cmdline_module_init() {

	params = hash_init_stringkey(50, 25, key_from_param);
}

// template method where extra_param is the pointer to an int variable to set
void cmdline_set_flag(int flag, void *extra_param) {
	*((int*)extra_param) = flag;
}

void cmdline_register(const cmdline_t *param) {
	
	if (hash_put(params, (void*)param) != NULL) {
		// must not happen
		exit(1);
	}
}

err_t cmdline_parse(int argc, char *argv[]) {

	err_t rv = E_OK;

        int i = 1;
        while (i < argc && !rv) {
                if (argv[i][0] == '-') {
			char *val = NULL;
			int flag = 1;
			const char *name = argv[i]+1;
			if (name == strstr(name, "no-")) {
				name = name+3;
				flag = 0;
			}
			char *end = index(name, '=');
			if (end != NULL) {
				val = end + 1;
				end[0] = 0;
			}
			
			// lookup option
			cmdline_t *opt = hash_get(params, name);
			if (opt != NULL) {
				switch (opt->type) {
				case PARTYPE_FLAG:
					opt->setflag(flag, opt->extra_param);
					break;
				case PARTYPE_PARAM:
					// TODO: error if param is missing
					// TODO: if started with "no-" return error
					rv = opt->setfunc(val, opt->extra_param);
					break;
				}
			} else {
				// TODO error cmdline parameter not found
				// also check on no-* negative flags
			}
                } else {
			// TODO snapshot parser configuration for each file before each file
                        infiles_register(argv[i]);
                }   
		i++;
        }   
	return rv;
}




