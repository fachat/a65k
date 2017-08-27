
/****************************************************************************

    commandline parsing
    Copyright (C) 2016 Andre Fachat

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


#ifndef CMDLINE_H
#define CMDLINE_H

#include "err.h"

// init the cmdline parser with the name as which the program was called
// for example "a65k", but also "xa65"
void cmdline_module_init(const char *prgname);

// parse the options
err_t cmdline_parse(int argc, char *argv[]);

typedef struct {
	// name of cmdline param
	const char 	*name;		

	// option has a parameter
	const int	need_arg;	

	// function to call when parsed
	// value will be NULL when no value provided
	err_t 		(*setfunc)(const char *value, void *extra_param);

	// extra param to be passed to set function
	void		*extra_param;
} cmdline_t;

void cmdline_register(const cmdline_t *param);

#endif

