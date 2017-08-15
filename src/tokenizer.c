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

// this code simply tokenizes the input string given some simple basic rules.

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"
#include "mem.h"
#include "err.h"
#include "tokenizer.h"


static type_t tokenizer_memtype = {
	"tokenizer_t",
	sizeof(tokenizer_t)
};

static op_details_t *sorted[256];

static op_details_t ops[] = {
        { 	OP_EXCL,		1,	0,	0	},
        {	OP_AT,			1,	0,	0	},
        {	OP_XQUOTE,		0,	0,	0	},
        {	OP_SQUOTE,		0,	0,	0	},
        {	OP_OPEN,		0,	0,	0	},
        {	OP_CLOSE,		0,	0,	0	},
        {	OP_HASH, 		0,	0,	0	},
        {	OP_BOPEN,		0,	0,	0	},
        {	OP_BBOPEN,		0,	0,	0	},
        {	OP_BCLOSE,		0,	0,	0	},
        {	OP_BBCLOSE,		0,	0,	0	}, 
        {	OP_STAR,		0,	0,	0	},
        {	OP_COMMA,		0,	0,	0	},
        {	OP_PLUS,		0,	0,	0	},
        {	OP_MINUS,		0,	1,	1	},
        {	OP_DIV,			0,	0,	1	},
        {	OP_COLON,		0,	0,	0	},
        {	OP_SEMICOLON,		0,	0,	0	},
        {	OP_DOT,			0,	0,	0	},
        {	OP_LARGER,		1,	0,	1	},
        {	OP_LARGEROREQUAL, 	1,	0,	1	},
        {	OP_LESS,		1,	0,	1	},
        {	OP_LESSOREQUAL,		0,	0,	1	},
        {	OP_ASSIGN,		0,	0,	0	}, 
        {	OP_ASSIGNADD,		0,	0,	0	},
        {	OP_ASSIGNSUB,		0,	0,	0	},
        {	OP_ASSIGNMULT,		0,	0,	0	},
        {	OP_ASSIGNDIV,		0,	0,	0	},
        {	OP_ASSIGNMOD,		0,	0,	0	},
        {	OP_ASSIGNBITOR,		0,	0,	0	},
        {	OP_ASSIGNBITAND,	0,	0,	0	},
        {	OP_ASSIGNBITXOR,	0,	0,	0	},
        {	OP_ASSIGNSHIFTLEFT,	0,	0,	0	},
        {	OP_ASSIGNSHIFTRIGHT,	0,	0,	0	},
        {	OP_BITXOR,		1,	0,	1	},
        {	OP_BITOR,		0,	0,	1	},
        {	OP_BITAND,		0,	0,	1	},
        {	OP_EQUAL,		0,	0,	1	},
        {	OP_NOTEQUAL,		0,	0,	1	},
        {	OP_SHIFTLEFT,		0,	0,	1	},
        {	OP_SHIFTRIGHT,		0,	0,	1	},
        {	OP_LOGICAND,		0,	0,	1	},
        {	OP_LOGICOR,		0,	0,	1	},
        {	OP_LOGICXOR,		0,	0,	1	},
        {	OP_BITINV,		1,	0,	1	},
        {	OP_MOD,			1,	0,	1	},
        {	OP_DOUBLESLASH,		0,	0,	0	},
        {	OP_XIND,		0,	0,	0	},
        {	OP_YIND,		0,	0,	0	},
        {	OP_ZIND,		0,	0,	0	},
        {	OP_SIND,		0,	0,	0	},
        {	OP_BIND,		0,	0,	0	}
};

void tokenizer_module_init(void) {

	memset(sorted, 0, 256 * sizeof(op_details_t*));

	for (unsigned int i = 0; i < sizeof(ops); i++) {
		op_details_t *p = &ops[i];	

		sorted[p->type] = p;
	}
}

// initialize a tokenizer 
tokenizer_t *tokenizer_create(const char *line) {
	
	tokenizer_t *tok = mem_alloc(&tokenizer_memtype);

	tok->line = line;
	tok->ptr = 0;
	tok->len = 0;
	tok->type = T_INIT;

	return tok;
}

static bool_t parse_base(tokenizer_t *tok, int ptr, int base) {

	int limit = base - 10;
	const char *line = tok->line;
	signed char c = 0;

	long value = 0;
	while (line[ptr] != 0) {
		if (isdigit(c = tok->line[ptr])) {
			c &= 0x0f;
			if (c > (base-1)) {
				// TODO: continue till end of number, so simple typo still allows detect further errors
				tok->vals.errno = E_TOK_DIGITRANGE;
				tok->type = T_ERROR;
				return false;
			}
			value = value * base + c;
		} else 
		if (base > 10) {
			c = tolower(c) - 'a';
		
			// c is char, which is unsigned, so no need to compare for <0
			if (c < limit) {
				value = value * base + c;
			} else {
				break;
			}
		} else {
			break;
		}
		ptr++;
	}
	tok->vals.literal.value = value;
	tok->len = ptr - tok->ptr;

	if (tok->len == 0) {
		// syntax error
		tok->vals.errno = E_TOK_EMPTY;
		tok->type = T_ERROR;
		return false;
	}
	return true;
}

