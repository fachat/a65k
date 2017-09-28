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


#ifndef PRINT_CONFIG_H
#define PRINT_CONFIG_H


typedef struct {
	// output filename
	const char 	*filename;
	// print the line number, or reserve space for it if not parsed
	int 		lineno;
	// if true (default) collapse multiple empty lines into a single empty line
	int 		collapse_empty;
} print_config_t;

void print_config_init(); 

// return a clone of the current configuration
print_config_t *print_current_config();

#endif

