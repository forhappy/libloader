.text
.globl _exit_func
_exit_func:
	call func
	int $0x80

