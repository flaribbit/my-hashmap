#include <stdlib.h>
#include <stdint.h>

typedef int TKey;
typedef int TValue;

typedef struct {
    TKey key;
    TValue value;
    Node *next;
} Node;

typedef struct {
    Node *node;
    Node *lastfree;
    size_t size;
    size_t capacity;
} Table;

uint32_t Table_hash(TKey val) {
    return val;
}

Table *Table_new() {
    Table *table = malloc(sizeof(Table));
    if (!table) return NULL;
    table->size = 0;
    table->capacity = 8;
}

Table *Table_add(Table *this, TKey key, TValue value) {
    size_t pos = Table_hash(key) % this->capacity;
}

Table *Table_set(Table *this, TKey key, TValue value) {
}
