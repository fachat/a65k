

#include "cmdline.h"
#include "parser.h"
#include "parser-config.h"


void print_cfg(parser_config_t *cfg) {

	printf("config:\n");
	printf("  initial_binary=%d\n", cfg->initial_binary);
}


int main(int argc, char *argv[]) {

	cmdline_module_init("a65k");
	parser_module_init();

	parser_config_t *cfg1 = parser_current_config();
	print_cfg(cfg1);

	char *pars[] = { "a65k", "-parse-initial-binary" };
	cmdline_parse(2, pars );

	parser_config_t *cfg2 = parser_current_config();
	print_cfg(cfg2);

	char *pars2[] = { "a65k", "-no-parse-initial-binary" };
	cmdline_parse(2, pars2 );

	parser_config_t *cfg3 = parser_current_config();
	print_cfg(cfg3);
}