static inline bool_t parse_decimal(tokenizer_t *tok, int ptr) {

	tok->type = T_LITERAL;
	tok->vals.literal.type = LIT_DECIMAL;
	return parse_base(tok, ptr, 10);
}

static inline bool_t parse_octal(tokenizer_t *tok, int ptr) {

	tok->type = T_LITERAL;
	tok->vals.literal.type = LIT_OCTAL;
	return parse_base(tok, ptr, 8);
}

static inline bool_t parse_binary(tokenizer_t *tok, int ptr) {

	tok->type = T_LITERAL;
	tok->vals.literal.type = LIT_BINARY;
	return parse_base(tok, ptr, 2);
}

static inline bool_t parse_hex(tokenizer_t *tok, int ptr) {

	tok->type = T_LITERAL;
	tok->vals.literal.type = LIT_HEX;
	return parse_base(tok, ptr, 16);
}

static inline bool_t parse_string(tokenizer_t *tok, int ptr) {

	const char *line = tok->line;

	char delim = line[ptr];
	tok->type = T_STRING;
	tok->vals.string.type = delim;

	ptr++;
	tok->vals.string.ptr = ptr;
	tok->vals.string.len = 0;

	while (isPrint(line[ptr])) {
		if (line[ptr] == delim) {
			break;
		}
		ptr++;
	}
	tok->vals.string.len = ptr - tok->vals.string.ptr;
	
	if (line[ptr] == delim) {
		ptr++;
	} else 
	if (line[ptr] != 0) {
		// non-printable char ends string
		tok->vals.errno = E_TOK_NONPRINT;
		tok->type = T_ERROR;
		return false;
	}

	tok->len = ptr - tok->ptr;

	if (tok->vals.string.len == 0) {
		// empty string
		tok->vals.errno = E_TOK_EMPTY;
		tok->type = T_ERROR;
		return false;
	}
	return true;
}

static inline bool_t parse_name(tokenizer_t *tok, int ptr) {

	const char *line = tok->line;
	tok->type = T_NAME;

	while (isalnum(line[ptr]) || (line[ptr] == '_')) {
		ptr++;
	}

	tok->len = ptr - tok->ptr;

	return true;
}

static inline bool_t parse_token(tokenizer_t *tok, int ptr, int can_have_operator, int allow_index) {

	const char *line = tok->line;

	char c = line[ptr];

	// default outcome
	tok->vals.op = c;
	tok->type = T_TOKEN;
	tok->len = 1;

	switch(c) {
	case '@':
	case '`':
	case '\'':
	case '#':
	case ':':
	case '.':
	case ';':
		return true;
	case '(':
	case ')':
		tok->type = T_BRACKET;
		return true;
	case ',':
		// TODO: check OP_*IND
		// using allow_index
		return true;
	case '%':
		if (line[ptr+1] == '=') {
			tok->vals.op = OP_ASSIGNMOD;
			tok->len++;
			return true;
		}
		return true;
	case '-':
		if (line[ptr+1] == '=') {
			tok->vals.op = OP_ASSIGNSUB;
			tok->len++;
			return true;
		}
		return true;
	case '+':
		if (line[ptr+1] == '=') {
			tok->vals.op = OP_ASSIGNADD;
			tok->len++;
			return true;
		}
		return true;
	case '*':
		if (line[ptr+1] == '=') {
			tok->vals.op = OP_ASSIGNMULT;
			tok->len++;
			return true;
		}
		return true;
	case '/':
		switch (line[ptr+1]) {
		case '=':
			tok->vals.op = OP_ASSIGNDIV;
			tok->len++;
			return true;
		case '/':
			tok->vals.op = OP_DOUBLESLASH;
			tok->len++;
			return true;
		}
		return true;
	case ']':
		tok->type = T_BRACKET;
		if (line[ptr+1] == ']') {
			tok->vals.op = OP_BBCLOSE;
			tok->len++;
			return true;
		}
		// OP_BOPEN
		return true;
	case '[':
		tok->type = T_BRACKET;
		if (line[ptr+1] == '[') {
			tok->vals.op = OP_BBOPEN;
			tok->len++;
			return true;
		}
		// OP_BOPEN
		return true;
	case '!':
		if (can_have_operator) {
			switch (line[ptr+1]) {
			case '=':
				tok->vals.op = OP_NOTEQUAL;
				tok->len++;
				return true;
			default:
				// default
				// OP_EXCL
				return true;
			}
		} else {
			// OP_EXCL
			return true;
		}
	case '>':
		if (can_have_operator) {
			// check ">>", ">=", "><"
			switch (line[ptr+1]) {
			case '>':
				if (line[ptr+2] == '=') {
					tok->vals.op = OP_ASSIGNSHIFTRIGHT;
					tok->len += 2;
					return true;
				}
				tok->vals.op = OP_SHIFTRIGHT;
				tok->len++;
				return true;
			case '=':
				tok->vals.op = OP_LARGEROREQUAL;
				tok->len++;
				return true;
			case '<':
				tok->vals.op = OP_NOTEQUAL;
				tok->len++;
				return true;
			default:
				// default
				return true;
			}
		} else {
			// ">" for high byte modifier or larger than
			return true;
		}
	case '<':
		if (can_have_operator) {
			// check "<<", "<=", "<>"
			switch (line[ptr+1]) {
			case '<':
				if (line[ptr+2] == '=') {
					tok->vals.op = OP_ASSIGNSHIFTLEFT;
					tok->len += 2;
					return true;
				}
				tok->vals.op = OP_SHIFTLEFT;
				tok->len++;
				return true;
			case '=':
				tok->vals.op = OP_LESSOREQUAL;
				tok->len++;
				return true;
			case '>':
				tok->vals.op = OP_NOTEQUAL;
				tok->len++;
				return true;
			default:
				// default
				return true;
			}
		} else {
			// "<" for high byte modifier
			return true;
		}
	case '=':
		if (can_have_operator) {
			// check "==", "=>", "=<"
			switch (line[ptr+1]) {
			case '=':
				tok->vals.op = OP_EQUAL;
				tok->len++;
				return true;
			default:
				// default, assign
				return true;
			}
		} else {
			// '=' assign
			return true;
		}
	case '&':
		// check "&", "&&", "&="
		switch (line[ptr+1]) {
		case '&':
			tok->vals.op = OP_LOGICAND;
			tok->len++;
			return true;
		case '=':
			tok->vals.op = OP_ASSIGNBITAND;
			tok->len++;
			return true;
		} 
		return true;
	case '|':
		// check "|", "||", "|="
		switch (line[ptr+1]) {
		case '|':
			tok->vals.op = OP_LOGICOR;
			tok->len++;
			return true;
		case '=':
			tok->vals.op = OP_ASSIGNBITOR;
			tok->len++;
			return true;
		} 
		return true;
	case '^':
		// check "^", "^^", "^="
		switch (line[ptr+1]) {
		case '^':
			tok->vals.op = OP_LOGICXOR;
			tok->len++;
			return true;
		case '=':
			tok->vals.op = OP_ASSIGNBITXOR;
			tok->len++;
			return true;
		} 
		return true;
	default:
		tok->vals.errno = E_TOK_UNKNOWN;
		tok->type = T_ERROR;
		return false;
	}

	return true;
}

