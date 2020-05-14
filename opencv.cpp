#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#define GRAD_X 0
#define GRAD_Y 1
#define GRAD_Z 2
#define GRAD_C 3
#define HALF_DIF 0
#define DIF_SPEC 1
#define DIFFUSED 0
#define SPECULAR 1


using namespace cv;
using namespace std;

int image_height, image_width;


int check_open();

Mat image[4][2];

int read_image_png()
{
    String xyzc[4] = {"x", "y", "z", "w"};
    String difspc[2] = {"no_specular/", "specular/"};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            image[i][j] = imread(difspc[j] + xyzc[i] + ".png", IMREAD_UNCHANGED);
        }
    }
    if(!check_open())
    {
        printf("open failed\n");
        return -1;
    }

    image_height = image[0][0].rows;
    image_width = image[0][0].cols;

    return 1;
}

void write_separated_image()
{
    String xyzc[4] = {"x", "y", "z", "c"};
    String difspc[2] = {"dif", "spc"};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            imwrite("sep_" + xyzc[i] + "_" + difspc[j] + ".png", image[i][j]);
        }
    }
}

int check_open()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if(image[i][j].empty())
                return -1;
        }
    }
    return 1;
}

void separate_diffuse_specular()
{
    for (int i = 0; i < 4; i++)
    {
        image_height = image[i][HALF_DIF].rows;
        image_width = image[i][HALF_DIF].cols;
        for (int j = 0; j < image_height; j++)
        {
            uchar *image_pointer[2];
            uchar diffused, specular;

            image_pointer[HALF_DIF] = image[i][HALF_DIF].ptr<uchar>(j);
            image_pointer[DIF_SPEC] = image[i][DIF_SPEC].ptr<uchar>(j);

            for (int k = 0; k < image_width; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    diffused = 2 * image_pointer[HALF_DIF][k * 4 + l];
                    specular = image_pointer[DIF_SPEC][k * 4 + l] - image_pointer[HALF_DIF][k * 4 + l];
                    image_pointer[DIFFUSED][k * 4 + l] = diffused;
                    image_pointer[SPECULAR][k * 4 + l] = specular;
                }
            }
        }
    }
}

void normalize_vector(float *view)
{
    float norm = sqrt(view[0] * view[0] + view[1] * view[1] + view[2] * view[2]);
    //cout << view[0] << " " << view[1] << " " << view[2] << endl;
    view[0] /= norm;
    view[1] /= norm;
    view[2] /= norm;
}

uchar ratio_to_uchar(float v)
{
    if (v > 1.0 or v < -1.0)
    {
        cout << "Unvalid ratio value!" << endl;
        exit(-1);
    }

    return (uchar)(255.0 * (v + 1) / 2.0);
}

void get_normal_from_diffuse()
{
    Mat normal(image_height, image_width, CV_8UC3);

    for (int i = 0; i < image_height; i++)
    {
        uchar *image_pointer[4];
        uchar *normal_image_pointer;

        int channel = 0;

        image_pointer[GRAD_X] = image[GRAD_X][DIFFUSED].ptr<uchar>(i);
        image_pointer[GRAD_Y] = image[GRAD_Y][DIFFUSED].ptr<uchar>(i);
        image_pointer[GRAD_Z] = image[GRAD_Z][DIFFUSED].ptr<uchar>(i);
        image_pointer[GRAD_C] = image[GRAD_C][DIFFUSED].ptr<uchar>(i);

        normal_image_pointer = normal.ptr<uchar>(i);
        
        for (int j = 0; j < image_width; j++)
        {
            float r_ratio[3];
            for (int k = 0; k < 3; k++)
                r_ratio[k] = (float)image_pointer[k][4 * j]/image_pointer[GRAD_C][4 * j + channel] - 0.5;

            normalize_vector(r_ratio);
            //if(j == 1080)
                //printf("%f %f %f\n", r_ratio[0], r_ratio[1], r_ratio[2]);
            normal_image_pointer[3 * j + 0] = ratio_to_uchar(r_ratio[2]);
            normal_image_pointer[3 * j + 1] = ratio_to_uchar(r_ratio[1]);
            normal_image_pointer[3 * j + 2] = ratio_to_uchar(r_ratio[0]);
        }
    }
    cout << "normal from diffuse done" << endl;
    imwrite("normal_from_diffuse.jpg", normal);

    normal.deallocate();

}

