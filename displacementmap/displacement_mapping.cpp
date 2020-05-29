#include <stdio.h>
#include <iostream>

#include "mesh.hpp"
#include "texture.hpp"

using namespace std;
using namespace cv;

Pxyz uv_to_xyz(int u, int v)
{
    // TODO
    return ver[u + v];
}

Pxyz uv_to_norm(int u, int v)
{
    // TODO
    return vern[u + v];
}

float norm(Pxyz p) {
    return sqrt(p * p);
}

float NCC(Pxyz p1, Pxyz p2)
{
    return (p1 * p2) / norm(p1) / norm(p2);   
}

float muX(Pxyz X)
{
    // TODO
    return 1.0;
}

int main(int argc, char* argv[])
{
    float delta = 1.0, w_s = 1.0, rho = 1.0, eta = 1.0;

    read_mesh(String(argv[1]));
    Mat image = open_texture(String(argv[2]));
    int width = image.rows, height = image.cols;
    int radius = 1;
    
    Mat dis_map(height, width, CV_32FC3);

    printf("%d\n", vert_n);

    float chi_m, chi_z, chi_p, del_p, w_p, del_s, del_mu, w_mu;

    for (int i = 0; i < height/2; i++)
    {
        Vec3f *source = image.ptr<Vec3f>(i);
        Vec3f *destination = dis_map.ptr<Vec3f>(i);
        for (int j = 0; j < width/2; j++)
        {
            Pxyz X = uv_to_xyz(j, i), n = uv_to_norm(j, i);
            chi_m = (1.0 - NCC(X - n * delta, n))/2.0;
            chi_z = (1.0 - NCC(X, n))/2.0;
            chi_p = (1.0 - NCC(X + n * delta, n))/2.0;

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
                    Pxyz Xi = uv_to_xyz(u, v);
                    wsum += exp(-norm(X - Xi));
                    vsum += exp(-norm(X - Xi)) * (muX(X) - muX(Xi)) * (1 - abs((X - Xi) * n) / norm(X - Xi));
                    //printf("%f %f\n", wsum, vsum);
                }
            }
            del_mu = eta * vsum / wsum;
            w_mu = 3 * rho * chi_z / delta / (chi_m + chi_z + chi_p);

            float dis = (w_p * del_p + w_s * del_s + w_mu * del_mu) / (w_p + w_s + w_mu);
            destination[j][0] = dis;
            destination[j][1] = dis;
            destination[j][2] = dis;
            //printf("%f %f %f %f\n", del_p, del_s, del_mu, dis); 
        }
    }

    show_image(dis_map, 0.16);
    waitKey(0);

    return 0;
}