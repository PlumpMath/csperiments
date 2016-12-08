#include <stdio.h>
#include <stdlib.h>

struct hash_val {
    int value;
    struct hash_val* next_in_hash;
};

struct hash_map {
    int num_elements;
    struct hash_val seen_elements[8];
    struct hash_val* seen_element_hash[8];
};

// returns 1 if value is already in hash.
// adds value to hash and returns 0 if value is not in hash.
int
is_in_hash(struct hash_map* map, int n)
{
    int hash_key = n & 7;
    int seen = 0;
    for (struct hash_val* val = map->seen_element_hash[hash_key];
         val;
         val = val->next_in_hash) {
        if (val->value == n) {
            seen = 1;
        }
    }
        
    if (!seen) {
        struct hash_val* new_hash_val = map->seen_elements + map->num_elements++;
        new_hash_val->value = n;
        new_hash_val->next_in_hash = map->seen_element_hash[hash_key];
        map->seen_element_hash[hash_key] = new_hash_val;
        return 0;
    }

    return 1;
}

int main(void)
{
    int array_1[5] = {1, 27, 39, 44, 58};
    int array_2[3] = {44, 58, 69};

    int result_c = 0;
    int result[8];

    struct hash_map map;
    map.num_elements = 0;
    for (int i=0;i<8;i++) {
        map.seen_element_hash[i] = NULL;
    }

    for (int i=0;i<5; i++) {
        int n = array_1[i];
        if (!is_in_hash(&map, n)) {
            result[result_c++] = n;
        }
    }

    for (int i=0;i<3; i++) {
        int n = array_2[i];
        if (!is_in_hash(&map, n)) {
            result[result_c++] = n;
        }
    }

    printf("Result: {");
    for (int i=0;i<result_c;i++) {
        printf("%i", result[i]);
        if (i+1 != result_c) {
            printf(", ");
        }
    }
    printf("}\n");    
}
