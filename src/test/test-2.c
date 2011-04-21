/*
 * =====================================================================================
 *
 *       Filename:  test-2.c
 *
 *    Description:  test bus error
 *
 *        Version:  1.0
 *        Created:  04/12/2011 11:33:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/mman.h>
int main()
{
	char *p = mmap(0, 4096, 1 | 2, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	for (int i = 0; i < 4095; i ++){
		p[i] = '1';
	}

	char *p2 = mmap(0, 4096, 1 | 2, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	for (int i = 0; i < 4095; i ++){
		p2[i] = '1';
	}

	p[4095] = '\0';
	printf("%s\n", p);
	return 0;
}
