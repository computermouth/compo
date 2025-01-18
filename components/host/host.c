
#include "host.h"

#include <stdio.h>
#include <stdlib.h>

HostInner host_new_inner(int32_t value);
HostOuter * host_new_outer(uint32_t incrementer, HostInner inner);
void host_outer_inc_inner(HostOuter * outer);
void host_outer_free(HostOuter * outer);

HostInner host_new_inner(int32_t value) {
	return (HostInner){.value = value};
}

HostOuter * host_new_outer(uint32_t incrementer, HostInner inner) {
	HostOuter * out = calloc(1, sizeof(HostOuter));
	*out = (HostOuter){.incrementer = incrementer, .inner = inner};
	return out;
}

void host_outer_inc_inner(HostOuter * outer) {
	outer->inner.value += outer->incrementer;
	fprintf(stderr, "o->j.value: %u\n", outer->inner.value);
}

void host_outer_free(HostOuter * o) {
	free(o);
}

// wasi-sdk compiler (as wasm32-wasi -- no main)
// wit bindgen {h, c, o}
// wasm-tools component new

// -Wl,--no-entry
// -nostdlib

/*
int main(){
	HostInner inner = host_new_inner(4);
	HostOuter * outer = host_new_outer(7, inner);
	host_outer_inc_inner(outer);
	fprintf(stderr, "o->inner.value: %u\n", outer->inner.value);
	host_outer_free(outer);
	return 0;
}
*/
