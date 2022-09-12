#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int TKey;
typedef int TValue;

typedef struct Node {
    bool empty;
    TKey key;
    TValue value;
    struct Node *next;
} Node;

typedef struct {
    size_t size;
    size_t capacity;
    Node *node;
    Node *lastfree;
} Table;

Table *Table_new();
Node *Table_find(Table *, TKey key);
Table *Table_del(Table *, TKey key);
Table *Table_set(Table *, TKey key, TValue value);
void Table_free(Table *);
void Table_debugprint(Table *);
