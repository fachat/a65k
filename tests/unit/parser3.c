

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
	pseudo_module_init();

	//parser_config_init();

	char *pars[] =  { "a65k", "--parse-initial-lineno", "--no-parse-colon-in-comments"  };
	cmdline_parse(3, pars);

	const cpu_t *cpu = cpu_by_name("nmos");

	segment_t *seg = segment_new(NULL, "test", SEG_ANY, cpu->type, true);

	const context_t *ctx = context_init(seg, cpu);

	position_t pos = { "bogusfile", 1 };

	char *pars2[] =  { "a65k", "--parse-colon-in-comments" };
	cmdline_parse(2, pars2);

	// sample lines from an existing source
	test(ctx, &pos, "10020 LDA #$E0:STA Z0+1");	
	test(ctx, &pos, "11010 SERI2 =$029B:SERI1 =$029C");	
	test(ctx, &pos, "12100 *=$ECD9:BC .BYT 11,0");	
	test(ctx, &pos, "26040 PIEC");	
	test(ctx, &pos, "22350 .BYT <-1,KT4-KT,KT6-KT");	
	test(ctx, &pos, "12000 .IF MIKE-1:.GOTO 12600");	
	test(ctx, &pos, "50000 .END; REM");	
//	test(ctx, &pos, "28650 HC LDA #<-1:STA $C6");	
}



