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


#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H


typedef struct {
	// parse address and bytes (disables initial_lineno, default off)
	int	initial_binary;	

	// parse initial line number (default, disables initial_binary)
	int	initial_lineno;

	// do the actual include (default in assembler, off in lint mode)
	int	follow_includes;

	// parse xa preprocessor (default off)
	int	xa_preprocessor;

	// convert xa pp to standard pseudo-ops (only in lint mode)
	int	convert_xapp;

	// allow colons in comments (instead of starting a new statement)
	int	colon_in_comments;

	// allow C-style hex and octal
	int	cstyle_allowed;

} parser_config_t;

void parser_config_init(); 

// return a clone of the current configuration
parser_config_t *parser_current_config();

#endif

