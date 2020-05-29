#ifndef MESH_HPP
#define MESH_HPP
#include <stdio.h>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

#define MAX_NUMBER 25000


typedef struct Pxyz {
    float coor[3];

    struct Pxyz operator *(float f) {
        return { coor[0] * f, coor[1] * f, coor[2] * f };
    }

    float operator *(struct Pxyz Y) {
        return coor[0] * Y.coor[0] + coor[1] * Y.coor[1] + coor[2] * Y.coor[2];
    }

    struct Pxyz operator +(struct Pxyz Y) {
        return { coor[0] + Y.coor[0], coor[1] + Y.coor[1], coor[2] + Y.coor[2] };
    }

    struct Pxyz operator -(struct Pxyz Y) {
        return { coor[0] - Y.coor[0], coor[1] - Y.coor[1], coor[2] - Y.coor[2] };
    }

} Pxyz;

typedef struct Face {
    int n;
    int vec[4][3];
} Face;

extern int ver_n;
extern int vern_n;
extern int vert_n;
extern int face_n;

extern Pxyz ver[MAX_NUMBER];
extern Pxyz vern[MAX_NUMBER];
extern Pxyz vert[MAX_NUMBER];
extern Face face[MAX_NUMBER];

void read_mesh(cv::String filename);

#endif