#include <stdio.h>

#ifndef DYNAMIC_DLOPEN
#include "collatzlibrary/collatz.h"
#endif

#ifdef DYNAMIC_DLOPEN
#include <dlfcn.h>

    int (*collatz_conjecture)(int input);
    int (*test_collatz_convergence)(int input, int max_iter);
#endif

int main() {
    int numbers[] = {10, 22, 3, 7};
    int result, iterations;
    size_t numbers_count = sizeof(numbers) / sizeof(numbers[0]);

#ifdef DYNAMIC_DLOPEN
    void* dll_handle = dlopen("collatzlibrary/libcollatz.so", RTLD_LAZY);
    if (!dll_handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    *(void**) (&collatz_conjecture) = dlsym(dll_handle, "collatz_conjecture");
    *(void**) (&test_collatz_convergence) = dlsym(dll_handle, "test_collatz_convergence");

    if (!collatz_conjecture || !test_collatz_convergence) {
        fprintf(stderr, "Error loading symbols: %s\n", dlerror());
        dlclose(dll_handle);
        return 1;
    }
#endif

    for (size_t i = 0; i < numbers_count; i++) {
        result = collatz_conjecture(numbers[i]);
        iterations = test_collatz_convergence(numbers[i], 100);

        printf("Collatz Conjecture for %d: %d\n", numbers[i], result);
        printf("Collatz Convergence test for %d: %d iterations\n", numbers[i], iterations);
    }

#ifdef DYNAMIC_DLOPEN
    dlclose(dll_handle);
#endif

    return 0;
}
