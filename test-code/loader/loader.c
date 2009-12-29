
#define out(x)	write(1, x, sizeof(x))


static int
write(int fd, char * str, int len)
{
	int ret;
	asm volatile (
		"xchgl %%ebx, %%esi\n"
		"int $0x80\n"
		"xchgl %%ebx, %%esi\n"
		: "=a" (ret)
		: "a" (4), "S" (fd), "c" (str), "d" (len)
			);
	return ret;
}

static void
exit(int status)
{
	asm volatile (
		"movl $1, %eax\n"
		"int $0x80\n"
	);
}

asm (
".globl _start\n\
 _start:\n\
	call main\n\
"
);

/* must be defined as static, or ld will create textrel object */
static int
main()
{
	out("in loader\n");
	exit(0);
	return 0;
}

