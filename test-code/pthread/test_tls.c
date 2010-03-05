#include <asm/unistd_32.h>
#include <unistd.h>

#include <stdio.h>

struct user_desc
{
  unsigned int entry_number;
  unsigned long int base_addr;
  unsigned int limit;
  unsigned int seg_32bit:1;
  unsigned int contents:2;
  unsigned int read_exec_only:1;
  unsigned int limit_in_pages:1;
  unsigned int seg_not_present:1;
  unsigned int useable:1;
  unsigned int empty:25;
};

#define READ_LDT		(0)
#define WRITE_LDT_OLD		(1)
#define READ_DEFAULT_LDT	(2)
#define WRITE_LDT			(0x11)

static int __modify_ldt(int func,
		struct user_desc * user_desc,
		unsigned long bc)
{
	return syscall(__NR_modify_ldt, func, user_desc, bc);
}

static char test_str[] = "abcdefg\n";

int main()
{
	int err;
	struct user_desc desc;
	desc.entry_number = 0;
	desc.base_addr = (unsigned long)test_str;
	desc.limit = sizeof(test_str);
	desc.seg_32bit = 1;
	desc.contents = 0;
	desc.read_exec_only = 0;
	desc.limit_in_pages = 0;
	desc.seg_not_present = 0;
	desc.useable = 1;

	err = __modify_ldt(WRITE_LDT, &desc, sizeof(desc));
	printf("%d\n", err);

	asm volatile ("movw %w0, %%fs\n" :: "q" (desc.entry_number * 8 + 4));


	int c;

	asm volatile ("movl %%fs:0x0, %%eax\n" : "=a" (c));
	printf("0x%x\n", c);

	return 0;
}

