
/****************************************************************************

    segment management
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


#ifndef SEGMENT_H
#define SEGMENT_H

typedef struct segment_s segment_t;

#include "types.h"
#include "position.h"
#include "cpu.h"
#include "parser.h"
#include "list.h"

// default names for the xa65 default segments
// can the compiler optimize that into a single text?
// otherwise still use the preprocessor...
extern const char *SEG_ANY_NAME;
extern const char *SEG_BSS_NAME;
extern const char *SEG_TEXT_NAME;
extern const char *SEG_DATA_NAME;
extern const char *SEG_ZP_NAME;


typedef enum {
	SEG_ANY,	// "any" type of segment, like original 6502 assembler
	SEG_TEXT,	// code
	SEG_ZP,		// zeropage data
	SEG_DATA,	// data
	SEG_ZPBSS,	// zeropage bss
	SEG_BSS		// bss
} seg_type;

struct segment_s {
	const char 	*name;
	seg_type	type;
	bool_t		readonly;
	int		cpu_width;	// for 65k; segment.cpu_width >= context.cpu_width
	list_t		*statements;	// list of statements
};

void segment_module_init();

// create a new segment or find an existing, matching one; match is by name
segment_t *segment_new(const position_t *loc, const char *name, seg_type type, cpu_type cpu, bool_t readonly);

void segment_push_statement(segment_t *seg, statement_t *stmt);

// get all segments
list_iterator_t *segment_get_iter();

list_iterator_t *segment_get_statements(segment_t *seg);

#endif

