#include <stdio.h>
#include <iostream>

#include "mesh.hpp"
#include "texture.hpp"
#include "util.cpp"
#include "geometry.hpp"

#define BLU(p) ((p).x)
#define GRN(p) ((p).y)
#define RED(p) ((p).z)

using namespace std;
using namespace cv;

int height, width;

Point3f **geometry;
Point3f **normal;
float **high_pass;

void gen_high_pass(Mat texture, String filename, int kernel_size)
{
    int kernel[50][50];
    high_pass = new float*[height];
    
    for (int i = 0; i < 2 * kernel_size + 1; i++)
        for (int j = 0; j < 2 * kernel_size + 1; j++)
            kernel[i][j] = 2 * kernel_size + 1 - abs(kernel_size - i) - abs(kernel_size - j);

    for (int v = 0; v < height; v++) {
        high_pass[v] = new float[width];
        for (int u = 0; u < width; u++) {
            float sum = 0.0;
            int weight_sum = 0;
//            UMN;
            for (int j = max(0, v - kernel_size), jk = 0; j < min(height, v + kernel_size + 1); j++, jk++) {
                Point3f *pointer = texture.ptr<Point3f>(j);
                for (int i = max(0, u - kernel_size), ik = 0; i < min(width, u + kernel_size + 1); i++, ik++) {
  //          cout << u << " " << v << " " << i << " " << j << endl;
                    sum += BLU(pointer[i]) * kernel[v - j + kernel_size][u - i + kernel_size];
                    weight_sum += kernel[ik][jk];
                
                }
    //            SIK;
            }
            
            high_pass[v][u] = BLU(texture.at<Point3f>(v, u)) - sum / weight_sum;
        }
    }

    write_map<float>(filename, high_pass, height, width);
}

void gen_displacement_map(float delta, float w_s, float rho, float eta, int radius)
{
    Mat dis_map(height, width, CV_32FC3);

    float chi_m, chi_z, chi_p, del_p, w_p, del_s, del_mu, w_mu;

    #pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        Point3f *destination = dis_map.ptr<Point3f>(i);

        #pragma omp parallel for
        for (int j = 0; j < width; j++)
        {
            Point3f x = geometry[i][j], n = normal[i][j];

            chi_m = (1.0 - NCC(x - n * delta, n))/2.0;
            chi_z = (1.0 - NCC(x, n))/2.0;
            chi_p = (1.0 - NCC(x + n * delta, n))/2.0;

            if (chi_m < chi_p && chi_m < chi_z) {
                del_p = -0.5 * delta;
                w_p = (chi_z - chi_m) / delta;
            }
            else if (chi_p < chi_m && chi_p < chi_z) {
                del_p = 0.5 * delta;
                w_p = (chi_z - chi_p) / delta;
            }
            else {
                del_p = 0.5 * (chi_m - chi_p)/(chi_m + chi_p - 2 * chi_z) / delta;
                w_p = 0.5 * (chi_m + chi_p - 2 * chi_z) / delta;
            }

            del_s = 1.0; // TODO
            float wsum = 0.0, vsum = 0.0;
            for (int u = max(0, i - radius); u < min(width, i + radius); u++)
            {
                for (int v = max(0, j - radius); v < min(height, j + radius); v++)
                {
                    if (u + v == i + j) continue;
                    Point3f xi = geometry[v][u];
                    wsum += exp(-norm(x - xi));
                    vsum += exp(-norm(x - xi)) * (high_pass[j][i] - high_pass[v][u]) * (1 - abs(IN(x - xi, n)) / norm(x - xi));
                }
            }
            del_mu = eta * vsum / wsum;
            w_mu = 3 * rho * chi_z / delta / (chi_m + chi_z + chi_p);

            float dis = (w_p * del_p + w_s * del_s + w_mu * del_mu) / (w_p + w_s + w_mu);
            destination[j].x = dis;
            destination[j].y = dis;
            destination[j].z = dis;
        }
    }

    imwrite("dismap.exr", dis_map);
    show_image(dis_map, 0.16);
}

int main(int argc, char* argv[])
{
    
    float delta = 1.0, w_s = 0.0, rho = 1.0, eta = 1.0;
    int radius = 2, n = 0;

    read_mesh(String(argv[1]));

    Mat texture = open_texture(String(argv[2]));
    width = texture.rows, height = texture.cols;
    
    read_map<Point3f>("Output/normal_map.bin", &normal, width, height);
    read_map<Point3f>("Output/geometry_map.bin", &geometry, width, height);
    //read_map<float>("Output/high_pass.bin", &high_pass, width, height);
    gen_high_pass(texture, "Output/high_pass.bin", 10);
    gen_displacement_map(1.0, 0.0, 1.0, 1.0, 2);
    waitKey(0);

    return 0;
}