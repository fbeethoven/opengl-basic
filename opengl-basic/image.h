#ifndef IMAGE_H
#define IMAGE_H


typedef struct Image {
    int width;
    int height;
    int channels;
    unsigned char *data;
} Image;

Image *image_load(const char *image_path);

void image_free(Image * img);


#endif  // IMAGE_H
