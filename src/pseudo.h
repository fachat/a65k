/****************************************************************************

    Manage pseudo opcodes
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


#ifndef PSEUDO_H
#define PSEUDO_H


#include "err.h"
#include "tokenizer.h"

typedef struct statement_s statement_t;

typedef struct pseudo_s pseudo_t;

struct pseudo_s {
	const char	*name;
	err_t		(*parse)(const pseudo_t *pseudo, tokenizer_t *tok, statement_t *stmt);
	err_t		(*pass1)(const pseudo_t *pseudo, statement_t *stmt);
	err_t		(*pass2)(const pseudo_t *pseudo, statement_t *stmt);
};


// init the pseudo opcodes
void pseudo_module_init(void);

err_t parse_pseudo(tokenizer_t *tok, statement_t *stmt);


#endif


