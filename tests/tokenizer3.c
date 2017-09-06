
#include <stdbool.h>

#include "log.h"
#include "tokenizer.h"


const char *teststrings[] = {
	"label2: lda <xyz,x ; comment",
	"dec.u 0125,y",
	"dec.n (0127),z",
	"inc.w (0x12df,x)",
	"inc.b $1234f,y",
	"inc.l0 $1234f,y",
	"inc.q $1234f,y",
	"inc.lnu $1234f,y",
	"lda . nw #123",
	"lda.1 #123",
	NULL
};


void do_test(const char *teststr);

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

	printf("tokenizer:\n");

	for (int p = 0; ;p++) {
		const char *teststr = teststrings[p];
		if (teststr == NULL) {
			break;
		}

		do_test(teststr);
	}
	return 0;
}


void do_test(const char *teststr) {

	printf("tokenizing line: %s\n", teststr);

	tokenizer_t *tok = tokenizer_create(teststr, 1);

	while (tokenizer_next(tok, true)) {

		printf("TOK -> type=%c, len=%d, ptr=%s\n", tok->type, tok->len, tok->line+tok->ptr);
	}
	
        printf("END -> type=%c, rest->%s\n", tok->type, tok->line+tok->ptr);

	tokenizer_free(tok);
}

