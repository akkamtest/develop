//#include <stdio.h>
//#include "stdint.h"

#include <common.h>
#include "memtest.h"

static unsigned long long int SEED_A;
static unsigned long long int SEED_B;
static unsigned long long int SEED_C;
static unsigned long long int SEED_D;

unsigned long long int rand1 (unsigned char salt)
{
   static unsigned int a = 18000, b = 30903, c = 15333, d = 21041;

   SEED_A  = salt*a*(SEED_A &65535) + (SEED_A >>16);
   SEED_B  = salt*b*(SEED_B &65535) + (SEED_B >>16);
   SEED_C  = salt*c*(SEED_C &65535) + (SEED_C >>16);
   SEED_D  = salt*d*(SEED_D &65535) + (SEED_D >>16);

   return ((SEED_A <<48) + ((SEED_B &65535)<<32) + (SEED_C <<16) + (SEED_D &65535));
}


void reset_seed(void)
{
	unsigned long long int seed1 = 521288629;
	unsigned long long int seed2 = 362436069;
	unsigned long long int seed3 = 123456789;
	unsigned long long int seed4 = 917456120;
	
   SEED_A  = seed1;   
   SEED_B  = seed2;
   SEED_C  = seed3;   
   SEED_D  = seed4;
}


/*  Allows to visualize which test return the error, the faulty address and the difference between the expected value and the one read */
void error(unsigned long long int adr, unsigned long long int good, unsigned long long int bad, int test_num)
{
	printf ("TEST number: %d\n", test_num);
	printf ("	Faulty address: %llx\n", adr);
	printf ("		>Expected result: %llx\n", good);
	printf ("		>Obtained value : %llx\n", bad);	
}

/*  Allows to visualize which test return the error, the faulty address and the difference between the expected value and the one read */
void mtest_debug(int test_num, unsigned long long int adr, unsigned long long int value)
{
	printf ("TEST number: %d\n", test_num);
	printf ("	> Address: %llx\n", adr);
	printf ("	> Value  : %llx\n", value);	
}

