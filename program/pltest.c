#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

// Hardware unrank
// Author: Andras Majdan

#define HWXOR_BASE_ADDRESS	0x43C00000

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int memfd;
volatile void *mapped_base;
volatile void *mapped_dev_base;

void help_and_die()
{
	printf("Usage: pltest <option> [num] [col]\n\n"
		   "Options:\n"
		   "w   Write num and col\n"
		   "r   Read result\n"
		   "i   Instant write and read\n");
	exit(0);
}

void map_memory(off_t dev_base, unsigned long map_size, unsigned long map_mask)
{
	memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (memfd == -1)
	{
	    printf("Can't open /dev/mem.\n");
	    exit(0);
    }

	mapped_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~map_mask);
	if (mapped_base == (void *) -1)
	{
	    printf("Can't map the memory to user space.\n");
	    exit(0);
	}

	mapped_dev_base = mapped_base + (dev_base & map_mask);
}

void unmap_and_exit(unsigned long map_size)
{
    if (munmap(mapped_base, map_size) == -1) {
        printf("Can't unmap memory from user space.\n");
        exit(0);
    }

    close(memfd);
}


int main(int argc, char *argv[])
{
	 char opt;
	 if (argc<2) help_and_die();
	 opt = argv[1][0];

     if (opt!='w' && opt!='r' && opt!='i') help_and_die();
	 map_memory(HWXOR_BASE_ADDRESS, MAP_SIZE, MAP_MASK);

	 volatile unsigned long *num = ((volatile unsigned long *)mapped_dev_base);
	 volatile unsigned long *col = ((volatile unsigned long *)mapped_dev_base) + 1;
	 volatile unsigned long *row = ((volatile unsigned long *) mapped_dev_base) + 2;
	 volatile unsigned long *val = ((volatile unsigned long *) mapped_dev_base) + 3;


	 if (opt=='w' || opt=='i')
	 {
	  	if (argc<4) help_and_die();
	    *(num) = strtoul (argv[2], NULL, 0);
	    *(col) = strtoul (argv[3], NULL, 0);
	 }

	 if (opt=='r' || opt=='i')
	 {
	    	printf("(num: %lu col: %lu) = (row: %lu val: %lu)\n", *(num), *(col), *(row), *(val));
	 }

	 unmap_and_exit(MAP_SIZE);
	 return 0;
}
