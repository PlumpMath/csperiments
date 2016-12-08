#include <stdio.h>
#include <assert.h>

union V2 {
    struct {
        float x, y;
    };
    float e[2];
};

union V3 {
    struct {
        float x, y, z;
    };
    struct {
        float r, g, b;
    };
    struct {
        V2 xy;
        float ignored_;
    };
    float e[3];
};

union V4 {
    struct {
        float x, y, z, w;
    };
    struct {
        float r, g, b, a;
    };
    struct {
        V2 xy;
        float ignored_1, ignored_2;
    };
    struct {
        V3 xyz;
        float ignored_3;
    };
    float e[4];
};

V2 v2(float x, float y)
{
    V2 v = {.x = x, .y = y};
    return v;
}

V2 operator+(V2 i, V2 j)
{
    return v2(i.x + j.x, i.y + j.y);
}

// Now here is where casey does use overloading for vectors and it really is
// a good idea because there are a LOT of functions to write here. Polymorphism
// would be quite nice to have.

int
main(void)
{
    V2 foo = v2(1, 2) + v2(3, 4);
    printf("%f.0\n", foo.x);
}
