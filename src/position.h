/****************************************************************************

    file position
    Copyright (C) 2015 Andre Fachat

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


#ifndef POSITION_H
#define POSITION_H

#include "parser-config.h"


typedef struct {
        const char	        *filename;
        int                     lineno;
} position_t;

typedef struct {
        const char              *line;
        position_t              *position;
	const parser_config_t	*parsercfg;
} line_t;

#endif

