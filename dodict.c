// maximize leaf node size.
// pid, portable id.
// turn pid to pointer.

#include <stdlib.h>
#include <stdio.h>

// A data oriented dict from key to key.
// Pack keys tight because searching through those is the most common use case.
typedef struct {
    int keypair_count;
    int keypair_size;
    char** keys;
    char** values;
} DOdict;

void
dodict_allocate(DOdict* dict, int num_keypairs)
{
    dict->keypair_count = 0;
    dict->keypair_size = num_keypairs;
    dict->keys = (char**)malloc(num_keypairs * sizeof(char*) * 2);
    dict->values = dict->keys + num_keypairs;
}

int
dodict_add(DOdict* dict, char* key, char* value)
{
    if (dict->keypair_count == dict->keypair_size) {
        return -1;
    }
    
    int kvp = dict->keypair_count++;
    dict->keys[kvp] = key;
    dict->values[kvp] = value;

    return 0;
}

char*
dodict_get(DOdict* dict, char* key)
{
    // Linear search of the keys but they are tightly packed in memory so we good.
    for (int i=0; i<dict->keypair_count; i++) {
        if (dict->keys[i] == key) {
            return dict->values[i];
        }
    }

    return NULL;
}


/* int main(void) */
/* { */
/*     char* kvps[] = { */
/*         "foo", "foo_val", */
/*         "bar", "bar_val", */
/*         "baz", "baz_val", */
/*         "wudup", "wudup_val", */
/*         NULL */
/*     }; */

/*     DOdict dict; */
/*     dodict_allocate(&dict, 4); */

/*     int i = 0; */
/*     char* key; */
/*     while((key = kvps[i++]) != NULL) { */
/*         char* val = kvps[i++]; */
/*         dodict_add(&dict, key, val); */
/*     } */

/*     printf("baz: %s", dodict_get(&dict, "baz")); */
    
/*     return 0; */
/* } */
