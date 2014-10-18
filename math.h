#ifndef math_h
#define math_h

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

double degrees_to_radians(double degrees);
double radians_to_degrees(double radians);

//Reduce the passed angle so that it is within [0.0,360.0]
void reduce_angle(double& angle);
//Returns the quadrant that the passed angle is in, or 0 if it is a quadrantal angle.
int get_angle_quadrant(double angle);

bool signs_same(int a,int b);

double distance_between_points(double x1,double y1,double x2,double y2);

#endif
