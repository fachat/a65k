

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "dbgprint.h"
#include "arith.h"


void dprint(const char *val) {

	do_print("---> %s", val);

	position_t pos = { "foo", 1 };
	tokenizer_t *tok = tokenizer_create(&pos, val, 1);

	const ilist_t *out = NULL;

	err_t rv = arith_parse(tok, NULL, 0, &out, 0);

	if (rv) {
		do_print("err=%d", rv);
	}

	print_debug_arith(out);
	
	do_print("   > next is type %c, %s", tok->type, tok->line + tok->ptr);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	dprint("<1234");
	dprint("<$1234");
	dprint("<$x234");
	dprint("0x234");

	dprint("123+234");
	dprint("123+34*44");

	dprint("(123+34)*44");

}