// set to next token; return true when there is a valid token
bool_t tokenizer_next(tokenizer_t *tok, int allow_index) {

	// move behind last token
	int ptr = tok->ptr + tok->len;

	const char *line = tok->line;

	// attempt to not have to store an extra operand/operator flag in the tokenizer	
	// or even have to pass it to this function as parameter...
	bool_t can_have_operator = (tok->type == T_LITERAL) 
			|| (tok->type == T_TOKEN && line[ptr] == ')');

	// skip whitespace
	while (line[ptr] != 0 && isspace(line[ptr])) {
		ptr++;
	}

	tok->ptr = ptr;

	//printf("ptr=%d, c=%d\n", ptr, line[ptr]);

	// are we done with the line yet?
	if (line[ptr] == 0 || tok->type == T_END || tok->type == T_ERROR) {
		tok->len = 0;
		tok->type = T_END;
		return false;
	}

	char c = line[ptr];

	if (isdigit(c)) {
		// handle octal (starting with '0'), hex (starting with '0x'), dec
		if (c == '0') {
			if (line[ptr+1] == 'x' || line[ptr+1] == 'X') {
				// hex
				ptr += 2;
				tok->ptr = ptr;
				return parse_hex(tok, ptr);
			} else {
				// octal
				return parse_octal(tok, ptr);
			}
		} else {
			// dec
			return parse_decimal(tok, ptr);
		}
	} else
	if ((!can_have_operator) && (c == '%')) {
		// binary
		ptr++;
		tok->ptr = ptr;
		return parse_binary(tok, ptr);
	} else
	if ((!can_have_operator) && (c == '$')) {
		// hex
		ptr++;
		tok->ptr = ptr;
		return parse_hex(tok, ptr);
	} else
	if ((!can_have_operator) && (c == '&')) {
		// octal
		ptr++;
		tok->ptr = ptr;
		return parse_octal(tok, ptr);
	} else
	if (is_string_delim(c)) {
		// string literal
		// c must be of one of quotetype_t enum!
		return parse_string(tok, ptr);
	} else
	if (isalpha(c) || c == '_') {
		// name
		return parse_name(tok, ptr);
	} else
	if (ispunct(c)) {
		// any other token
		return parse_token(tok, ptr, can_have_operator, allow_index);
	} else {
		// non-printable - error
		tok->vals.errno = E_TOK_UNKNOWN;
		tok->type = T_ERROR;
		return false;
	}
	
}


void tokenizer_free(tokenizer_t *tok) {
	
	tok->line = NULL;

	mem_free(tok);
}



