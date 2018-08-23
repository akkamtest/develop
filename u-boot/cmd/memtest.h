
#ifndef _MEMTEST_H_
#define _MEMTEST_H_

//typedef unsigned long ulong;



#define MOD_SZ			   		20
#define MEMTEST_MOD_OFFSET 		0
#define MEMTEST_ITERATION  		20
#define MEMTEST_RAND_SEED_1		5
#define MEMTEST_RAND_SEED_2		17
#define MEMTEST_PATTERN_64_A	0xCAFEDECACAFEDECA
#define MEMTEST_PATTERN_64_B 	0xDECACAFEDECACAFE


unsigned long long int rand1 (unsigned char salt);
void reset_seed(void);
void error(unsigned long long int adr, unsigned long long int good, unsigned long long int bad, int test_num);
unsigned char addr_tst0(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
//void mtest_debug(int test_num, unsigned long long int adr, unsigned long long int value);
unsigned char addr_tst1(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, char stop_after_err);
unsigned char movinv (int iter, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
unsigned char movinv_8bit (int iter, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
unsigned char movinvr (int iter, unsigned char salt, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
unsigned char movinv64(unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
unsigned char rand_seq(unsigned char salt, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
unsigned char modtst(int offset, int iter, unsigned long long int p1, unsigned long long int p2, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
void bit_fade_fill(unsigned long long int p1, unsigned long long int *buf, unsigned long long int start, unsigned long long int end);
unsigned char bit_fade_chk(unsigned long long int p1, unsigned long long int *buf, unsigned long long int start, unsigned long long int end, unsigned char stop_after_err);
void wait (unsigned int sec);

#endif /* _TEST_H_ */
