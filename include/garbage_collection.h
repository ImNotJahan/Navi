#pragma once

// Deletes all unmarked allocations & unmarks everything
void collect();
// Marks all objects root touches
void mark(Atom root);

// For keeping reference of everything created so no memory leak
struct Allocation
{
	struct Pair pair;
	int mark : 1;
	struct Allocation* next;
};

#define MAX_ITER 100000