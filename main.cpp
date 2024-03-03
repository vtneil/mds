#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"

using aligned_int = container::aligned_t<int, 128>;

constexpr size_t Size = sizeof(aligned_int);


int main() {
    io::println(Size);
}
