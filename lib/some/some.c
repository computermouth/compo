
#include "some.h"

#include <stdio.h>
#include <stdlib.h>

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

void free_other(Other * o) {
	free(o);
}

/*
int main(){
	Junk j = new_junk(4);
	Other * o = new_other(7, j);
	inc_other_junk_value(o);
	fprintf(stderr, "o->j.value: %u\n", o->j.value);
	free_other(o);
	return 0;
}
*/
