

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "dbgprint.h"
#include "expr.h"


void dprint(const char *val) {

	eval_t result;

	do_print("---> %s", val);

	position_t pos = { "foo", 1 };
	tokenizer_t *tok = tokenizer_create(&pos, val, 1);

	const ilist_t *out = NULL;

	err_t rv = expr_parse(tok, NULL, 0, &out, 0);

	if (rv) {
		do_print("parse err=%d", rv);
	} else {

		err_t rvx = expr_eval(out, &result);

		if (rvx) {
			do_print("eval err=%d", rv);
		}
	}

	print_debug_eval((const eval_t*)&result);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	dprint("<1234");
/*
	dprint("<$1234");
	dprint("<$x234");
	dprint("0x234");

	dprint("123+234");
	dprint("123+34*44");

	dprint("(123+34)*44");
*/
}



