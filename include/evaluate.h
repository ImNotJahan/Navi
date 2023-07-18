#pragma once

Error evaluate_expr(Atom expr, Atom environment, Atom* result);
Error evaluate_do_execution(Atom* stack, Atom* expr, Atom* environment);
Error evaluate_do_binding(Atom* stack, Atom* expr, Atom* environment);
Error evaluate_do_applying(Atom* stack, Atom* expr, Atom* environment, Atom* result);
Error evaluate_do_returning(Atom* stack, Atom* expr, Atom* environment, Atom* result);