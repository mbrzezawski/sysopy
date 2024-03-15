#include <stdio.h>

int collatz_conjecture(int input) {
    if (input % 2 == 0) {
        return input / 2;
    } else {
        return 3 * input + 1;
    }
}

int test_collatz_convergence(int input, int max_iter) {
    int iterations = 0;

    while (input != 1 && iterations < max_iter) {
        input = collatz_conjecture(input);
        iterations++;
    }

    if (input == 1) {
        return iterations;
    } else {
        return -1;
    }
}