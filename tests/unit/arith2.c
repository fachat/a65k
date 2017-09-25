

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "dbgprint.h"
#include "arith.h"


void dprint(const char *val, int allow_index) {

	do_print("---> %s%s", val, allow_index ? " (with index)":"");

	position_t pos = { "foo", 1 };
	tokenizer_t *tok = tokenizer_create(&pos, val, 0);

	const ilist_t *out = NULL;

	arith_parse(tok, NULL, allow_index, &out, 0);

	print_debug_arith(out);
	
	do_print("   > next is type %c, %s", tok->type, tok->line + tok->ptr);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	dprint("123,x", 1);
	dprint("$123,X", 0);
	dprint("$123,X", 1);
	dprint("($123),y", 0);
	dprint("($123),y", 1);
	dprint("($123,X)", 0);
	dprint("($123,X)", 1);
	dprint("($123,s,X)", 1);

}


