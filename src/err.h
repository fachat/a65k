
/****************************************************************************

    error defnitions
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


#ifndef ERR_H
#define ERR_H


#include "log.h"
#include "tokenizer.h"

typedef enum {
	E_OK			= 0,		// OK
	E_SYNTAX		= 1,		// Syntax error, unexpected token
	E_ABORT			= 2,		// Abort, e.g. after an illegal command line parameter

	E_TOK_DIGITRANGE 	= 100,		// illegal digit when parsing an int value
	E_TOK_EMPTY		= 101,		// empty token (e.g. "$" hex indicator, but no digits), or empty string
	E_TOK_NONPRINT		= 102,		// illegal (non-printable) character in parsed string
	E_TOK_UNKNOWN		= 103,		// unknown token

	E_ARITH_CLOSING		= 104,		// wrong closing bracket	
} err_t;


void error_module_init();


#define	loclog_error(loc, msg, ...)	loclog(LEV_ERROR, loc, msg, __VA_ARGS__)
#define	loclog_warn(loc, msg, ...)	loclog(LEV_WARN, loc, msg, __VA_ARGS__)
#define	loclog_info(loc, msg, ...)	loclog(LEV_INFO, loc, msg, __VA_ARGS__)
#define	loclog_debug(loc, msg, ...)	loclog(LEV_DEBUG, loc, msg, __VA_ARGS__)
#define	loclog_trace(loc, msg, ...)	loclog(LEV_TRACE, loc, msg, __VA_ARGS__)

#define	toklog_error(loc, msg, ...)	toklog(LEV_ERROR, loc, msg, __VA_ARGS__)
#define	toklog_warn(loc, msg, ...)	toklog(LEV_WARN, loc, msg, __VA_ARGS__)
#define	toklog_info(loc, msg, ...)	toklog(LEV_INFO, loc, msg, __VA_ARGS__)
#define	toklog_debug(loc, msg, ...)	toklog(LEV_DEBUG, loc, msg, __VA_ARGS__)
#define	toklog_trace(loc, msg, ...)	toklog(LEV_TRACE, loc, msg, __VA_ARGS__)


void loclog(err_level l, const position_t *pos, const char *msg, ...);

void toklog(err_level l, const tokenizer_t *tok, const char *msg, ...);


#endif


