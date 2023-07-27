#pragma once

struct Frame;

Error evaluate_expr(Atom expr, Atom environment, Atom* result);
Error evaluate_do_execution(Frame* stack, Atom* expr, Atom* environment);
Error evaluate_do_binding(Frame* stack, Atom* expr, Atom* environment);
Error evaluate_do_applying(Frame* stack, Atom* expr, Atom* environment, Atom* result);
Error evaluate_do_returning(Frame* stack, Atom* expr, Atom* environment, Atom* result);