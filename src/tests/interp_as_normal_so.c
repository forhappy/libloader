extern void test_debug(void);
extern void test_tls(void);
extern void test_mm(void);
extern void test_dict(void);
extern void test_compression(void);
extern void test_relocation(void);

int main()
{
#if 0
	test_debug();
	test_tls();
	test_mm();
	test_dict();
	test_compression();
#endif
	test_relocation();
	return 0;
}

// vim:ts=4:sw=4

