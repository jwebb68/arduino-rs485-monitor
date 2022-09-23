// -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-

#include "panic.h"

#include "defs.h"

void panic0_(char const *file, int line)
{
    CONSOLE.println();
    CONSOLE.print(F("****************"));
    CONSOLE.print(F("panic:"));
    CONSOLE.print(file);
    CONSOLE.print(':');
    CONSOLE.print(line);
    CONSOLE.print(F("****************"));
    CONSOLE.println();
    while (true)
        ;
}
