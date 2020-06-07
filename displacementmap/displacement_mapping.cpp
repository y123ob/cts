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

void gen_high_pass(Mat texture, int kernel_size)
{
    GaussianBlur(texture, texture, Size_<int>(2 * kernel_size + 1, 2 * kernel_size + 1), 0.0, 0.0, BORDER_DEFAULT);
    
    high_pass = new float*[height];
    for (int v = 0; v < height; v++) {
        high_pass[v] = new float[width];
        for (int u = 0; u < width; u++) {
            high_pass[v][u] = texture.at<Point3f>(v, u).x; 
        }
    }
}

void gen_displacement_map(String filename, float delta, float w_s, float rho, float eta, int radius)
{
    Mat dis_map(height, width, CV_32FC3);

    dis_map.forEach<Point3f>([&](Point3f& point, const int position[]) -> void {
        float chi_m, chi_z, chi_p, del_p, w_p, del_s, del_mu, w_mu;
        
        int i = position[0], j = position[1];
        Point3f &x = geometry[i][j], &n = normal[i][j];

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
            for (int v = max(0, j - radius); v < min(height, j + radius); v++) {
                Point3f &xi = geometry[v][u];
                wsum += exp(-norm(x - xi));
                vsum += exp(-norm(x - xi)) * (high_pass[j][i] - high_pass[v][u]) * (1 - abs(n.dot(x - xi)) / norm(x - xi));
            }

        del_mu = eta * vsum / wsum;
        w_mu = 3 * rho * chi_z / delta / (chi_m + chi_z + chi_p);

        float dis = (w_p * del_p + w_s * del_s + w_mu * del_mu) / (w_p + w_s + w_mu);
        point.x = dis;
        point.y = dis;
        point.z = dis;
    });

    imwrite(filename, dis_map);
    show_image(dis_map, 0.16);
    ~dis_map;
}

int main(int argc, char* argv[])
{
    float delta = 1.0, w_s = 0.0, rho = 1.0, eta = 1.0;
    int radius = 2;

    char key[50], value[200], output_loc[200];

    FILE *config = fopen("config.conf", "r");

    while(1) {
        fscanf(config, "%s %s", key, value);
        if(strcmp(key, "normal_map_loc") == 0) {
            printf("Reading normal map from \"%s\"...\n", value);
            read_map<Point3f>(String(value), &normal, width, height);
        }
        else if(strcmp(key, "geometry_map_loc") == 0) {
            printf("Reading geometry map from \"%s\"...\n", value);
            read_map<Point3f>(String(value), &geometry, width, height);
        }
        else if(strcmp(key, "mesh_loc") == 0) {
            printf("Reading mesh from \"%s\"...\n", value);
            read_mesh(value);
        }
        else if(strcmp(key, "texture_loc") == 0) {
            printf("Reading texture from \"%s\"...\n", value);
            Mat texture = open_texture(value);
            width = texture.rows, height = texture.cols;
        
            printf("Generating high pass...\n");
            gen_high_pass(texture, 5);
            ~texture;
        }
        else if(strcmp(key, "dismap_loc") == 0) {
            strcpy(output_loc, value);
        }
        else if(strcmp(key, "delta") == 0) {
            sscanf(value, "%f", &delta);
        }
        else if(strcmp(key, "w_s") == 0) {
            sscanf(value, "%f", &w_s);
        }
        else if(strcmp(key, "rho") == 0) {
            sscanf(value, "%f", &rho);
        }
        else if(strcmp(key, "eta") == 0) {
            sscanf(value, "%f", &eta);
        }
        else if(strcmp(key, "end") == 0) {
            break;
        }
        else {
            printf("Unknown config key word: \"%s\"\n", key);
            printf("Terminate the process\n");
            exit(1);
        }
    }

    fclose(config);

    printf("Hyper parameter configuration:\n\t[delta = %f, w_s = %f, rho = %f, eta = %f]\n", delta, w_s, rho, eta);
    printf("Generating displacement map...");
    fflush(stdout);
    gen_displacement_map(output_loc, delta, w_s, rho, eta, radius);
    printf("Done\n");
    printf("Press any key to close...\n");
    waitKey(0);

    return 0;
}