/* Walking ones */
/* Test 0 [Address test, 8bits walking ones, no cache] */
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char addr_tst0(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	unsigned char i;
	unsigned char *p;
	unsigned char *pe;
	unsigned char mask;
	//unsigned long j=1; // only for debug
	unsigned long length;
	int test_num = 1;

	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = (unsigned char*)buf;
	length = (end - start) / sizeof(unsigned char);
	pe = p + length;

	// test each bit for all address
	for (; p <= pe; p++) 
	{
		for (i = 0; i<8; i++) 
		{
			mask = 1<<i;
			*p &= mask;
			*p |= mask;
			//*p = mask;
			//error((unsigned long long int)p, mask, *p, j);
			
			
			if(*p != mask) 
			{
				error((unsigned long long int)p, mask, (unsigned long long int)*p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
		}
		//j++; // only for debug
	}

	return(0);
}

/* Write each address with it's own address */
/* Test 1 [Address test, own address, Sequential] */
/* Test 2 [Address test, own address, Parallel] */
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char addr_tst1(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, char stop_after_err)
{
	unsigned long long int *p;
	unsigned long long int *pe;
	//unsigned long j=1; // only for debug
	unsigned long length;
	int test_num = 2;	

	/* Initialise tested memory range */
	end = end -1; // for length calculation	
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;
	
	/* Write each address with it's own address */	
	for (; p <= pe; p++) 
	{		
		*p = (unsigned long long int)p;
		//mtest_debug(test_num, p, *p);
	}

	/* Each address should have its own address */
	p = buf;
	pe = p + length;
	for (; p <= pe; p++) 
	{
		
		if(*p != (unsigned long long int)p) 
		{
			error((unsigned long long int)p, (unsigned long long int)p, *p, test_num);
			if (stop_after_err == 1)
			{
				return(1);	
			}
		}
	} 
	return(0);
}


/* moving inversion 64bits single pattern and complement */
/* Test 3 [Moving inversions, ones&zeros, Parallel] */
// iter : Repeat the reading of the pattern and the writing and reading of its complement
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char movinv (int iter, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int i;
	int test_num = 3;
	unsigned long long int *p, *pe;
	unsigned long long int p1 = 0xA5A5A5A5A5A5A5A5;
	unsigned long length;

	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;

	/* Initialize memory with the initial pattern.  */
	for (; p <= pe; p++) 
	{
		*p = p1;
	}
	
	/* Do moving inversions test. Check for initial pattern and then
	 * write the complement for each memory location. Test from bottom
	 * up and then from the top down.  */
	for (i=0; i<iter; i++) 
	{
		p = buf;
		pe = p + length;
		for (; p <= pe; p++) 
		{
			if (*p != p1) 
			{
				error((unsigned long long int)p, p1, *p, test_num);
				if (stop_after_err == 1)
				{			
					return(1);	
				}
			}
			*p = ~p1;
		}
		
		pe = buf;
		p = pe + length; 
		do 
		{			
			if (*p != ~p1) 
			{
				error((unsigned long long int)p, ~p1, *p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = p1;
		} while (--p >= pe);
	}
	return(0);
}


/* moving inversion 8bits single pattern and complement*/
/* Test 4 [Moving inversions, 8 bit pattern] */
// iter : Repeat the reading of the pattern and the writing and reading of its complement
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char movinv_8bit (int iter, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int i;
	int test_num = 58;
	unsigned char *p, *pe;
	unsigned char p1 = 0x0F;
	unsigned char p2 = ~p1;
	unsigned long length;
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = (unsigned char*)buf;
	length = (end - start) / sizeof(unsigned char);
	pe = p + length;
	
	/* Initialize memory with the initial pattern.  */
	for (; p <= pe; p++)
	{
		*p = p1;
	}
	
	/* Do moving inversions test. Check for initial pattern and then
	 * write the complement for each memory location. Test from bottom
	 * up and then from the top down.  */
	for (i=0; i<iter; i++) 
	{
		p = (unsigned char*)buf;
		pe = p + length;

		for (; p <= pe; p++) 
		{
			if (*p != p1) 
			{
				error((unsigned long long int)p, (unsigned char)p1, (unsigned char)*p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = p2;
		}
		pe = (unsigned char*)buf;
		p = pe + length; 
		do 
		{
			if (*p != p2) 
			{
				error((unsigned long long int)p, (unsigned char)p2, (unsigned char)*p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = p1;
		} while (--p >= pe);
	}
	return(0);
}


/* moving inversion random 64bits pattern and complement */
/* Test 5 [Moving inversions, random pattern] */
// iter : Repeat the reading of the pattern and the writing and reading of its complement
// salt : Modify random pattern
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char movinvr (int iter, unsigned char salt, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int i;
	int test_num = 5;
	unsigned long long int *p, *pe;
	unsigned long long int p1;
	unsigned long length;
	
	/* Initialise random pattern */
	reset_seed();
	p1 = rand1(salt);

	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;
	
	/* Initialize memory with the initial pattern */
	for (; p <= pe; p++) 
	{
		*p = p1;
		
	}

	/* Do moving inversions test. Check for initial pattern and then
	 * write the complement for each memory location. Test from bottom
	 * up and then from the top down.  */
	for (i=0; i<iter; i++) 
	{
		p = buf;
		pe = p + length;

		for (; p <= pe; p++) 
		{
			if (*p != p1) 
			{
				error((unsigned long long int)p, p1, *p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = ~p1;
		}


		pe = buf;
		p = pe + length;
		do 
		{
			if (*p != ~p1) 
			{
				error((unsigned long long int)p, ~p1, *p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = p1;
		} while (--p >= pe);
	}
	return(0);
}



/* Test 7 [Moving inversions, 64 bit pattern] */
// iter : Repeat the reading of the pattern and the writing and reading of its complement
// p1   : 64bit pattern to be written in the memory
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char movinv64(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int k=0;
	unsigned long long int *p, *pe, pat, comp_pat, p1 = MEMTEST_PATTERN_64_A;
	int test_num = 7;
	unsigned long long int tab[64];
	unsigned char tab_compl = 0;
	unsigned long length;
	
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;

	/* Initialize memory with the initial pattern.  */
	k = 0;
	pat = p1;	
	while (p <= pe)
	{
		*p = pat;		
		if (tab_compl == 0)
		{
			
			tab[k] = pat;
		}
		
		if (++k >= 64)
		{
			pat = p1;
			k = 0;
			tab_compl = 1;
		}
		else
		{
			pat = pat << 1;
		}
		p++;
	}


	/* Do moving inversions test. Check for initial pattern and then
	 * write the complement for each memory location. Test from bottom
	 * up and then from the top down.  */
	p = buf;
	pe = p + length;
	k = 0;
	while (1)
	{			
		pat = tab[k];
		if (*p != pat)
		{
			error((unsigned long long int)p, pat, *p, test_num);
			if (stop_after_err == 1)
			{
				return(1);	
			}
		}
		comp_pat = ~pat;
		*p = comp_pat;
		if (p >= pe)
		{
			break;
		}
		p++;

		if (++k >= 64)
		{
			k = 0;
		}

	}

	pe = buf;
	p = pe + length;
	while (1)
	{		
		pat = tab[k];
		comp_pat = ~pat;	
		if (*p != comp_pat)
		{
			error((unsigned long long int)p, comp_pat, *p, test_num+1);
			if (stop_after_err == 1)
			{
				return(1);	
			}
		}
		
		*p = pat;
		if (p <= pe)
		{
			break;
		}
		p--; 
		if (--k < 0)
		{
			k = 63;
		}
	}

	return(0);
}

/* Test 8 [Random number sequence] */
// salt : Modify random pattern
// Start: Starting address of the test
// End  : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char rand_seq(unsigned char salt, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int i;
	unsigned long long int *p, *pe, num;
	int test_num = 8;
	unsigned long length;
	
	reset_seed();
	
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;	
	
	for (; p <= pe; p++) 
	{
		*p = rand1(salt);
	}


	/* Do moving inversions test. Check for initial pattern and then
	 * write the complement for each memory location. */
	for (i=0; i<2; i++)
	{
		reset_seed();
		p = buf;
		pe = p + length;
	
		for (; p <= pe; p++)
		{			
			num = rand1(salt);
			if (i)
			{
				num = ~num;
			}
			if (*p != num)
			{
				error((unsigned long long int)p, num, *p, test_num);
				if (stop_after_err == 1)
				{
					return(1);	
				}
			}
			*p = ~num;
		}
	}
	return(0);
}


/* Test 9 [Modulo 20, Random pattern] */
// offset: modify the starting address
// iter  : Repeat the reading of the pattern and the writing and reading of its complement
// p1    : 32bit pattern to be written in the memory
// Start : Starting address of the test
// End   : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
unsigned char modtst(int offset, int iter, unsigned long long int p1, unsigned long long int p2, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	int k, l;
	int test_num = 9;
	unsigned long long int *p;
	unsigned long long int *pe;
	unsigned long length;
	end -= MOD_SZ * 8;	/* adjust the ending address */
	
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf + offset;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length ;		
	for (; p <= pe; p += MOD_SZ)
	{
		*p = p1;		
	} 

	/* Write the rest of memory "iter" times with the pattern complement */
	for (l=0; l<iter; l++) 
	{
		//calculate_chunk(&start, &end);
		p = buf;
		pe = p + length;
		k = 0;

		for (; p <= pe; p++) 
		{
			if (k != offset)
			{
				*p = p2;
			}
			if (++k > MOD_SZ-1)
			{
				k = 0;
			}
		}
	}
	
	p = buf + offset;
	pe = p + length ;
	end -= MOD_SZ*8;	/* adjust the ending address */
	for (; p <= pe; p += MOD_SZ) 
	{
		if (*p != p1) 
		{
			error((unsigned long long int)p, p1, *p, test_num);
		}
		if (stop_after_err == 1)
		{
			return(1);	
		}
	}
	return(0);
}

 /* Test 10 [Bit fade test, 2 patterns] */
 /* Test memory for bit fade, fill memory with pattern wait and see */
 // p1    : 32bit pattern to be written in the memory
// Start : Starting address of the test
// End   : Ending address of the test
// stop_after_err: 1 stop the test after an error / 0 let the test running
void bit_fade_fill(unsigned long long int p1, unsigned long long int *buf, unsigned long long int start, unsigned long long int end)
{
	unsigned long long int *p, *pe;
	unsigned long length;
	
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;
	
	/* Initialize memory with the initial pattern. */		
	for (; p <= pe;p++)
	{
		*p = p1;
	}
}
	
unsigned char bit_fade_chk(unsigned long long int p1, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err)
{
	unsigned long long int *p, *pe, bad;
	int test_num = 10;
	unsigned long length;
	
	/* Initialise tested memory range */
	end = end -1; // for length calculation
	p = buf;
	length = (end - start) / sizeof(unsigned long long int);
	pe = p + length;

	/* Make sure that nothing changed while sleeping */
	for (; p <= pe;p++)
	{
		
		if ((bad=*p) != p1)
		{
			error((unsigned long long int)p, p1, bad, test_num);
			if (stop_after_err == 1)
			{
				return(1);	
			}
		}
	}
	return(0);
}

void wait (unsigned int sec)
{
	unsigned long long int t, wait;
	wait = 100000 * sec;
	for ( t = 1; t <= wait; t++)
	{
		printf("\rusec: %llu", t*10);
	}
	printf("\n");
	return;
}
