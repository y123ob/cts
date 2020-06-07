#include <stdio.h>
#include <iostream>
#include "mesh.hpp"
#include "texture.hpp"
#include "util.cpp"
#include "geometry.hpp"

using namespace std;
using namespace cv;

int **face_map;
Point3f **normal_map, **geometry_map;

int width, height;

void gen_face_map(String filename)
{
    face_map = new int*[height];

    #pragma omp paraller for
    for (int v = 0; v < height; v++) {
        face_map[v] = new int[width];

        for (int u = 0; u < width; u++)
            face_map[v][u] = -1;
    }

    #pragma omp parallel for
    for (int f = 0; f < face_n; f++) {
        Point3i points[4];

        for (int n = 0; n < 3; n++)
            points[n] = verti[face[f].set[n][VT]];

        int up =    max(max(points[0].y, points[1].y), points[2].y);
        int down =  min(min(points[0].y, points[1].y), points[2].y);
        int right = max(max(points[0].x, points[1].x), points[2].x);
        int left =  min(min(points[0].x, points[1].x), points[2].x);

        #pragma omp parallel for
        for (int u = left; u < right; u++) {
            for (int v = down; v < up; v++) {
                points[3] = {u, v, 0};

                if(point_in_triangle(points))
                    face_map[v][u] = f;
            }
        }
    }

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if(face_map[i][j] == -1) {
                face_map[i][j] = max(
                                 max(face_map[i - 1][j - 1], face_map[i - 1][j + 1]), 
                                 max(face_map[i + 1][j - 1], face_map[i + 1][j + 1]));
            }
        }
    }

    write_map<int>(filename, face_map, width, height);
}

void gen_normal_map(String filename, int mode)
{
    Point3f **normal_map = new Point3f*[height];
    Point3f scale_vec = {1.0, 1.0, 1.0};
    for (int i = 0; i < height; i++) {
        normal_map[i] = new Point3f[width];
        for (int j = 0; j < width; j++) {
            if(face_map[i][j] == -1) continue;
            Face f = face[face_map[i][j]];

            Point3f loc = {(float)j / (float)width, (float)i / (float)height, 0.0f};
            Point3f comp = decompose_point(vert[f.set[0][VT]], vert[f.set[1][VT]], vert[f.set[2][VT]], loc);
            Point3f normal = comp.x * vern[f.set[0][VN]] + comp.y * vern[f.set[1][VN]] + comp.z * vern[f.set[2][VN]];
            
            float size = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;

            normal /= sqrt(size);

            if(mode) normal = (normal + scale_vec) / 2.0;
            normal_map[i][j].x = normal.z;
            normal_map[i][j].y = normal.y;
            normal_map[i][j].z = normal.x;
        }
    }

    write_map<Point3f>(filename, normal_map, height, width);
}

void gen_geometry_map(String filename)
{
    Point3f **geometry_map = new Point3f*[height];
    
    for (int i = 0; i < height; i++)
    {
        geometry_map[i] = new Point3f[width];
        for (int j = 0; j < width; j++)
        {
            if(face_map[i][j] == -1) continue;

            Face f = face[face_map[i][j]];

            Point3f loc = {(float)j / (float)width, (float)i / (float)height, 0.0f};
            Point3f comp = decompose_point(vert[f.set[0][VT]], vert[f.set[1][VT]], vert[f.set[2][VT]], loc);
            Point3f pij = comp.x * ver[f.set[0][V]] + comp.y * ver[f.set[1][V]] + comp.z * ver[f.set[2][V]];

            geometry_map[i][j].x = pij.z;
            geometry_map[i][j].y = pij.y;
            geometry_map[i][j].z = pij.x;
        }
    }

    write_map<Point3f>(filename, geometry_map, height, width);
}

int main(int argc, char* argv[])
{
    Mat image = open_texture("Color_raw/00_diffuse_unlit_raw.exr");
    width = image.cols, height = image.rows;
    ~image;

    read_mesh(String(argv[1]));
    cvt_to_vert_i(width, height);

    int mode = (argc >= 3 && strcmp(argv[2], "view") == 0);

    gen_face_map("Output/face_map.bin");
    gen_normal_map("Output/normal_map.bin", mode);
    gen_geometry_map("Output/geometry_map.bin");

    return 0;
}