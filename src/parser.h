/****************************************************************************

    parser
    Copyright (C) 2012,2017 Andre Fachat

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


#ifndef PARSER_H
#define PARSER_H

typedef struct statement_s statement_t;

#include "context.h"
#include "label.h"
#include "block.h"
#include "instruction.h"
#include "tokenizer.h"
#include "list.h"
#include "inline_list.h"
#include "err.h"
#include "pseudo.h"

typedef enum {
	S_NONE,
        S_PCDEF,                // set PC
        S_LABEQPC,              // set label to PC
        S_LABDEF,               // set label from parameter
	S_OPCODE,		// opcode
	S_PSEUDO,		// pseudo opcode
} stype_t;



/**
 * The statement_t struct is the main structure of the internal workings.
 * The parser creates a statement_t with all information to be gathered from the source.
 * The printer can "lint" this statement and pretty-format it
 * The assembler itself resolves labels and computes references and extends it with binary values
 * The printer can then also print the extended statement with binary values
 * The renderer can then put (a list of) statements into a binary format, depending on the output options
 */
struct statement_s {
        const block_t           *blk;		// pointer to current block structure for label dereferencing
        const context_t         *ctx;		// current context, like CPU type, width, other options
        stype_t                 type;		// type of statement (see above)
        // optional
	int			lineno;		// parsed BASIC-style line number
        const label_t           *label;		// TODO allow for multiple label definitions per line/statement
	// S_LABDEF (see also param in S_OPCODE)
	op_t			assign;		// assign operation
	// S_OPCODE
       	const instruction_t     *op;		// pointer to info about operation, like "ADC", "TAX", ...
	op_t			base;		// base extension of the parameter, like ",B", ",s" if any, zero else
	le_type			le_prefix;	// LE prefix
	rs_type			rs_prefix;	// RS prefix
	bool_t			um_prefix;	// UM prefix
	bool_t			nf_prefix;	// NF prefix
	syntax_type		syn;		// addressing mode syntax, see SY_* constants
       	ilist_t  	        *param;		// list of anode_t structs that describe the syntax tree for the parameter value (also for S_LABDEF)
	// S_PSEUDO
	pseudo_t		*pseudo;	// pseudo opcode definition
	list_t			*pparams;	// parameters for the pseudo opcode
	// COMMENT
	char			*comment;	// comment
};


void parser_module_init(void);

err_t parser_push(const context_t *context, const line_t *line);

list_iterator_t *parser_get_statements(void);

// for test cases only
void parser_reset();

#endif

