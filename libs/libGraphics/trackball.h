#ifndef __trackball_h_
#define __trackball_h__

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y);

void build_rotmatrix(float m[4][4], float q[4]);

void build_rotmatrix(double m[4][4], float q[4]);

void add_quats(float q1[4], float q2[4], float dest[4]);

#endif
