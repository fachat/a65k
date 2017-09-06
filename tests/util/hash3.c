

#include <string.h>

#define	DEBUG

#include "log.h"
#include "astring.h"
#include "hashmap.h"

typedef struct {
	const char *name;
	const char *data;
} entry_t;

static entry_t data[] = {
	{ "a", "11" }, 
	{ "b", "22" }, 
	{ "c", "33" }, 
	{ "d", "44" }, 
	{ "e", "55" }, 
	{ "f", "66" }, 
	{ "g", "77" }, 
	{ "h", "88" }, 
	{ "i", "99" },
	{ "nmos_illegal_nobcd", "test has negative hash used to cause illegal pointer ref" },
};

static bool_t equals_key(const void *data1, const void *data2) {

	log_debug("equals for '%s' vs. '%s' is %d", data1, data2, strcmp((char*)data1, (char*)data2));

	return (0 == strcmp((char*)data1, (char*)data2));
}

static const void* key_from_entry(const void *data) {

	entry_t *entry = (entry_t*) data;

	return 	entry->name;
}


// short hash algorithm to provoke hash conflicts
static int shorthash_from_key(const void *data) {
	
	char *cdata = (char*)data;

	int len = strlen(cdata);
	
	int hash = len;

	// the hash algorithm used below only makes sense
	// for up to 2 bytes 
	if (len > 2) {
		len = 2;
	}
	for (int i = 0; i < len; i++) {
		hash = hash * 13 + (cdata[i] & 0x07);
	}

	log_debug("hash for '%s' is %d", cdata, hash);

	return hash;
}


void do_test(hash_t *hash);

int main(int argc, char *argv[]) {

        (void)argc;
        (void)argv;

        log_module_init(LEV_DEBUG);

	hash_t *hash = NULL;

	log_debug("%s", "hashmap:");

	hash = hash_init(3, 3, shorthash_from_key, key_from_entry, equals_key);

	do_test(hash);

	return 0;
}


void do_test(hash_t *hash) {

	int datasize = sizeof(data)/sizeof(entry_t);
	printf ("sizeof data=%d\n", datasize);
	
	for (int i = 0; i < datasize; i++) {
		
		hash_put(hash, (void*)&(data[i]));
	}

	printf("-------- len =%ld\n", hash_size(hash));
	hash_iterator_t *iter = hash_iterator(hash);
	void *v = NULL;
	while ( (v = hash_iterator_next(iter)) ) {
		printf("k:%s, v:%s\n", ((entry_t*)v)->name, ((entry_t*)v)->data);
	}

	v = hash_iterator_next(iter);
	printf("excess call returns: %p\n", v);

	entry_t e1 = { "foo", "bar" };
	hash_put(hash, (void*) &e1);

	printf("-------- len =%ld\n", hash_size(hash));
	iter = hash_iterator(hash);
	while ( (v = hash_iterator_next(iter)) ) {
		printf("k:%s, v:%s\n", ((entry_t*)v)->name, ((entry_t*)v)->data);
	}

	printf("-------- len =%ld\n", hash_size(hash));
	entry_t e2 = { "foo2", "bar" };
	int i = 0;
	iter = hash_iterator(hash);
	while ( (v = hash_iterator_next(iter)) ) {
		if (i++ == 4) {
			hash_put(hash, &e2);
		}
		printf("k:%s, v:%s\n", ((entry_t*)v)->name, ((entry_t*)v)->data);
	}
}



