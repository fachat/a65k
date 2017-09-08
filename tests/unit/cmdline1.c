

#include "cmdline.h"
#include "parser.h"
#include "parser-config.h"
#include "mem.h"

static err_t set_flag(int flag, void *extra) {
	printf("set_flag(%d)\n",flag);
	return E_OK;
}
static err_t set_inc(const char *val, void *extra) {
	printf("set_inc(%s)\n",val);
	return E_OK;
}
static err_t set_inc2(const char *val, void *extra) {
	printf("set_inc2(%s)\n",val);
	return E_OK;
}

cmdline_t par[] = {
        { "include-dir", "I", PARTYPE_PARAM, set_inc, NULL, NULL,
                "include dir", NULL },
        { NULL, "i", PARTYPE_PARAM, set_inc2, NULL, NULL,
                "include dir2", NULL },
        { NULL, "o", PARTYPE_FLAG, NULL, set_flag, NULL,
                "include flag", NULL },
};


void print_cfg(parser_config_t *cfg) {

	printf("config:\n");
	printf("  initial_binary=%d\n", cfg->initial_binary);
}


int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	cmdline_module_init("a65k");
	cmdline_register_mult(par, sizeof(par)/sizeof(cmdline_t));
	parser_module_init();

	parser_config_t *cfg1 = parser_current_config();
	print_cfg(cfg1);

	char *pars[] = { "a65k", "--parse-initial-binary" };
	cmdline_parse(2, pars );

	parser_config_t *cfg2 = parser_current_config();
	print_cfg(cfg2);

	char *pars2[] = { "a65k", "--no-parse-initial-binary" };
	cmdline_parse(2, pars2 );

	parser_config_t *cfg3 = parser_current_config();
	print_cfg(cfg3);

	char *pars3[] = { "a65k", mem_alloc_str("--include-dir=/foo"), mem_alloc_str("-I/bar") };
	cmdline_parse(3, pars3 );

	char *pars5[] = { "a65k", mem_alloc_str("-oifub") };
	cmdline_parse(2, pars5 );

	char *pars6[] = { "a65k", mem_alloc_str("+oiyyy") };
	cmdline_parse(2, pars6 );

	char *parsx[] = { "a65k", mem_alloc_str("-?") };
	cmdline_parse(2, parsx );
}



