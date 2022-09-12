extern "C" {
#include "table.h"
}
#include <unordered_map>
#include <chrono>

void test0() {
    srand(114514);
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        int value = rand();
    }
    for (int i = 0; i < 20000; i++) {
        int key = rand();
    }
}

void test1() {
    Table *table = Table_new();
    srand(114514);
    int count = 0;
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        int value = rand();
        Table_set(table, key, value);
    }
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        if (Table_find(table, key)) count++;
    }
    // printf("len = %zd\n", table->size);
    // printf("count = %d\n", count);
    Table_free(table);
}

void test2() {
    std::unordered_map<int, int> table;
    srand(114514);
    int count = 0;
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        int value = rand();
        table[key] = value;
    }
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        if (table.find(key) != table.end()) count++;
    }
    // printf("len = %zd\n", table.size());
    // printf("count = %d\n", count);
}

void timeit(void (*f)()) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < 100; j++) f();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf("time = %lf\n", duration);
}

int main() {
    // timeit(test0);
    timeit(test1);
    timeit(test2);
    return 0;
}
