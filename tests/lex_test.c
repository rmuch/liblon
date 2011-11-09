#include <lon.h>

#include <stdio.h>
#include <stdlib.h>

char *test1 = "{a = 1, b = 2, c = 3}";
char *test2 = "{[\"a\"]=1;[\"b\"]    = 2;  [\"c\"] = 50; 30;20,{1,2,3}}";

/**
 * Helper function to read a file into a string.
 */
static char *file_read(char *path)
{
	FILE *f;
	char *buf;
	long sz;
	f = fopen(path, "r");
	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc(sizeof(char) * (sz + 1));
	fread(buf, sizeof(char), sz, f);
	fclose(f);
	return buf;
}

int main(int argc, char *argv[])
{
	char *test_1;
	LONParser *p;

	// This is very ugly.
	test_1 = file_read("lex_test_1.lon");
	if (test_1 == NULL) {
		fprintf(stderr, "error: could not read lex_test_1.lon\n");
		test_1 = file_read("tests/lex_test_1.lon");
		if (test_1 == NULL) {
			fprintf(stderr, "error: could not read tests/lex_test_1.lon\n");
			test_1 = file_read("../../tests/lex_test_1.lon");
			if (test_1 == NULL) {
				fprintf(stderr, "error: could not read ../../tests/lex_test_1.lon\n");
				return 1;
			}
		}
	}

	p = LONParserCreate();
	LONParserParse(p, test_1);

	LONDebugFun1(p);
	LONDebugFun2(p);

	LONParserDestroy(p);
	free(test_1);

	return 0;
}
