
/****************************************************************************

    arithmetic operations and expressions
    Copyright (C) 2015,2017 Andre Fachat

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


#ifndef ARITH_H
#define ARITH_H

#include "inline_list.h"
#include "label.h"
#include "block.h"
#include "err.h"

typedef signed long maxval_t;

/*
The anode_t struct is a node in an arithmetic expression
It is linked together in a tree-structure like such:

	(foo + 3) << 2, "foo", -1

	node0(A_BRACKET, AB_RND, OP_SHIFTLEFT), 
          |  |    
          |  |    
	  |  |
	  |  +-(value)->node1(A_LABEL, OP_ADD),
          |             nodw2(A_VALUE, AV_DEC, OP_NONE)
          |
        node3(A_VALUE, AV_DEC, OP_END)

	node4(A_VALUE, AB_STRD, OP_END)
	 
	node5(MOD_NEG, A_VALUE, AV_DEC)

Note: comma-separated values are only allowed in special
pseudo-opcodes and are handled as lists of values there.

The parser result also is a list. Only where brackets are
involved, there are sublists.
*/

// we might as well make our constants printable..
typedef enum {
	A_NONE		='-',	// placeholder
	A_BRACKET	='b',	// open bracket
	A_VALUE		='v',	// value
	A_STRING	='s',	// string
	A_LABEL		='l',	// label reference
	A_INDEX		='i',	// index, ",x"
	A_UNARY		='u',	// unary operator
} a_type;

typedef enum {
	// A_BRACKET subtypes (not necessarily all are used)
	AB_NONE		= 0,
	AB_RND		=OP_OPEN,	// (
	AB_ANG		=OP_LESS,	// <
	AB_RCT		=OP_BOPEN,	// [
	AB_CRV		=OP_COPEN,	// {
	AB_DBLRND	=OP_RROPEN,	// ((
	AB_DBLRCT	=OP_BBOPEN,	// [[
} asub_type;

typedef enum {
	AMOD_NONE 	=0,		// none
	AMOD_LOW	=OP_LESS,	// low byte selector (bits 0-7)
	AMOD_HIGH	=OP_LARGER,	// high byte selector (bits 8-15)
} amod_type;

/*
 The anode struct allows building the AST for an arithmetic expression.
*/
typedef struct anode_s anode_t;

struct anode_s {
	// type, incl. brackets, arithm. ops etc
	a_type		type;
	// modifier
	amod_type	modifier;
	// operation
	op_t		op;
	// value (depending on a_type)
	union {
	  // A_VALUE
	  struct {
	    maxval_t 	value;
	    littype_t	type;	// note: maybe LIT_NONE in case of stacked unaries
	  } intv;
	  // A_STRING
	  struct {
	    const char 	*value;
	    int 	len;
	    quotetype_t	type;
	  } strv;
	  // A_LABEL
	  struct {
	    const char 	*name;
	    label_t 	*label;
	  } lab;
	  // A_BRACKET
	  struct {
	    ilist_t *value;
	    asub_type	type;
	  } subv;
	  // A_UNARY
	  struct {
	    ilist_t *value;
	    op_t op;
	  } unary;
	} val;
};


static type_t anode_memtype = {
	"label",
	sizeof(anode_t)
};

static inline anode_t *anode_init() {
	anode_t *anode = mem_alloc(&anode_memtype);
	
	anode->type = A_NONE;
	anode->op = OP_NONE;
	anode->modifier = AMOD_NONE;

	return anode;
}

err_t expr_parse(tokenizer_t *tok, const block_t *blk, int allow_index, const ilist_t **anode, int allow_string);

static inline err_t expr_parse_integer(tokenizer_t *tok, const block_t *blk, int allow_index, const ilist_t **anode) {
	return expr_parse(tok, blk, allow_index,anode, 0);
}

static inline err_t expr_parse_strings(tokenizer_t *tok, const block_t *blk, int allow_index, const ilist_t **anode) {
	return expr_parse(tok, blk, allow_index,anode, 1);
}

// ---------------------------------------------------------
// evaluation of expressions

typedef enum {
	EV_UNSET	= '-',	// not set so far
	EV_CONST	= 'c',	// constant value
	EV_ADDR		= 'a',	// address value
	EV_UNDEF	= 'u',	// undefined label
} result_type;

typedef struct {
	maxval_t	value;		// the value
	int		len;		// number of bytes (1,2,4,8)
	result_type	type;
	segment_t	*segment;	// for addresses
	label_t		*label;		// for undefined values
} eval_t;

static inline int eval_len_from_value(maxval_t val) {
	if (val < (2^8)) return 1;
	if (val < (2^16)) return 2;
	if (val < (2^24)) return 3;
	if (val < (2^32)) return 4;
	return 8;
}

static inline void eval_init(eval_t *e) {
	e->value = 0;
	e->type = EV_UNSET;
	e->len = 0;
	e->segment = NULL;
	e->label = NULL;
}

err_t expr_eval(ilist_t *nodelist, eval_t *result);


#endif

