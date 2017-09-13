

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "print.h"
#include "arith.h"


void print(const char *val) {

	do_print("---> %s", val);

	position_t pos = { "foo", 1 };
	tokenizer_t *tok = tokenizer_create(&pos, val, 1);

	const ilist_t *out = NULL;

	arith_parse(tok, 0, &out);

	print_debug_arith(out);
	
	do_print("   > next is type %c, %s", tok->type, tok->line + tok->ptr);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	print("<1234");
	print("<$1234");
	print("<$x234");
	print("0x234");

	print("123+234");
	print("123+34*44");

	print("(123+34)*44");

}



