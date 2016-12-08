/*  steve.h, collection of small useful utils for c programming by steve.
    Modeled after stb.h by https://github.com/nothings/stb which I also use.
    v0.01
    public domain, no warranty is offered or implied.

    Most of this is stuff for games, lots of it synergises with gameguy which
    will also be open source at some point.

    malloc, free, realloc are all used. look into a wrapper like stb has for
    when you want custom allocation.

    use stb_arr instead of stretchy_buffer, it's the same thing I think.

    - realloc under the hood, store before the array.
    - casey doesn't like allocation, sean doesn't care?
    - can relocate so watch out!
*/

#ifndef STEVE
#define STEVE

#endif


// what is used most in stb by sean?
// stb_lerp, stb_arr_len, stb_remap
// stb_rand and stb_frant,
// stb_min and stb_max
// stb_clamp
// stb_sdict   sting->pointer hashmap    <- use this steve!
// also use the pointer->pointer hashmap!

// look into both stb_arr and stb_sdict and decide if you want either of them...

// read the knuth books! they are not fun but good to read.

// imgui libraries are good

// stb_image is used for like everything.


/* pointer->dict is pointer to pointer */
/* stb_arr_ is array and good */

// casey and jeff like purity, sean likes a 10% hit to streamline the programming
// experience.
