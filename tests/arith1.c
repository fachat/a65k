

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "print.h"


int main(int argc, char *argv[]) {

	tokenizer_module_init();

	tokenizer_t *tok = tokenizer_create("<1234");

	const ilist_t *out = NULL;

	arith_parse(tok, 0, &out);

	print_debug_arith(out);
}



