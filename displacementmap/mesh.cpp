#include <stdio.h>
#include <string.h>
#include <fstream>
#include "mesh.hpp"

int ver_n;
int vern_n;
int vert_n;
int face_n;

Pxyz ver[MAX_NUMBER];
Pxyz vern[MAX_NUMBER];
Pxyz vert[MAX_NUMBER];
Face face[MAX_NUMBER];

void read_mesh(cv::String filename)
{
    char type[3], line[255];
    int section = 0;
    
    FILE *fp = fopen(filename.c_str() , "r");

    while(section < 4) {
        fscanf(fp, "%s %[^\n]\n", type, line);

        if (!strcmp(type, "v")) {
            sscanf(line, "%f %f %f", ver[ver_n].coor, ver[ver_n].coor + 1, ver[ver_n].coor + 2);
            ver_n++;
        }
        else if (!strcmp(type, "vn")) {
            sscanf(line, "%f %f %f", vern[vern_n].coor, vern[vern_n].coor + 1, vern[vern_n].coor + 2);
            vern_n++;
        }
        else if (!strcmp(type, "vt")) {
            sscanf(line, "%f %f %f", vert[vert_n].coor, vert[vert_n].coor + 1, vert[vert_n].coor + 2);
            vert_n++;
        }
        else if (!strcmp(type, "f")) {

            int n = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                face[face_n].vec[0], face[face_n].vec[0] + 1,face[face_n].vec[0] + 2,
                face[face_n].vec[1], face[face_n].vec[1] + 1,face[face_n].vec[1] + 2,
                face[face_n].vec[2], face[face_n].vec[2] + 1,face[face_n].vec[2] + 2,
                face[face_n].vec[3], face[face_n].vec[3] + 1,face[face_n].vec[3] + 2
            );

            for (int i = 0; i < 4; i++)
            {
                if (face[face_n].vec[i][0] < 0) face[face_n].vec[i][0] = ver_n + face[face_n].vec[i][0];
                if (face[face_n].vec[i][1] < 0) face[face_n].vec[i][1] = vern_n + face[face_n].vec[i][1];
                if (face[face_n].vec[i][2] < 0) face[face_n].vec[i][2] = vert_n + face[face_n].vec[i][2];
            }
            face[face_n].n = n == 9? 3 : 4;
            face_n++;
        }
        else if (!strcmp(type, "#")) {
            section++;
        }
    }
    printf("%d %d %d %d\n", ver_n, vern_n, vert_n, face_n);
}