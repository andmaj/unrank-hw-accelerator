#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>

// Note: compile with -std=c99

// Parameters
#define RANK_MIN 0
#define RANK_MAX 19
#define WIDTH 6
#define WEIGHT 3

// ----------------------------
// DO NOT MODIFY FROM THIS LINE
// ----------------------------

#define BITS_MAX 32
#define HW_BASEADDRESS 0

#define HW_INPUTCOL HW_BASEADDRESS+0
#define HW_INPUTNUM HW_BASEADDRESS+1
#define HW_OUTPUTROW HW_BASEADDRESS+2

#define HWUNRANK_BASE_ADDRESS	0x43C00000

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int memfd;
volatile void *mapped_base;
volatile void *mapped_dev_base;

volatile unsigned long *col;
volatile unsigned long *num;
volatile unsigned long *row;

void init_lookup_table();
uint_fast32_t unrank(uint_fast32_t rank, uint_fast8_t width, uint_fast8_t weight);
void print_res(uint_fast32_t rank, uint_fast32_t num, uint_fast8_t width);
void map_memory(off_t dev_base, unsigned long map_size, unsigned long map_mask);

uint_fast32_t lookup_table[BITS_MAX+1][BITS_MAX+1];

#ifdef DEBUG
#define HW_SET(ADDR,VAL) debug_hw_set(ADDR,VAL)
#define HW_GET(ADDR) debug_hw_get(ADDR)
#else
#define HW_SET(ADDR,VAL) hw_set(ADDR,VAL)
#define HW_GET(ADDR) hw_get(ADDR)
#endif // DEBUG

#ifdef DEBUG
uint32_t debug_hw_get(uint32_t addr);
void debug_hw_set(uint32_t addr, uint32_t val);
void debug_hw_process();

uint32_t debug_hw_inputcol, debug_hw_inputnum;
uint32_t debug_hw_outputrow;

void debug_hw_set(uint32_t addr, uint32_t val)
{
    switch(addr)
    {
        case HW_INPUTCOL:
            debug_hw_inputcol = val;
            break;
        case HW_INPUTNUM:
            debug_hw_inputnum = val;
            break;
        default:
            assert(false);
    }
}
uint32_t debug_hw_get(uint32_t addr)
{
    debug_hw_process();

    switch(addr)
    {
        case HW_OUTPUTROW:
            return debug_hw_outputrow;
        default:
            assert(false);
    }
}

void debug_hw_process()
{
    uint_fast32_t num;

    for(uint_fast8_t row=1; row<=BITS_MAX; row++)
    {
        num = lookup_table[row][debug_hw_inputcol];
        if(num>=debug_hw_inputnum)
        {
            debug_hw_outputrow = row;
            return;
        }
    }

    assert(false);
}
#else
void hw_set(uint32_t addr, uint32_t val)
{
    switch(addr)
    {
        case HW_INPUTCOL:
            *col = val;
            break;
        case HW_INPUTNUM:
            *num = val;
            break;
        default:
            assert(false);
    }
}
uint32_t hw_get(uint32_t addr)
{
	switch(addr)
	{
		case HW_OUTPUTROW:
			return *row;
	        default:
	            assert(false);
	    }
}
#endif // DEBUG

void init_lookup_table()
{
    for(uint_fast8_t i=0; i<=BITS_MAX; i++)
    {
        lookup_table[i][1] = i;
    }

	for(uint_fast8_t row=2; row<=BITS_MAX; row++)
		for(uint_fast8_t col=2; col<row+1; col++)
			lookup_table[row][col] = lookup_table[row-1][col-1] + lookup_table[row-1][col];
}

void print_res(uint_fast32_t rank, uint_fast32_t num, uint_fast8_t width)
{
    char numinstr[width+1];

    numinstr[width] = '\0';

    printf("%5" PRIuFAST32 " %2" PRIuFAST32 " ", rank, num);

    while(width--)
    {
        if(num%2)
            numinstr[width] = '1';
        else
            numinstr[width] = '0';

        num /= 2;
    }

    printf("%s\n", numinstr);
}

void map_memory(off_t dev_base, unsigned long map_size, unsigned long map_mask)
{
	memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (memfd == -1)
	{
	    printf("Can't open /dev/mem.\n");
	    exit(1);
    }

	mapped_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~map_mask);
	if (mapped_base == (void *) -1)
	{
	    printf("Can't map the memory to user space.\n");
	    exit(1);
	}

	mapped_dev_base = mapped_base + (dev_base & map_mask);

	col = ((volatile unsigned long *)mapped_dev_base);
	num = ((volatile unsigned long *)mapped_dev_base) + 1;
	row = ((volatile unsigned long *)mapped_dev_base) + 2;
}

int main()
{
    uint_fast32_t res;

    init_lookup_table();
	map_memory(HWUNRANK_BASE_ADDRESS, MAP_SIZE, MAP_MASK);

    for(uint_fast8_t i=RANK_MIN; i<=RANK_MAX; i++)
    {
        res = unrank(i, WIDTH, WEIGHT);
        print_res(i, res, WIDTH);
    }

    return 0;
}

uint_fast32_t unrank(uint_fast32_t rank, uint_fast8_t width, uint_fast8_t weight)
{
    // Please notice:
    // - maximum of res is 31 bit, because of the right shifting at the end
    // - less significant bit of mask is don't care

    uint_fast32_t lookfor, fromtable, res=0, mask=1;
    uint_fast8_t shift, foundrow;

    assert(width <= BITS_MAX);
    assert(weight <= width);

    fromtable = lookup_table[width][weight];
    lookfor = fromtable - rank;

    while(weight)
    {
        HW_SET(HW_INPUTCOL, weight);
        HW_SET(HW_INPUTNUM, lookfor);

        foundrow = HW_GET(HW_OUTPUTROW);
        shift = width - foundrow + 1;
        rank -= fromtable -  lookup_table[foundrow][weight];


        mask <<= shift;
        res |= mask;
        width -= shift;

        fromtable = lookup_table[width][--weight];
        lookfor = fromtable - rank;
    }
    assert(rank==0);
    return res >> 1;
}
