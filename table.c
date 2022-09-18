#include "table.h"

Table *Table_resize(Table *this);

uint32_t Table_hash(TKey val) {
    return val;
}

Table *Table_new(THashFunc hashFunc, TCmpFunc cmpFunc, TToStrFunc toStrFunc) {
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
    this->hashFunc = hashFunc;
    this->cmpFunc = cmpFunc;
    this->toStrFunc = toStrFunc;
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

Table *Table_add_with_key(Table *this, TKey key, TValue value) {
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

Table *Table_add(Table *this, TValue value) {
  return Table_add_with_key(this, this->hashFunc(value), value);
}

TValue Table_find(Table *this, TValue vKey) {
    TKey key = this->hashFunc(vKey);
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // occupied by other key
    if (Table_hash(node->key) % this->capacity != pos) {
        return NULL;
    }
    // find node
    while (node && !node->empty) {
        if (node->key == key && this->cmpFunc(vKey, node->value) == 0) {
            return node->value;
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
        Table_add_with_key(&table, this->node[i].key, this->node[i].value);
    }
    // Table_debugprint(&table);
    free(this->node);
    this->size = table.size;
    this->node = table.node;
    this->capacity = table.capacity;
    this->lastfree = table.lastfree;
    return this;
}

Table *Table_del(Table *this, TValue vKey) {
    TKey key = this->hashFunc(vKey);
    size_t pos = Table_hash(key) % this->capacity;
    Node *node = this->node + pos;
    // occupied by other key
    if (Table_hash(node->key) % this->capacity != pos) {
        return this;
    }
    // if head is what we want
    if (node->key == key && this->cmpFunc(vKey, node->value) == 0) {
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
    Node *prev = node;
    while (prev->next) {
        if (prev->next->key == key && this->cmpFunc(vKey, prev->next->value) == 0) {
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

Table *Table_set(Table *this, TValue value) {
    TValue node = Table_find(this, value);
    // todo: need to consider collision here, currently just delete first
    if (node) {
        Table_del(this, node);
    }
    Table_add(this, value);
    return this;
}

void Table_free(Table *this) {
    free(this->node);
    free(this);
}

void Table_debugprint(Table *this) {
    for (size_t i = 0; i < this->capacity; i++) {
        Node *node = this->node + i;
        long pos = node->next - this->node;
        if (pos < 0 || pos >= this->capacity) pos = -1;
        if (!node->empty) {
            if (!this->toStrFunc)
                printf("Node[%zd](key=%d, value=addr(%p), next=%ld)\n", i, node->key, node->value, pos);
            else {
                char* s = this->toStrFunc(node->value);
                printf("Node[%zd](key=%d, value=%s, next=%ld)\n", i, node->key, s, pos);
                free(s);
            }
        } else {
            printf("Node[%zd]()\n", i);
        }
    }
}
