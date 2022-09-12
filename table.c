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
    Node *node;
    Node *lastfree;
    size_t size;
    size_t capacity;
} Table;

uint32_t Table_hash(TKey val) {
    return val;
}

Table *Table_new() {
    Table *this = malloc(sizeof(Table));
    if (!this) return NULL;
    this->size = 0;
    this->capacity = 8;
    this->node = malloc(sizeof(Node) * this->capacity);
    if (!this->node) {
        printf("failed to malloc nodes");
        return NULL;
    }
    this->lastfree = this->node + this->capacity - 1;
    for (size_t i = 0; i < this->capacity; i++) {
        this->node[i].empty = true;
    }
    return this;
}

Table *Table_add(Table *this, TKey key, TValue value) {
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // 没有冲突 直接存储
    if (node->empty) {
        node->key = key;
        node->value = value;
        node->empty = false;
        node->next = NULL;
        this->size++;
        // update lastfree pointer
        while (!this->lastfree->empty) {
            this->lastfree--;
        }
        return this;
    }
    // 被别人占了
    size_t node_pos = Table_hash(node->key) % this->capacity;
    if (node_pos != pos) {
        // 找前一个节点
        Node *p = this->node + node_pos;
        while (p) {
            if (p->next == node) break;
            p = p->next;
        }  // 理论上p不是空指针
        p->next = this->lastfree;
        // 搬走
        *this->lastfree = *node;
        node->key = key;
        node->value = value;
        node->empty = false;
        node->next = NULL;
        this->size++;
        // update lastfree pointer
        while (!this->lastfree->empty) {
            this->lastfree--;
        }
        return this;
    }
    // 解决冲突
    node = this->lastfree;
    node->key = key;
    node->value = value;
    node->empty = false;
    node->next = NULL;
    this->size++;
    // insert to head
    Node *head = this->node + pos;
    node->next = head->next;
    head->next = node;
    // update lastfree pointer
    while (!this->lastfree->empty) {
        this->lastfree--;
    }
    return this;
}

Node *Table_find(Table *this, TKey key) {
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // occupied by other key
    if (Table_hash(node->key) % this->capacity != pos) {
        return NULL;
    }
    // find node
    while (node) {
        if (node->key == key) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

Table *Table_set(Table *this, TKey key, TValue value) {
    Node *node = Table_find(this, key);
    if (node) {
        node->value = value;
    } else {
        Table_add(this, key, value);
    }
    return this;
}

void Table_free(Table *this) {
    free(this);
}

void Table_debugprint(Table *this) {
    for (size_t i = 0; i < this->capacity; i++) {
        Node *node = this->node + i;
        printf("Node[%d](empty=%d, key=%d, value=%d, next=0x%p)\n", i, node->empty, node->key, node->value, node->next);
    }
}

int main() {
    Table *table = Table_new();
    Table_set(table, 3, 114);
    Table_set(table, 11, 514);
    Table_set(table, 19, 514);
    Table_set(table, 7, 514);
    Table_set(table, 6, 514);
    Table_set(table, 5, 514);
    Table_set(table, 4, 514);
    Table_debugprint(table);
    printf("%p\n", Table_find(table, 2));
    printf("%p\n", Table_find(table, 3));
    Table_free(table);
}
