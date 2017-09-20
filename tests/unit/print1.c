

#include <stdio.h>

#include "types.h"
#include "position.h"
#include "cpu.h"
#include "segment.h"
#include "context.h"
#include "position.h"
#include "operation.h"
#include "cmdline.h"
#include "parser.h"
#include "print-config.h"
#include "print.h"
#include "log.h"


static void test(const context_t *ctx, const print_config_t *cfg, position_t *pos, const char *txt) {

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
		print_formatted_stmt(stmt, cfg);
	}
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

	const print_config_t *cfg = print_current_config();

	const cpu_t *cpu = cpu_by_name("nmos");

	segment_t *seg = segment_new(NULL, "test", SEG_ANY, cpu->type, true);

	const context_t *ctx = context_init(seg, cpu);

	position_t pos = { "bogusfile", 1 };

	test(ctx, cfg, &pos, "label adc #123");	
/*
	test(ctx, &pos, "lda 123");	
	test(ctx, &pos, "ldx (12),y");	
	test(ctx, &pos, "lda ($12,s,x)");	
	test(ctx, &pos, "lda (12,s),y");	

	test(ctx, &pos, "ldx [12],y");	
	test(ctx, &pos, "lda [$12,s,x]");	
	test(ctx, &pos, "lda [12,s],y");	

	test(ctx, &pos, "ldx [[12]],y");	
	test(ctx, &pos, "lda [[$12,s,x]]");	
	test(ctx, &pos, "lda [[12,s]],y");	

	test(ctx, &pos, "ldx ((12)),y");	
	test(ctx, &pos, "lda (($12,s,x))");	
	test(ctx, &pos, "lda ((12,s)),y");	

	test(ctx, &pos, "ldx.0 123");	
	test(ctx, &pos, "lda.1u 123");	
	test(ctx, &pos, "lda.ne 123");	
	test(ctx, &pos, "lda.sw 123");	
*/
}


