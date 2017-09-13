/****************************************************************************

    error handling
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

#ifndef ERRORS_H
#define ERRORS_H


#include "tokenizer.h"

void error_module_init();


#define	loclog_error(loc, msg, ...)	loclog(LEV_ERROR, loc, msg, __VA_ARGS__)
#define	loclog_warn(loc, msg, ...)	loclog(LEV_WARN, loc, msg, __VA_ARGS__)
#define	loclog_info(loc, msg, ...)	loclog(LEV_INFO, loc, msg, __VA_ARGS__)
#define	loclog_debug(loc, msg, ...)	loclog(LEV_DEBUG, loc, msg, __VA_ARGS__)
#define	loclog_trace(loc, msg, ...)	loclog(LEV_TRACE, loc, msg, __VA_ARGS__)

void loclog(err_level l, const position_t *pos, const char *msg, ...);

void toklog(err_level l, const tokenizer_t *tok, const char *msg, ...);


#endif

