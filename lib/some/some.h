
#ifndef __SOME_H__
#define __SOME_H__

#include <stdint.h>

typedef struct Junk {
	int32_t value;
} Junk;

typedef struct Other {
	uint32_t incrementer;
	Junk j;
} Other;

Junk new_junk(int32_t value);
Other * new_other(uint32_t incrementer, Junk j);
void inc_other_junk_value(Other * o);
void free_other(Other * o);

#endif
