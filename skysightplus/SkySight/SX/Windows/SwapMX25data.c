#include <assert.h>
#include <string.h>

void Arrayswap (short *Imptr1, short *Imptr2, short height, short width, short *end1, short *end2)
{
#if 1
	int y,z;
// Imptr1 points to output array start, Imptr2 points to start of multiplexed image data

short *Impt1, *Impt2, *Impt5;

memset(Imptr1, 0, (end1-Imptr1)*sizeof(short));

Impt1=Imptr1;
Impt2=Imptr1+width;

Impt5=Imptr2;

for (y=1; y <= height/2; y++)
{		// data move
	for (z=1; z <= width/2; z++)
	{
		assert(Impt1 < end1);
		assert(Impt5 < end2);
		*Impt1++=*Impt5++;
		assert(Impt2 < end1);
		assert(Impt5 < end2);
		*Impt2++=*Impt5++;
		assert(Impt2 < end1);
		assert(Impt5 < end2);
		*Impt2++=*Impt5++;
		assert(Impt1 < end1);
		assert(Impt5 < end2);
		*Impt1++=*Impt5++;
	};
    Impt1+=width;
    Impt2+=width;
}
#else
memcpy(Imptr1, Imptr2, width * height * sizeof(short));
#endif
}
