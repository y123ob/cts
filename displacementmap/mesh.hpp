#ifndef MESH_HPP
#define MESH_HPP
#include <stdio.h>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

#define MAX_NUMBER 50000
#define V 0
#define VT 1
#define VN 2

typedef struct Face {
    int n;
    int set[4][3];
} Face;

extern int ver_n;
extern int vern_n;
extern int vert_n;
extern int face_n;

extern cv::Point3f ver[MAX_NUMBER];
extern cv::Point3f vern[MAX_NUMBER];
extern cv::Point3f vert[MAX_NUMBER];
extern cv::Point3i verti[MAX_NUMBER];
extern Face face[MAX_NUMBER];

void read_mesh(cv::String filename);
void cvt_to_tri_mesh();
void cvt_to_vert_i(int width, int height);

#endif