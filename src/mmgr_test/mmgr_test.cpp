
#include <iostream>
#include "../framework/memory.hpp"

int main(int argc, char const *argv[])
{
    void * mem = mem_alloc(64);

    mem_free(mem);

    return 0;
}
