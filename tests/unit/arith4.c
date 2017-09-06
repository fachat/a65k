

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "print.h"
#include "arith.h"


void print(const char *val, int allow_index) {

	do_print("---> %s%s", val, allow_index ? " (with index)":"");

	tokenizer_t *tok = tokenizer_create(val, 0);

	const ilist_t *out = NULL;

	arith_parse(tok, allow_index, &out);

	print_debug_arith(out);
	
	do_print("   > next is type %c, %s", tok->type, tok->line + tok->ptr);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	print("1+2+3+4", 0);
	print("1+2+3+4", 1);
	print("1+(2+3)*4", 0);
	print("1+(2+3)*4", 1);

}


