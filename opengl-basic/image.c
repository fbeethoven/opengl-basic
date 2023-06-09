#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Image *image_load(const char *image_path) {

    Image *img = (Image *) malloc(sizeof(Image));

    img->data = stbi_load(
        image_path, &img->width, &img->height, &img->channels, 3
    );
    if (img->channels != 3) {
        printf(
            "File %s returned %d channels. Expected 3\n",
            image_path, img->channels
        );
        exit(1);
    }
    if(img->data == NULL) {
        printf("IMAGE: Error loading image %s\n", image_path);
        exit(1);
    }

    return img;
}

void image_free(Image *img) {
    stbi_image_free(img->data);
    free(img);
}

