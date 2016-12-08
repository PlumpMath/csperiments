// requires theft git@github.com:silentbicycle/theft.git
#include <theft.h>

#include "dodict.c"

// This is the sort of structure that would be pretty great to generate.
typedef enum {
    OP_ADD = 0,
    OP_GET = 1,
} Operation_Type;

static int num_operation_types = 2;

typedef struct {
    char* key;
    char* value;
    
    int result;
} OperationAdd;

typedef struct {
    char* key;
    
    char* result;
} OperationGet;

typedef struct {
    Operation_Type type;
    union {
        OperationAdd add;
        OperationGet get;
    };
} Operation;

typedef struct {
    int operation_count;
    Operation* operations;
} OperationList;

char*
allocate_rand_string(struct theft *t)
{

    int length = (int)abs(theft_random(t) % 128);
    char* rand_string = malloc(length);
    for (int i=0; i < length-1; i++) {
        // get rand char
        rand_string[i] = (char)((theft_random(t) % (126 - 33) + 1) + 32);
    }
    rand_string[length] = 0;

    return rand_string;
}

void*
allocate_operation_list(struct theft *t, theft_seed seed, void *env)
{
    // Creat a list of random operations.
    int list_length = (seed % 3) + 1;
    OperationList* op_list = malloc(sizeof(OperationList));
    op_list->operation_count = 0;
    op_list->operations = malloc(sizeof(Operation) * list_length);

    int add_operations_count = 0;
    Operation* add_operations[list_length];

    int get_operations_count = 0;
    Operation* get_operations[list_length];

    for (int i=0; i < list_length; i++) {

        Operation_Type type = theft_random(t) % num_operation_types;
        Operation* op = op_list->operations + op_list->operation_count++;
        op->type = type;

        switch(type) {
        case(OP_ADD): {
            op->add.key = allocate_rand_string(t);
            op->add.value = allocate_rand_string(t);


            add_operations[add_operations_count++] = op;
        } break;
        case(OP_GET): {
            int get_known_key = theft_random(t) % 2;
            if (get_known_key && add_operations_count != 0) {
                // Pull one of the keys we already set.
                int add_index = (int)(theft_random(t) % add_operations_count);
                op->get.key = add_operations[add_index]->add.key;
                
            } else {
                op->get.key = allocate_rand_string(t);
            }

            get_operations[get_operations_count++] = op;
        } break;
        };

    }

    return op_list;
}

void
apply_op_list(DOdict* dict, OperationList* op_list)
{
    for (int i=0; i < op_list->operation_count; i++) {
        Operation* op = op_list->operations + i;
        switch(op->type) {
        case(OP_ADD): {
            op->add.result = dodict_add(dict, op->add.key, op->add.value);
        } break;
        case(OP_GET): {
            op->get.result = dodict_get(dict, op->get.key);
        } break;
        };
    }
}

static theft_trial_res
this_shit_runs(OperationList *op_list)
{
    DOdict dict;
    dodict_allocate(&dict, 512);

    for (int i=0; i < op_list->operation_count; i++) {
        Operation* op = op_list->operations + i;
        switch(op->type) {
        case(OP_ADD): {
            op->add.result = dodict_add(&dict, op->add.key, op->add.value);
        } break;
        case(OP_GET): {
            op->get.result = dodict_get(&dict, op->get.key);
        } break;
        };
    }

    return THEFT_TRIAL_PASS;
}

// allocate a random 

static struct theft_type_info DOdict_operation_list_info = {
    .alloc = allocate_operation_list,
};

int main() {
    struct theft *t = theft_init(0);
    struct theft_cfg cfg = {
        .name = __func__,

        .fun = this_shit_runs,

        .type_info = { &DOdict_operation_list_info },

        .trials = 100,
    };

    theft_run_res result = theft_run(t, &cfg);
    theft_free(t);
    if (result == THEFT_RUN_PASS) {
        fprintf(stderr, "Tests Passed");
    } else {
        fprintf(stderr, "Tests Failed");
    }
    return result == THEFT_RUN_PASS;
}
