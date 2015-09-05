/****************************************************************************

    tokenizing a line
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


#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

#include "types.h"

typedef enum {
	T_INIT			=  0,	// initial state
	T_NAME			=  1,	// a name, like label1 do_this
	T_TOKEN			=  2,	// a token, like ( ) . ++ == <> & 
	T_STRING_LITERAL	= 10,	// a quoted string, like 'abc' "cde"
	T_DECIMAL_LITERAL	= 20,	// a decimal number, like 1 2 3
	T_OCTAL_LITERAL		= 28,	// an octal number, like &127 0127
	T_BINARY_LITERAL	= 22,	// a binary number, like %110101
	T_HEX_LITERAL		= 26,	// a hex number, like $12dA 0x12Ad
	T_ERROR			=254,	// an error has happened
	T_END			=255,	// end of string reached
} tok_t;

typedef enum {
	OP_EXCL 		= '!',	
	OP_AT			= '@',
	OP_XQUOTE		= '`',
	OP_OPEN			= '(',
	OP_CLOSE		= ')',
	OP_HASH			= '#',
	OP_BOPEN		= '[',
	OP_BCLOSE		= ']',
	OP_STAR			= '*',
	OP_COMMA		= ',',
	OP_PLUS			= '+',
	OP_MINUS		= '-',
	OP_DIV			= '/',
	OP_COLON		= ':',
	OP_SEMICOLON		= ';',
	OP_DOT			= '.',
	OP_LARGER		= '>',
	OP_LARGEROREQUAL	= 128,
	OP_LESS			= '<',
	OP_LESSOREQUAL		= 129,
	OP_EQUAL		= '=',
	OP_NOTEQUAL		= 130,
	OP_SHIFTLEFT		= 131,
	OP_SHIFTRIGHT		= 132,
	OP_LOGICAND		= 133,
	OP_LOGICOR		= 134,
	OP_LOGICXOR		= 135
} op_t;


typedef struct {
	const char 	*line;		// line to tokenize
	tok_t		type;		// type of token
	int 		ptr;		// current pointer to token in line
	int		len;		// length of current token (total, i.e. incl. quotes for a string)
	int		strptr;		// pointer to start of string for T_STRING_LITERAL
	int		strlen;		// length of string for T_STRING_LITERAL
	long		value;		// value for decimal, octal, binary, or hex literal
} tokenizer_r;

// initialize a tokenizer 
tokenizer_r *tokenizer_init(const char *line);

// set to next token; return true when there is a valid token
bool_t tokenizer_next(tokenizer_r *tok);

void tokenizer_free(tokenizer_r *tok);

#endif

