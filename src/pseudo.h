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

typedef struct {
	const char	*name;
	err_t		(*parse)(tokenizer_t *tok, statement_t *stmt);
	err_t		(*pass1)(statement_t *stmt);
	err_t		(*pass2)(statement_t *stmt);
} pseudo_t;


// init the pseudo opcodes
void pseudo_module_init(void);

pseudo_t *find_pseudo(tokenizer_t *tok);


#endif


