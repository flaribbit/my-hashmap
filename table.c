#include "table.h"

Table *Table_resize(Table *this);

uint32_t Table_hash(TKey val) {
    return val;
}

Table *Table_new() {
    Table *this = malloc(sizeof(Table));
    if (!this) return NULL;
    this->size = 0;
    this->capacity = 8;
    this->node = malloc(sizeof(Node) * this->capacity);
    this->lastfree = this->node + this->capacity - 1;
    if (!this->node) {
        printf("failed to malloc nodes");
        return NULL;
    }
    for (size_t i = 0; i < this->capacity; i++) {
        this->node[i].empty = true;
    }
    return this;
}

void Table_find_free(Table *this) {
    while (!this->lastfree->empty) {
        this->lastfree--;
        if (this->lastfree < this->node) {
            this->lastfree = this->node + this->capacity - 1;
        }
    }
}

Table *Table_add(Table *this, TKey key, TValue value) {
    if (this->size == this->capacity) {
        Table_resize(this);
    }
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // case 1: target node is empty
    if (node->empty) {
        node->key = key;
        node->value = value;
        node->empty = false;
        node->next = NULL;
        this->size++;
        return this;
    }
    // case 2: target node is occupied by other hash
    size_t node_pos = Table_hash(node->key) % this->capacity;
    if (node_pos != pos) {
        // find its prev node
        Node *p = this->node + node_pos;
        while (p) {
            if (p->next == node) break;
            p = p->next;
        }
        // ... then move to free node
        Table_find_free(this);
        p->next = this->lastfree;
        *this->lastfree = *node;
        node->key = key;
        node->value = value;
        node->empty = false;
        node->next = NULL;
        this->size++;
        return this;
    }
    // case 3: collision resolution
    Table_find_free(this);
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
    Table table;
    table.size = 0;
    table.capacity = this->capacity * 2;
    table.node = malloc(sizeof(Node) * table.capacity);
    table.lastfree = table.node + table.capacity - 1;
    for (size_t i = 0; i < table.capacity; i++) {
        table.node[i].empty = true;
    }
    for (size_t i = 0; i < this->size; i++) {
        Table_add(&table, this->node[i].key, this->node[i].value);
    }
    // Table_debugprint(&table);
    free(this->node);
    this->size = table.size;
    this->node = table.node;
    this->capacity = table.capacity;
    this->lastfree = table.lastfree;
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
            prev->next->empty = true;
            prev->next = prev->next->next;
            this->size--;
            return this;
        }
        prev = prev->next;
    }
    // not found
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
    free(this->node);
    free(this);
}

void Table_debugprint(Table *this) {
    for (size_t i = 0; i < this->capacity; i++) {
        Node *node = this->node + i;
        int pos = node->next - this->node;
        if (pos < 0 || pos >= this->capacity) pos = -1;
        if (!node->empty) {
            printf("Node[%zd](key=%d, value=%d, next=%d)\n", i, node->key, node->value, pos);
        } else {
            printf("Node[%zd]()\n", i);
        }
    }
}

void test() {
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
    Table_set(table, 8, 114);
    Table_set(table, 9, 114);
    Table_set(table, 10, 114);
    Table_debugprint(table);
    printf("%p\n", Table_find(table, 2));
    printf("%p\n", Table_find(table, 3));
    printf("%p\n", Table_find(table, 11));
    printf("%zd", table->size);
    Table_free(table);
}
