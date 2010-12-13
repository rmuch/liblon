#ifndef TEST_H_
#define TEST_H_

#define TEST_MAIN() \
	int main(int argc, char *argv[])\
	{\
		test_main(argc, argv);\
	}

static inline int test_main(int argc, char *argv[])
{
	return 1;
}

#endif /* TEST_H_ */
