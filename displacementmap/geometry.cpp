#include "geometry.hpp"

using namespace cv;

float norm(Point3f p)
{
    return sqrt(IN(p, p));
}

float NCC(Point3f p1, Point3f p2)
{
    return IN(p1, p2) / norm(p1) / norm(p2);   
}

int sign (Point3i p1, Point3i p2, Point3i p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool point_in_triangle (Point3i *points)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(points[3], points[0], points[1]);
    d2 = sign(points[3], points[1], points[2]);
    d3 = sign(points[3], points[2], points[0]);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}


 
// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool on_segment(Point3f p, Point3f q, Point3f r) 
{ 
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && 
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) 
       return true; 
  
    return false; 
} 
  
// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(Point3f p, Point3f q, Point3f r) 
{ 
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
    // for details of below formula. 
    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
  
    if (abs(val) <=  1e-10) return 0;  // colinear 
  
    return (val > 0)? 1: 2; // clock or counterclock wise 
} 
  
// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool intersects(Point3f p1, Point3f q1, Point3f p2, Point3f q2) 
{ 
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 

    return o1 != o2 && o3 != o4;
    // General case 
    if (o1 != o2 && o3 != o4) 
        return true; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && on_segment(p1, p2, q1)) return true; 
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && on_segment(p1, q2, q1)) return true; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && on_segment(p2, p1, q2)) return true; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && on_segment(p2, q1, q2)) return true; 
  
    return false; // Doesn't fall in any of the above cases 
}

Point3f decompose_point(Point3f p1, Point3f p2, Point3f p3, Point3f pt)
{
    float x = pt.x - p3.x, y = pt.y - p3.y;
    float a = p1.x - p3.x, b = p2.x - p3.x;
    float c = p1.y - p3.y, d = p2.y - p3.y;

    float det = (float)(a * d - b * c);

    float p = (float)(x * d - b * y) / det, q = (float)(-x * c + a * y) / det;
    return {p, q, 1.0f - p - q};
}
