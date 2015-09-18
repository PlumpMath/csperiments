#include <stdlib.h>

typedef struct {
    int foo;
    int bar;
} Element;


typedef struct {
    Element* array;
    size_t size;
    size_t used;
} ArrayStruct;


typedef struct {
    ArrayStruct array_struct;
} StructWithArray;


void
arraystruct_init(ArrayStruct* as)
{
    as->array = (Element*)malloc(5 * sizeof(Element));
    as->size = 5;
}


void
arraystruct_resize(ArrayStruct* as)
{
    if (as->used == as->size) {
        as->size *= 2;
        as->array = (Element*)realloc(as->array, as->size * sizeof(Element));
    }
}


void
arraystruct_add_element(ArrayStruct* as, int foo, int bar)
{
    arraystruct_resize(as);

    Element* element = &(as->array[as->used++]);
    element->foo = foo;
    element->bar = bar;
}


int main(int argc, char* argv[])
{
    StructWithArray *main_struct = calloc(1, sizeof(StructWithArray));
    arraystruct_init(&main_struct->array_struct);
    arraystruct_add_element(&main_struct->array_struct, 1, 2);
}
