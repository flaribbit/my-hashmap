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

void Table_update_lastfree(Table *this) {
    while (!this->lastfree->empty) {
        this->lastfree--;
        if (this->lastfree < this->node) {
            this->lastfree = this->node + this->capacity + 1;
        }
    }
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
        Table_update_lastfree(this);
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
        Table_update_lastfree(this);
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
    Table_update_lastfree(this);
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
    while (node && !node->empty) {
        if (node->key == key) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

Table *Table_resize(Table *this) {
    return this;
}

Table *Table_del(Table *this, TKey key) {
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // occupied by other key
    if (Table_hash(node->key) % this->capacity != pos) {
        return this;
    }
    // if head is what we want
    if (node->key == key) {
        if (node->next) {
            node->next->empty = true;
            node->key = node->next->key;
            node->value = node->next->value;
            node->next = node->next->next;
        } else {
            node->empty = true;
        }
        this->size--;
        return this;
    }
    // else find prev node and del next node
    Node *prev = NULL;
    while (prev->next) {
        if (prev->next->key == key) {
            break;
        }
        prev = prev->next;
    }
    prev->next->empty = true;
    prev->next = prev->next->next;
    this->size--;
    return this;
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
        printf("Node[%zd](empty=%d, key=%d, value=%d, next=0x%p)\n", i, node->empty, node->key, node->value, node->next);
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
    Table_del(table, 6);
    Table_del(table, 5);
    Table_del(table, 4);
    Table_set(table, 5, 114);
    Table_set(table, 6, 114);
    Table_set(table, 7, 114);
    Table_debugprint(table);
    printf("%p\n", Table_find(table, 2));
    printf("%p\n", Table_find(table, 3));
    printf("%p\n", Table_find(table, 11));
    printf("%zd", table->size);
    Table_free(table);
}
