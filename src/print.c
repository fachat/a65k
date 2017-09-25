/****************************************************************************

    print output
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

#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "print-config.h"
#include "print.h"
#include "arith.h"

#define	BUF_LEN		4096

void print_module_init() {

	print_config_init();
}

void print(const char *pattern, ...) {

        va_list ap;
        va_start(ap, pattern);

        char buf[BUF_LEN];

        int r = vsnprintf(buf, BUF_LEN, pattern, ap);
        if (r < 0 || r > BUF_LEN) {
                // error
                log_error("Error printing %d\n", r);
                return;
        }
        printf("%s", buf);
}


