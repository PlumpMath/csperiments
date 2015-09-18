#include <stdio.h>

struct kvp
{
    int code;
    char* name;
};

char*
get(struct kvp* kvps, int key) {
    for (int i = 0; 0 != kvps[i].code; i++) {
        if (key == kvps[i].code) {
            return kvps[i].name;
        }
    }
    return 0;
}

int main(void) {
    struct kvp entities[] = {
        {1, "hello"},
        {2, "world"},
        {0, NULL},
    };
    
    for (int i = 0; i < sizeof(entities)/sizeof(struct kvp); i++) {
        struct kvp entity = entities[i];
        printf("%i, %s\n", entity.code, entity.name);
    }

    printf("key 2 is %s", get(entities, 2));    
}
