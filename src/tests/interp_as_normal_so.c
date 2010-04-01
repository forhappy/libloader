extern void test_debug(void);
extern void test_tls(void);
extern void test_mm(void);
extern void test_dict(void);
extern void test_compression(void);

int main()
{
#if 0
	test_debug();
	test_tls();
	test_mm();
	test_dict();
#endif
	test_compression();
	return 0;
}

// vim:ts=4:sw=4

