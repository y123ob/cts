#ifndef UTIL_C
#define UTIL_C

#include <stdio.h>
#include <opencv2/opencv.hpp>

template <typename T>
int read_map(cv::String filename, T ***array, int height, int width)
{
    FILE *fp = fopen(filename.c_str(), "r");
    *array = new T*[height];

    if (fp == NULL) {
        printf("\"%s\": no such file.\n", filename.c_str());
        exit(1);
    }

    for (int i = 0; i < height; i++) {
        (*array)[i] = new T[width];
        fread((*array)[i], sizeof(T), width, fp);
    }

    fclose(fp);
    
    return 1;
}

template <typename T>
int write_map(cv::String filename, T **array, int height, int width)
{
    FILE *fp = fopen(filename.c_str(), "w");

    if (fp == NULL)
        return 0;

    for (int i = 0; i < height; i++) {
        fwrite(array[i], sizeof(T), width, fp);
    }

    fclose(fp);

    return 1;
}
#endif