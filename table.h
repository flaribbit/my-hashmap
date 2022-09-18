#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int TKey;
typedef void* TValue;

typedef TKey (*THashFunc)(TValue val);
typedef int (*TCmpFunc)(TValue val1, TValue val2);
typedef char* (*TToStrFunc)(TValue val);

typedef struct Node {
    bool empty;
    TKey key;
    TValue value;
    struct Node *next;
} Node;

typedef struct Table {
    size_t size;
    size_t capacity;
    Node *node;
    Node *lastfree;

    THashFunc hashFunc;
    TCmpFunc cmpFunc;
    TToStrFunc toStrFunc;
} Table;

Table *Table_new(THashFunc hashFunc, TCmpFunc cmpFunc, TToStrFunc toStrFunc);
Table *Table_add(Table *, TValue value);
TValue Table_find(Table *, TValue val);
Table *Table_del(Table *, TValue key);
Table *Table_set(Table *, TValue value);
void Table_free(Table *);
void Table_debugprint(Table *);
