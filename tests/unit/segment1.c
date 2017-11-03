

#include <stdio.h>

#include "types.h"
#include "position.h"
#include "cpu.h"
#include "segment.h"
#include "context.h"
#include "position.h"
#include "instruction.h"
#include "cmdline.h"
#include "parser.h"
#include "dbgprint.h"
#include "log.h"


static void test(const context_t *ctx, position_t *pos, const char *txt) {

	parser_reset();

	line_t line = { txt, pos, parser_current_config() };

	do_print("");
	do_print(">>> %s", line.line);
		
	err_t err = parser_push(ctx, &line);

	if (err) {
		do_print("err=%d", err);
	}
	//log_set_level(LEV_DEBUG);

	list_iterator_t *iter = parser_get_statements();
	while (list_iterator_has_next(iter)) {
		statement_t *stmt = list_iterator_next(iter);
		print_debug_stmt(stmt);
	}
	list_iterator_free(iter);
}


int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	cmdline_module_init();
	cpu_module_init();
	segment_module_init();
	operation_module_init();
	parser_module_init();
	tokenizer_module_init();

	//parser_config_init();

	const cpu_t *cpu = cpu_by_name("nmos");

	segment_t *seg = segment_new(NULL, "seg1", SEG_ANY, cpu->type, true);

	context_t *ctx = (context_t*) context_init(seg, cpu);

	position_t pos = { "bogusfile", 1 };

	test(ctx, &pos, "label adc #123");	

	ctx = context_dup();
	ctx->segment = segment_new(NULL, "seg1", SEG_ANY, cpu->type, true);

	test(ctx, &pos, "lda 123");	

	ctx = context_dup();
	ctx->segment = segment_new(NULL, "seg2", SEG_ANY, cpu->type, true);

	test(ctx, &pos, "ldx (12),y");	

	ctx = context_dup();
	ctx->segment = segment_new(NULL, "seg1", SEG_ANY, cpu->type, true);

	test(ctx, &pos, "lda ($12,s,x)");	

	ctx = context_dup();
	ctx->segment = segment_new(NULL, "seg2", SEG_ANY, cpu->type, true);

	test(ctx, &pos, "lda (12,s),y");	


	do_print("By segment");
	list_iterator_t *iter = segment_get_iter();
	while (list_iterator_has_next(iter)) {
		segment_t *segm = list_iterator_next(iter);

		do_print("got segment: %s", segm->name);

		list_iterator_t *stiter = segment_get_statements(segm);
		while(list_iterator_has_next(stiter)) {
			statement_t *stmt = list_iterator_next(stiter);
			print_debug_stmt(stmt);
		}
		list_iterator_free(stiter);
	}
	list_iterator_free(iter);
}



