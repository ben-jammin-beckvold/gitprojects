//Ben Beckvold
//HW 7
/*
	The main logic is setup for either an associative or direct mapped cache.
	
        Originally the global variables in all caps were #define macros but i
        couldn't come up with a way of using them though multiple test runs and 
        have no user input.
        The program run from the command line will expect 3 arguments:
        cachesize setsize cachelinesize

        cachesize and cachelinesize will be in bytes.
        setsize is 1 for Direct Mapped and is 0 for fully associative.
*/
#include <stdio.h>
#include <time.h>
#include <math.h>   //for the log() function

//globals
#define MAXCACHELINES 128    //max number of cache lines
unsigned int SETSIZE; 	//size of the sets (direct mapped = number of lines; fully associative = 1)
unsigned int CACHESIZE;     //size of cache in bytes
unsigned int CACHELINESIZE; //size of each cache line in bytes
unsigned int NUMCACHELINES; //number of cache lines
unsigned int NUMSETS;	    //number of sets

//to make boolean logic simpler
typedef int bool;
#define true 1
#define false 0	

//test run input
#define TESTSIZE 39
#define NUMRUNS 4
const int testSet[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72,
76, 80, 0, 4, 8, 12, 16, 71, 3, 41, 81, 39, 38, 71, 15, 39, 11, 51, 57, 41 };

// each group of 3 params represent {cacheSize, blockSize, setSize}
// setSize is 0 for direct mapped, and 1 for fully associative.
const int cacheParams[NUMRUNS][3] = {{128, 8, 0},{64, 8, 2},{128, 16, 0},{64, 8, 1}};

//log base 2 function for determining number of bits
const unsigned int logBase2(const int x)
{
	return (unsigned int) (log(x) / log(2));
}

unsigned int startTime;

struct row {
	bool valid;
	unsigned int tag;
	unsigned int timeStamp;
}; 
struct row cache[MAXCACHELINES] = { false, 0, 0 };

bool isHitOrMiss(int tag, int set);

void param_init(int runNum)
{
    printf("\n-----Run number: %d-----\n", runNum + 1);
    CACHESIZE = cacheParams[runNum][0];
    printf("Cache Size: %d\n", CACHESIZE);
    CACHELINESIZE = cacheParams[runNum][1];
    printf("Block Size: %d\n", CACHELINESIZE);
    NUMCACHELINES = (CACHESIZE / CACHELINESIZE);
    
    SETSIZE = cacheParams[runNum][2];
    printf("Set Size: %d\n", SETSIZE);
    if (SETSIZE == 0)
        SETSIZE = NUMCACHELINES;

    NUMSETS =  (NUMCACHELINES / SETSIZE);

} //end param_init

int main() {
    int runNumber = 0;
    while(runNumber < NUMRUNS)
    {
	startTime = clock();
        param_init(runNumber);
        
        //is it a Direct Mapped, Fully Associative, or Set Associative
	bool directMap = false, fullyAssoc = false;
	unsigned int setBits = 1, setShift = logBase2(SETSIZE);
	for (unsigned int i = 1; i < setShift; i++)
		setBits = (setBits << 1) | 1;

	const unsigned int indexBits = logBase2(SETSIZE);
	if (SETSIZE == 1)
		fullyAssoc = true;
	else if (SETSIZE == NUMCACHELINES)
		directMap = true;
	//if both are false it's Set Associative

	//process test addresses
	const unsigned int offsetBits = logBase2(CACHELINESIZE);
	unsigned int testNum, address, tag, index, set;
	for (testNum = 0; testNum < TESTSIZE; testNum++)
	{
		address = testSet[testNum];
		//parse the address using bitwise operators
		tag = address >> (offsetBits + setBits);
		set = (address >> offsetBits) & setBits;
		if (directMap || fullyAssoc)
			index = set;
		else
			index = (address >> offsetBits) & (1 << indexBits);

		//is the address in the cache
		if (isHitOrMiss(tag, set))
			printf("address(%d) Hit\n", address);
		else
			printf("address(%d) Miss\n", address);

	}

        runNumber++;
    }// end each test run 
	getchar();
	return 0;
}//end main

bool isHitOrMiss(int tag, int set) {
	bool isHit = false;
	int rowIdx;
        
        printf("set: %d\ttag: %d", set, tag);

	if (SETSIZE == NUMCACHELINES)	//direct mapped
	{
		//check indexed line for a match, return if found
		if (cache[set].valid && cache[set].tag == tag)
			return true;
		else //replace the data with the line
		{
			cache[set].valid = true;
			cache[set].tag = tag;
			cache[set].timeStamp = clock() - startTime;
			return false;
		}
	}
	else	//associative (fully and set)
	{
		for (rowIdx = 0; rowIdx < SETSIZE; rowIdx++)
		{
			if (cache[(rowIdx + set * SETSIZE)].valid && cache[(rowIdx + set * SETSIZE)].tag == tag)
			{
				cache[(rowIdx + set * SETSIZE)].timeStamp = clock() - startTime;
				return true;
			}
		}

		//eviction replacement
		unsigned int oldestTime = 0;
		unsigned int oldestRow = 0;
		// find the oldest cache line and evict, only for Fully Associative (FIFO)
		for (rowIdx = 0; rowIdx < SETSIZE; rowIdx++)
		{
			if (cache[rowIdx + set * SETSIZE].timeStamp > oldestTime)
			{
				oldestRow = rowIdx + set * SETSIZE;
				oldestTime = cache[rowIdx + set * SETSIZE].timeStamp;
			}
		}
		cache[oldestRow].valid = true;
		cache[oldestRow].tag = tag;
		cache[oldestRow].timeStamp = clock() - startTime;

	}	//end associated replacement/eviction

	//no cache hit, but stored the new address
	return false;
}

