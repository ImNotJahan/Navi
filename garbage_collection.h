#pragma once

struct Allocation
{
	Pair pair;
	int mark : 1;
	Allocation* next;
	int length;
};

static Allocation* global_allocations = NULL;

void mark(Atom root);
void collect();

static Atom symbol_table = null;