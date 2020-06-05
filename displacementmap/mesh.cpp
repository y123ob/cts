#include <stdio.h>
#include <string.h>
#include <fstream>
#include "mesh.hpp"
#include "debug.hpp"
#include "geometry.hpp"

int ver_n;
int vern_n;
int vert_n;
int face_n;

cv::Point3f ver[MAX_NUMBER];
cv::Point3f vern[MAX_NUMBER];
cv::Point3f vert[MAX_NUMBER];
cv::Point3i verti[MAX_NUMBER];

Face face[MAX_NUMBER];

using namespace std;
using namespace cv;


void read_mesh(cv::String filename)
{
    char type[3], line[255];
    int section = 0;
    
    FILE *fp = fopen(filename.c_str() , "r");

    while(section < 4) {
        fscanf(fp, "%s %[^\n]\n", type, line);

        if (!strcmp(type, "v")) {
            sscanf(line, "%f %f %f", &ver[ver_n].x, &ver[ver_n].y, &ver[ver_n].z);
            ver_n++;
        }
        else if (!strcmp(type, "vn")) {
            sscanf(line, "%f %f %f", &vern[vern_n].x, &vern[vern_n].y, &vern[vern_n].z);
            vern_n++;
        }
        else if (!strcmp(type, "vt")) {
            sscanf(line, "%f %f %f", &vert[vert_n].x, &vert[vert_n].y, &vert[vert_n].z);
            vert[vert_n].y = 1.0f - vert[vert_n].y;
            vert_n++;
        }
        else if (!strcmp(type, "f")) {

            int n = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
                face[face_n].set[0], face[face_n].set[0] + 1,face[face_n].set[0] + 2,
                face[face_n].set[1], face[face_n].set[1] + 1,face[face_n].set[1] + 2,
                face[face_n].set[2], face[face_n].set[2] + 1,face[face_n].set[2] + 2,
                face[face_n].set[3], face[face_n].set[3] + 1,face[face_n].set[3] + 2
            );

            for (int i = 0; i < 4; i++)
            {
                if (face[face_n].set[i][0] < 0) face[face_n].set[i][0] = ver_n + face[face_n].set[i][0];
                if (face[face_n].set[i][1] < 0) face[face_n].set[i][1] = vert_n + face[face_n].set[i][1];
                if (face[face_n].set[i][2] < 0) face[face_n].set[i][2] = vern_n + face[face_n].set[i][2];
            }
            face[face_n].n = n == 9? 3 : 4;
            face_n++;
        }
        else if (!strcmp(type, "#")) {
            section++;
        }
    }
}

void cvt_to_tri_mesh()
{
    for(int i = 0; i < ver_n; i++)
        printf("v %f %f %f\n", ver[i].x, ver[i].y, ver[i].z);

    printf("# %d vertices\n", ver_n);

    for(int i = 0; i < vern_n; i++)
        printf("vn %f %f %f\n", vern[i].x, vern[i].y, vern[i].z);

    printf("# %d vertex normals\n", vern_n);

    for(int i = 0; i < vert_n; i++)
        printf("vt %f %f %f\n", vert[i].x, vert[i].y, vert[i].z);

    printf("# %d texture coords\n", vern_n);

    int n = 0;
    for(int i = 0; i < face_n; i++) {
        if(face[i].n == 3) {
            printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                face[i].set[0][0], face[i].set[0][1], face[i].set[0][2],
                face[i].set[1][0], face[i].set[1][1], face[i].set[1][2],
                face[i].set[2][0], face[i].set[2][1], face[i].set[2][2]
            );
            n++;
        }
        else {
            Point3f p[4];
            for(int j = 0; j < 4; j++) {
                p[j].x = vert[face[i].set[j][VT]].x;
                p[j].y = vert[face[i].set[j][VT]].y;
            }
            printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                face[i].set[0][V], face[i].set[0][VT], face[i].set[0][VN],
                face[i].set[1][V], face[i].set[1][VT], face[i].set[1][VN],
                face[i].set[2][V], face[i].set[2][VT], face[i].set[2][VN]
            );

            if(intersects(p[0], p[1], p[2], p[3])) {
                printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                    face[i].set[0][V], face[i].set[0][VT], face[i].set[0][VN],
                    face[i].set[3][V], face[i].set[3][VT], face[i].set[3][VN],
                    face[i].set[1][V], face[i].set[1][VT], face[i].set[1][VN]
                );
            }
            else {
                printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                    face[i].set[2][V], face[i].set[2][VT], face[i].set[2][VN],
                    face[i].set[3][V], face[i].set[3][VT], face[i].set[3][VN],
                    face[i].set[0][V], face[i].set[0][VT], face[i].set[0][VN]
                );
            }
            n++;n++;
        }
    }

    printf("# %d faces\n", n);
    
}

void cvt_to_vert_i(int width, int height)
{
    for (int vt = 0; vt < vert_n; vt++) {
        verti[vt].x = (int)(vert[vt].x * width);
        verti[vt].y = (int)(vert[vt].y * height);
        verti[vt].z = (int)(vert[vt].z * 0);
    }
}