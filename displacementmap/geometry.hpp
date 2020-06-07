#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <opencv2/opencv.hpp>

using namespace cv;


float norm(Point3f p);
float NCC(Point3f p1, Point3f p2);

int sign(Point3i p1, Point3i p2, Point3i p3);
bool point_in_triangle (Point3i *points);


bool onSegment(Point3f p, Point3f q, Point3f r);
int orientation(Point3f p, Point3f q, Point3f r);
bool intersects(Point3f p1, Point3f q1, Point3f p2, Point3f q2);

Point3f decompose_point(Point3f p1, Point3f p2, Point3f p3, Point3f pt);

#endif