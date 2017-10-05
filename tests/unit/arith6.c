

#include <stdio.h>

#include "inline_list.h"
#include "tokenizer.h"
#include "dbgprint.h"
#include "arith.h"


void dprint(const char *val, block_t *blk, position_t *pos, int allow_index) {

	do_print("---> %s%s", val, allow_index ? " (with index)":"");

	const ilist_t *out = NULL;

	tokenizer_t *tok = tokenizer_create(pos, val, 0);
	arith_parse(tok, blk, allow_index, &out, 0);

	print_debug_arith(out);
	
	do_print("   > next is type %c, %s", tok->type, tok->line + tok->ptr);
}

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	tokenizer_module_init();

	position_t pos = { "foo", 0 };
        block_t *blk = block_init(NULL, &pos);

	dprint("1+label1+3+4", blk, &pos, 0);
	dprint("1+2+label2+4", blk, &pos, 1);

        label_t *label1 = label_init(NULL, "label1", &pos);
        block_add_label(blk, label1);

	dprint("1+(label1+3)*4", blk, &pos, 0);
	dprint("1+(2+3)*label3", blk, &pos, 1);

}



