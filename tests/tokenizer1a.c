
#include <stdbool.h>

#include "log.h"
#include "tokenizer.h"


const char *teststrings[] = {
	"label1 lda #$10",
	"label2: lda <xyz ; comment",
	"dec 125",
	"dec 125,",
	"dec 125;",
	"dec 0128",
	"dec 0127,",
	"dec 0126;",
	"inc 0x12df",
	"inc $1234f",
	"inc 0x1234f,",
	"inc $1234f,",
	"inc $1234f;",
	"inc $12g4f;",
	"inc 0x12g4f;",
	".byt 'xsd&'",
	".byt 'xsd&",
	".asc \"dsdd'",
	".asc \"dsA€\"",
	NULL
};


void do_test(const char *teststr);

int main(int argc, char *argv[]) {


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

	tokenizer_t *tok = tokenizer_create(teststr, 0);

	while (tokenizer_next(tok, false)) {

		if (tok->type == T_LITERAL) {
			printf("TOK -> type=%c, len=%d, ptr=%s ltype=%c, val=%ld\n", 
				tok->type, tok->len, tok->line+tok->ptr, tok->vals.literal.type, tok->vals.literal.value);
		} else {
			printf("TOK -> type=%c, len=%d, ptr=%s\n", tok->type, tok->len, tok->line+tok->ptr);
		}
	}
	
	printf("END -> type=%c, rest->%s\n", tok->type, tok->line+tok->ptr);

	tokenizer_free(tok);
}

