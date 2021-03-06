

#include <stdio.h>

#include "types.h"
#include "position.h"
#include "cpu.h"
#include "segment.h"
#include "cmdline.h"

#include "context.h"


int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	cmdline_module_init();
	cpu_module_init();

	// test vectors
	segment_t *s = (segment_t*) 2;
	const cpu_t *c = cpu_by_name("nmos");

	const context_t *ctx = context_init(s, c);

	if (ctx->cpu_width != c->width) {
		printf("cpu width mismatch, was %d, expected %d\n", ctx->cpu_width, c->width);
	}

	context_t *dup = context_dup();

	if (dup == NULL) {
		printf("ctx dup returned NULL\n");
	}
	if (dup == ctx) {
		printf("ctx dup returned original input\n");
	}

	if (dup->segment != ctx->segment) {
		printf("segment mismatch, was %p, expected %p\n", (void*)dup->segment, (void*)ctx->segment);
	}
	if (dup->cpu != ctx->cpu) {
		printf("cpu mismatch, was %p, expected %p\n", (void*)dup->cpu, (void*)ctx->cpu);
	}
}



