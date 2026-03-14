#ifndef VGA_3D_H
#define VGA_3D_H

#include "vga.h"

#define MAX_POINTS 16 
#define MAX_EDGES 32

typedef struct Point {
    double x;
    double y;
} Point_t;

typedef struct Point_3d {
    double x;
    double y;
    double z;
} Point_3d_t;

typedef struct Edge {
    int idx_p1;
    int idx_p2;
} Edge_t;

typedef struct Figure {
    Point_3d_t points[MAX_POINTS];
    size_t points_len;
    Edge_t edges[MAX_EDGES];
    size_t edges_len;
    Point_3d_t coordinates;
    double local_angle;
} Figure_t;

struct StateScene{
    Figure_t figures[4]; 
};

extern double global_angle;
extern double test_angle;
extern double global_dz;
extern Point_3d_t cam;

void draw_line_3d(Point_3d_t p1, Point_3d_t p2, u8 color);
Point_t project(Point_3d_t point);
PointScreen_t screen(Point_t point);
Point_3d_t rotate_xz(Point_3d_t point, double angle);
Point_3d_t move_cam_by_yaw(Point_3d_t cam_point, double yaw, double speed);
void draw_figure(Figure_t *figure);
Figure_t create_tetrahedron(double size, Point_3d_t coordinates, double local_angle);
Figure_t create_cube(double size, Point_3d_t coordinates, double local_angle);

#endif
