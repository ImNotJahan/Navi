#include "../include/main.h"
#include "../include/garbage_collection.h"

void mark(Atom root)
{
	Allocation* allocation;

	if (!(root.type == Atom::PAIR
		|| root.type == Atom::CLOSURE
		|| root.type == Atom::EXPANSION)) return;

	allocation = (Allocation*)((char*)root.value.pair -
		offsetof(Allocation, pair));

	if (allocation->mark) return;

	allocation->mark = 1;

	mark(head(root));
	mark(tail(root));
}

void collect()
{
	Allocation* allocation, ** pair;

	mark(symbol_table);

	// Delete unmarked allocations
	pair = &global_allocations;

	while (*pair != NULL)
	{
		allocation = *pair;

		if (!allocation->mark)
		{
			*pair = allocation->next;
			delete allocation;
		}
		else
		{
			pair = &allocation->next;
		}
	}
}