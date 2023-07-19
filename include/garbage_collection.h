#pragma once

// Deletes all unmarked allocations & unmarks everything
void collect();
// Marks all objects root touches
void mark(Atom root);

// For keeping reference of everything created so no memory leak
struct Allocation
{
	struct Pair pair;
	bool mark : true;
	struct Allocation* next;
};

size_t getBytesAllocated();
size_t getNextCollection();