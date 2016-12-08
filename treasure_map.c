/*
  Precedural treasure map generator.
  https://www.reddit.com/r/proceduralgeneration/comments/3vcbb3/monthly_challenge_1_dec_2015_procedural_pirate_map/    |

  * Islands.
  * Treasure Location.
  * Maybe one of those cool arrow things.
  * Burned edge paper.
  
 */

#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
    uint8_t r, g, b, a;
} Color;

Color
color(float r, float g, float b)
{
    Color color = {.r = 255/r,
                   .g = 255/g,
                   .b = 255/b,
                   .a = 255};
    return color;
}

int
main(void) {
    int width = 300;
    int height = 300;
    Color data[width*height];

    fprintf(stderr, "Generating Map\n");
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            data[y*width + x] = color(x/height,y/width,0);
        }
    }
    
    fprintf(stderr, "Writing Map to map.png\n");
    stbi_write_png("map.png", width, height, 4, data, sizeof(Color)*width);

    fprintf(stderr, "Done\n");
    return(0);
}
