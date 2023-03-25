#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Image *image_load(const char *image_path) {

    Image *img = (Image *) malloc(sizeof(Image));

    img->data = stbi_load(image_path, &img->width, &img->height, &img->channels, 0);
    if(img->data == NULL) {
        printf("IMAGE: Error in loading the image\n");
        exit(1);
    }

    return img;
}

void image_free(Image *img) {
    stbi_image_free(img->data);
    free(img);
}

static int test_main() {
    char *image_path = "texture920x613.png";
    Image *test = image_load(image_path);
    printf("Success? \n");
    printf("width: %d height: %d channels: %d \n", test->width, test->height, test->channels);
    return 0;
}