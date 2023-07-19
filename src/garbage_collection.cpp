#include "../include/main.h"
#include "../include/garbage_collection.h"

void mark(Atom root)
{
	Allocation* alloc;

	if (!(root.type == Atom::PAIR || 
		root.type == Atom::CLOSURE || 
		root.type == Atom::EXPANSION)) return;
	
	alloc = (Allocation*) ((char*)root.value.pair - offsetof(Allocation, pair));
	
	if (alloc->mark) return; // Reached an already marked portion
	
	alloc->mark = 1; // Since something could still access this, mark it to avoid deletion

	// Keep marking everything that could be accessed
	mark(head(root));
	mark(tail(root));
}