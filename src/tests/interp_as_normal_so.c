extern void test_debug(void);
extern void test_tls(void);
extern void test_mm(void);

int main()
{
	test_debug();
	test_tls();
	test_mm();
	return 0;
}

// vim:ts=4:sw=4

