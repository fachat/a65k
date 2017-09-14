

#include "cmdline.h"
#include "parser.h"
#include "parser-config.h"
#include "mem.h"
#include "infiles.h"

static err_t set_inc(const char *val, void *extra, int ival) {
	printf("set_inc(%s -> %d)\n",val, ival);
	return E_OK;
}

static param_enum_t modes[] = {
        { "ass",        "Assembler mode (default)" },
        { "lint",       "Just format the code and print it on stdout" },
        { NULL },
};

static param_enum_t *main_get_modes() {
        return modes;
}

cmdline_t par[] = {
        { "testopt", "o", PARTYPE_ENUM, set_inc, NULL, NULL,
                "include flag", main_get_modes },
};




int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	infiles_module_init();
	cmdline_module_init("a65k");
	cmdline_register_mult(par, sizeof(par)/sizeof(cmdline_t));
	parser_module_init();

	// success cases

	char *pars1[] = { "a65k", mem_alloc_str("--testopt=lint") };
	cmdline_parse(2, pars1 );

	char *pars2[] = { "a65k", "-o", "lint" };
	cmdline_parse(3, pars2 );

	char *pars3[] = { "a65k", "-olint" };
	cmdline_parse(2, pars3 );

	// failure cases
	char *pars11[] = { "a65k", mem_alloc_str("--opt=lint") };
	cmdline_parse(2, pars11 );

	char *pars12[] = { "a65k", "-o", "lnt" };
	cmdline_parse(3, pars12 );

	char *pars13[] = { "a65k", "-olnt" };
	cmdline_parse(2, pars13 );

	char *pars14[] = { "a65k", mem_alloc_str("--testopt=lnt") };
	cmdline_parse(2, pars14 );

}



