#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <vector>
#include <typeinfo>

template <typename T>
static void t(void)
{
    printf("std::vector<%s>().max_size() = %zu\t SIZE_MAX / sizeof(%s) = %zu\n",
            typeid(T).name(), std::vector<T>().max_size(),
            typeid(T).name(), SIZE_MAX / sizeof(T));
}

int main(void)
{
    printf("SIZE_MAX / sizeof(char) / 2 = %zu\n", SIZE_MAX / sizeof(char) / 2);
    t<int>();
    t<char>();
    t<long long>();
    return 0;
}