void get_albedo_from_diffuse()
{
    Mat albedo(image_height, image_width, CV_8UC3);

    for (int i = 0; i < image_height; i++)
    {
        uchar *image_pointer;
        uchar *albedo_image_pointer;

        image_pointer = image[GRAD_C][DIFFUSED].ptr<uchar>(i);

        albedo_image_pointer = albedo.ptr<uchar>(i);
        
        for (int j = 0; j < image_width; j++)
        {
            float rhoDB = image_pointer[4 * j + 0] * 2.0 / 3.1416;
            float rhoDG = image_pointer[4 * j + 1] * 2.0 / 3.1416;
            float rhoDR = image_pointer[4 * j + 2] * 2.0 / 3.1416;

            albedo_image_pointer[3 * j + 0] = (uchar)rhoDB;
            albedo_image_pointer[3 * j + 1] = (uchar)rhoDG;
            albedo_image_pointer[3 * j + 2] = (uchar)rhoDR;
        }
    }
    cout << "albedo from diffuse done" << endl;
    imwrite("albedo_from_diffuse.jpg", albedo);

    albedo.deallocate();

}
void get_normal_from_specular1(){
    Mat normal(image_height, image_width, CV_8UC3);

    for(int i=0; i<image_height; i++){
        uchar *image_pointer[4];
        uchar *normal_image_pointer;

        image_pointer[GRAD_X] = image[GRAD_X][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_Y] = image[GRAD_Y][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_Z] = image[GRAD_Z][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_C] = image[GRAD_C][SPECULAR].ptr<uchar>(i);

        normal_image_pointer = normal.ptr<uchar>(i);

        for(int j=0; j<image_width; j++){
            float uux, uuy, uuz, Ns;
            uux = (float)image_pointer[GRAD_X][4*j] - 0.5 * image_pointer[GRAD_C][4*j];
            uuy = (float)image_pointer[GRAD_Y][4*j] - 0.5 * image_pointer[GRAD_C][4*j];
            uuz = (float)image_pointer[GRAD_Z][4*j] - 0.5 * image_pointer[GRAD_C][4*j];

            Ns = sqrt(uux*uux + uuy*uuy + uuz*uuz);

            float ux, uy, uz, Nbar;

            ux = uux/Ns;
            uy = uuy/Ns;
            uz = uuz/Ns - 1;

            Nbar = sqrt(ux*ux + uy*uy + uz*uz);

            normal_image_pointer[3*j + GRAD_Z] = ratio_to_uchar(ux / Nbar);
            normal_image_pointer[3*j + GRAD_Y] = ratio_to_uchar(uy / Nbar);
            normal_image_pointer[3*j + GRAD_X] = ratio_to_uchar(uz / Nbar);
        }
    }
    cout << "normal from specular1 done" << endl;
    imwrite("normal_from_specular1.jpg", normal);

    normal.deallocate();
}

void get_normal_from_specular()
{
    Mat normal(image_height, image_width, CV_8UC3);

    for (int i = 0; i < image_height; i++)
    {
        uchar *image_pointer[4];
        uchar *normal_image_pointer;

        image_pointer[GRAD_X] = image[GRAD_X][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_Y] = image[GRAD_Y][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_Z] = image[GRAD_Z][SPECULAR].ptr<uchar>(i);
        image_pointer[GRAD_C] = image[GRAD_C][SPECULAR].ptr<uchar>(i);

        normal_image_pointer = normal.ptr<uchar>(i);
        
        for (int j = 0; j < image_width; j++)
        {
            float r_ratio[3];
            int channel = 0;
            for (int k = 0; k < 3; k++)
                r_ratio[k] = (float)image_pointer[k][4 * j + channel]/image_pointer[GRAD_C][4 * j + channel] - 0.5;
            normalize_vector(r_ratio);
            r_ratio[2] += 1.0;
            normalize_vector(r_ratio);
            //if(j == 1080)
                //printf("%f %f %f\n", r_ratio[0], r_ratio[1], r_ratio[2]);
            normal_image_pointer[3 * j + 0] = ratio_to_uchar(r_ratio[2]);
            normal_image_pointer[3 * j + 1] = ratio_to_uchar(r_ratio[1]);
            normal_image_pointer[3 * j + 2] = ratio_to_uchar(r_ratio[0]);
        }
    }
    cout << "normal from specular done" << endl;
    imwrite("normal_from_specular.jpg", normal);

    normal.deallocate();

}
int main()
{
    read_image_png();

    separate_diffuse_specular();

    //write_separated_image();
    get_normal_from_diffuse();
    get_normal_from_specular();
    //get_normal_from_specular1();
    
    //get_albedo_from_diffuse();

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            image[i][j].release();

    return 0;
}
