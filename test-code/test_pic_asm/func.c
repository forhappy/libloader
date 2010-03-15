__attribute__((visibility("hidden"))) int func() 
{
	return 1;
}


asm (".text\n\
.globl _exit_func2\n\
_exit_func2:\n\
call func\n\
int $0x80\n\
");

int main()
{
	return 0;
}

