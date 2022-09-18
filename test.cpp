extern "C" {
#include "table.h"
}
#include <unordered_map>
#include <chrono>

typedef struct {
    int key;
    int value;
} item;

int item_hash(void* v) {
    item* i = (item*) v;
    return i->key;
}

int item_cmp(void* v1, void* v2) {
    item* i1 = (item*)v1;
    item* i2 = (item*)v2;
    return i1->key - i2->key;
}

char* item_tostr(void* it) {
    item* i = (item*)it;
    char* s = (char*)malloc(50);
    sprintf(s, "item{key=%d, value=%d}", i->key, i->value);
    return s;
}

void ex1() {
    Table *table = Table_new(item_hash, item_cmp, item_tostr);
    auto x = new item[13] {
        {.key=3, .value=114},
        {.key=11, .value=514},
        {.key=19, .value=514},
        {.key=7, .value=514},
        {.key=6, .value=514},
        {.key=5, .value=514},
        {.key=4, .value=514},
        {.key=5, .value=114},
        {.key=6, .value=114},
        {.key=7, .value=114},
        {.key=8, .value=114},
        {.key=9, .value=114},
        {.key=10, .value=114},
    };
    Table_set(table, &x[0]);
    Table_set(table, &x[1]);
    Table_set(table, &x[2]);
    Table_set(table, &x[3]);
    Table_set(table, &x[4]);
    Table_set(table, &x[5]);
    Table_set(table, &x[6]);
    Table_del(table, &x[4]);
    Table_del(table, &x[5]);
    Table_del(table, &x[6]);
    Table_set(table, &x[7]);
    Table_set(table, &x[8]);
    Table_set(table, &x[9]);
    Table_set(table, &x[10]);
    Table_set(table, &x[11]);
    Table_set(table, &x[12]);
    Table_debugprint(table);
//    printf("%p\n", Table_find(table, 2));
//    printf("%p\n", Table_find(table, 3));
//    printf("%p\n", Table_find(table, 11));
//    printf("%zd", table->size);
    Table_free(table);
    delete[] x;
}

void ex2() {
    Table *table = Table_new(item_hash, item_cmp, item_tostr);
    auto x = new item[3] {
        {.key=0, .value=1},
        {.key=8, .value=2},
        {.key=16, .value=3},
    };
    Table_set(table, &x[0]);
    Table_set(table, &x[1]);
    Table_set(table, &x[2]);
    Table_debugprint(table);
    printf("------------------\n");
    item finder = {.key = 0};
    printf("%d\n", ((item*)Table_find(table, &finder))->value);
    finder.key = 8;
    printf("%d\n", ((item*)Table_find(table, &finder))->value);
    finder.key = 16;
    printf("%d\n", ((item*)Table_find(table, &finder))->value);
    printf("------------------\n");
    Table_del(table, &x[1]);
    Table_del(table, &x[2]);
    Table_del(table, &x[0]);
    Table_debugprint(table);
    Table_free(table);
    delete[] x;
}

int test0() {
    srand(114514);
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        int value = rand();
    }
    for (int i = 0; i < 20000; i++) {
        int key = rand();
    }
    return 0;
}

int test1() {
    Table *table = Table_new(item_hash, item_cmp, nullptr);
    srand(114514);
    int count = 0;
    for (int i = 0; i < 20000; i++) {
        int key = rand();
        int value = rand();
//        Table_set(table, key, value);
    }
    for (int i = 0; i < 20000; i++) {
        int key = rand();
//        if (Table_find(table, key)) count++;
    }
    // printf("len = %zd\n", table->size);
    // printf("count = %d\n", count);
    Table_free(table);
    return count;
}

int test2() {
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
    return count;
}

void timeit(int (*f)()) {
    auto start = std::chrono::high_resolution_clock::now();
    int count;
    for (int j = 0; j < 100; j++) count = f();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf("count = %d, time = %lf\n", count, duration);
}

int main() {
//    timeit(test0);
//    timeit(test1);
//    timeit(test2);
    // test1();
    // test2();
    ex1();
    printf("=============\n");
    ex2();
    return 0;
}
