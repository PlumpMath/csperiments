#include <stdio.h>

#define ARRAY_COUNT(x) (sizeof(x)) / (sizeof(x[0]))

#define uint32 unsigned int

typedef struct _MyThing{
    uint32 key;
    uint32 value;
    
    struct _MyThing *next_in_hash;
} MyThing;

typedef struct {
    uint32 thing_count;
    MyThing thing_stack[100];
    MyThing* first_free_my_thing;
    // Note(stephen): Must be power of 2 because of terrible hash function.
    MyThing* hash_map[16];
} MyThingStore;

MyThing* push_thing(MyThingStore* store)
{
    MyThing* new_thing = 0;
    if (store->first_free_my_thing) {
        new_thing = store->first_free_my_thing;
        store->first_free_my_thing = new_thing->next_in_hash;
    } else {
        new_thing = store->thing_stack + store->thing_count++;
    }
    return new_thing;
}

void add_thing(MyThingStore* storage, uint32 key, uint32 value)
{
    MyThing* found = 0;
    uint32 hash_bucket = key & (ARRAY_COUNT(storage->hash_map) - 1);
    for (MyThing* thing = storage->hash_map[hash_bucket];
         thing;
         thing = thing->next_in_hash) {
        if (thing->key == key) {
            found = thing;
            found->value = value;
            return;
        }
    }

    found = push_thing(storage);
    found->key = key;
    found->value = value;
    found->next_in_hash = storage->hash_map[hash_bucket];
    storage->hash_map[hash_bucket] = found;
}

int get_thing(MyThingStore* store, uint32 key)
{
    uint32 hash_bucket = key & (ARRAY_COUNT(store->hash_map) - 1);
    for (MyThing* thing = store->hash_map[hash_bucket];
         thing;
         thing = thing->next_in_hash) {
        if (thing->key == key) {
            return thing->value;
        }
    }
    return -1;
}

void remove_thing(MyThingStore* store, uint32 key)
{
    uint32 hash_bucket = key & (ARRAY_COUNT(store->hash_map) - 1);
    for (MyThing** thing = &store->hash_map[hash_bucket];
         *thing;
         thing = &(*thing)->next_in_hash) {
        if ((*thing)->key == key) {
            MyThing* removed_thing = *thing;
            *thing = (*thing)->next_in_hash;
            removed_thing->next_in_hash = store->first_free_my_thing;
            store->first_free_my_thing = removed_thing;
            break;
        }
    }
}

int main(void)
{
    MyThingStore store = {.thing_count = 0,
                          .thing_stack = {},
                          .hash_map = {}};

    add_thing(&store, 13, 99);
    add_thing(&store, 12, 82);
    add_thing(&store, 14, 7);
    add_thing(&store, 12, 15);
    add_thing(&store, 15, 44);

    int result = get_thing(&store, 12);
    printf("This should be 15: %i\n", result);

    remove_thing(&store, 12);
    
    result = get_thing(&store, 12);
    printf("This should be -1: %i\n", result);
}

// these notes pertain to casey's double entity hash thing, not the simple hash table I made above.

// Casey is writing an index on 2 things. He wants to be able to clear all the rules for one of
// an index. Easy in sql, hard in a standard hashmap.

// Do the hash in both directions?
// Just know which one you want to remove?

// Right now you'd have to tablescan to remove all the rules for one person.

// This is a problem that WILL come up with goats, double indexed table.


// Also! for removal, he does this thing called
// MyThing* first_free_my_thing
// where he keeps a list of deleted ones.
// So that when he allocates a new one he pulls it from that list instead of pushing onto the stack.


