#include <stdio.h>
#include <stdint.h>

uint8_t compute_parity(uint16_t packet)
{
	uint8_t count = 0;

	for (int i = 0; i < 15; i++)
	{
		count ^= packet & 1;
		packet >>= 1;
	}

	if (count % 2 == 0)
		return 0;
	else
		return 1;
}

int main()
{
	for (int i = 0; i < 16384; i++)
	{
		printf("i: %x\t parity: %d\n", i, compute_parity(i));
	}

	return 0;
}