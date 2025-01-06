
#include "some.h"

#include <stdio.h>
#include <stdlib.h>

Other * new_other(uint32_t u32, Junk j);
void inc_other_b(Other * o);

Junk new_junk(int32_t value) {
	return (Junk){.value = value};
}

Other * new_other(uint32_t incrementer, Junk j) {
	Other * out = calloc(1, sizeof(Other));
	*out = (Other){.incrementer = incrementer, .j = j};
	return out;
}

void inc_other_junk_value(Other * o) {
	o->j.value += o->incrementer;
	fprintf(stderr, "o->j.value: %u\n", o->j.value);
}

/*
int main(){
	Junk j = new_junk(4);
	Other * o = new_other(7, j);
	inc_other_junk_value(o);
	fprintf(stderr, "o->j.value: %u\n", o->j.value);
}
*/
