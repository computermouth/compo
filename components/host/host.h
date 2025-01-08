
#ifndef __HOST_H__
#define __HOST_H__

#include <stdint.h>

typedef struct HostInner {
	int32_t value;
} HostInner;

typedef struct HostOuter {
	uint32_t incrementer;
	HostInner inner;
} HostOuter;

HostInner host_new_inner(int32_t value);
HostOuter * host_new_outer(uint32_t incrementer, HostInner inner);
void host_outer_inc_inner(HostOuter * outer);
void host_outer_free(HostOuter * outer);

#endif
