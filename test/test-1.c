/*
 * =====================================================================================
 *
 *       Filename:  test-1.c
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2011 12:59:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <unistd.h>
int main()
{
#if 1 
	asm volatile("\
			movl $20, %eax\n\
			call *%gs:0x10\n\
			");
#endif
	int i = 0;
	printf("hello program:\n");
	while(1)
	{
		printf("%d\n", i++);
		sleep(1);
	}
	return 0;
}
