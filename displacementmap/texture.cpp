#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "texture.hpp"

using namespace cv;
using namespace std;

Mat open_texture(String filename)
{
    return imread(filename, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
}

void show_image(Mat image, float scale)
{
    Mat small_image;
    resize(image,small_image, Size(image.rows * scale,image.cols * scale),0,0,CV_WINDOW_AUTOSIZE);
    imshow("resize",small_image);
    ~small_image;
}