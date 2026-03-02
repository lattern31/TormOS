#include "3d.h"
#include "vga.h"
#include "../math.h"

double global_angle = 0.1;
double test_angle = 0.1;
double global_dz = 0;
Point_3d_t cam = (Point_3d_t){0, 0, 0};

Point_t project(Point_3d_t point) {
    if (point.z < 0) {
        return (Point_t){-99, -99};
    }
    return (Point_t){point.x/point.z, point.y/point.z};
}

PointScreen_t screen(Point_t p){ // points -1.6, -1 to 1.6, 1
    double aspect_ratio = 0.625;
    return (PointScreen_t){
        (p.x*aspect_ratio+1)/2*VGA_WIDTH, (1-(p.y+1)/2)*VGA_HEIGHT,
    };
}

Point_3d_t rotate_xz(Point_3d_t point, double angle) {
    double sin = fast_sin(angle);
    double cos = fast_cos(angle);
    Point_3d_t rotated_p = (Point_3d_t){
        .x = point.x*cos-point.z*sin,
        .y = point.y,
        .z = point.x*sin+point.z*cos,
    };
    return rotated_p;
}

Point_3d_t move_cam_by_yaw(Point_3d_t cam_point, double yaw, double speed) {
    // cos(pitch) is 1 by now
    double sin = fast_sin(yaw);
    double cos = fast_cos(yaw);
    double forward_x = sin;
    double forward_y = 0;
    double forward_z = cos;
    cam_point.x += forward_x * speed;
    //cam_point.y += forward_y * speed;
    cam_point.z += forward_z * speed;
    return cam_point;
}

void draw_line_3d(Point_3d_t p1, Point_3d_t p2, u8 color) {
    const float NEAR = 0.01f;
    
    if (p1.z < NEAR && p2.z < NEAR) return;
    
    if (p1.z < NEAR) {
        float t = (NEAR - p2.z) / (p1.z - p2.z);
        p1.x = p2.x + t * (p1.x - p2.x);
        p1.y = p2.y + t * (p1.y - p2.y);
        p1.z = NEAR;
    } else if (p2.z < NEAR) {
        float t = (NEAR - p1.z) / (p2.z - p1.z);
        p2.x = p1.x + t * (p2.x - p1.x);
        p2.y = p1.y + t * (p2.y - p1.y);
        p2.z = NEAR;
    }
    
    PointScreen_t s1 = screen(project(p1));
    PointScreen_t s2 = screen(project(p2));

    draw_line(s1, s2, 3);
}

void draw_figure(Figure_t *figure) {
    Point_3d_t current_points_3d[figure->points_len];

    for (int i = 0; i < figure->points_len; i++){
        Point_3d_t rotated_by_local_p = rotate_xz(
            figure->points[i], figure->local_angle
        );
        rotated_by_local_p = rotate_xz(
            rotated_by_local_p, test_angle
        );
        //Point_3d_t rotated_by_local_p = figure->points[i];
        rotated_by_local_p.x +=  figure->coordinates.x;
        rotated_by_local_p.y +=  figure->coordinates.y;
        rotated_by_local_p.z +=  figure->coordinates.z;
        // cam
        Point_3d_t relative_at_cam = (Point_3d_t){
            rotated_by_local_p.x - cam.x,
            rotated_by_local_p.y, //rotated_by_local_p.y - cam.y,
            rotated_by_local_p.z - cam.z,
        };
        
        Point_3d_t rotated_by_global_p = rotate_xz(
            relative_at_cam, global_angle
        );

        current_points_3d[i] = rotated_by_global_p;
    }

    for (int i = 0; i < figure->edges_len; i++) {
        Edge_t edge = figure->edges[i];
        draw_line_3d(
            current_points_3d[edge.idx_p1],
            current_points_3d[edge.idx_p2],
            3  // color
        );
    }
}

Figure_t create_tetrahedron(double size, Point_3d_t coordinates, double local_angle) {
    size_t points_len = 4;
    size_t edges_len = 6;
    Figure_t tetrahedron = (Figure_t){
        .points_len=points_len, .edges_len=edges_len,
        .coordinates=coordinates, .local_angle=local_angle
    };
    Point_3d_t points[] = { 
        (Point_3d_t){size, size, size},
        (Point_3d_t){size, -size, },
        (Point_3d_t){-size, -size, size},
        (Point_3d_t){0, 0, -size},
    };
    for (int i = 0; i < points_len; i++) {
        tetrahedron.points[i] = points[i];
    };
    Edge_t edges[] = {
        (Edge_t){0, 1},
        (Edge_t){0, 2},
        (Edge_t){0, 3},
        (Edge_t){1, 2},
        (Edge_t){1, 3},
        (Edge_t){2, 3},
    };
    for (int i = 0; i < edges_len; i++) {
        tetrahedron.edges[i] = edges[i];
    };
    return tetrahedron;
}

Figure_t create_cube(double size, Point_3d_t coordinates, double local_angle) {
    size_t points_len = 8;
    size_t edges_len = 12;
    Figure_t cube = (Figure_t){
        .points_len=points_len, .edges_len=edges_len,
        .coordinates=coordinates, .local_angle=local_angle
    };

    Point_3d_t points[] = { 
        (Point_3d_t){size, size, size},
        (Point_3d_t){size, size, -size},
        (Point_3d_t){size, -size, size},
        (Point_3d_t){size, -size, -size},
        (Point_3d_t){-size, size, size},
        (Point_3d_t){-size, size, -size},
        (Point_3d_t){-size, -size, size},
        (Point_3d_t){-size, -size, -size},
    };
    for (int i = 0; i < points_len; i++) {
        cube.points[i] = points[i];
    }
    Edge_t edges[] = {
        (Edge_t){0, 1}, (Edge_t){2, 3}, (Edge_t){0, 2}, (Edge_t){1, 3},
        (Edge_t){4, 5}, (Edge_t){6, 7}, (Edge_t){4, 6}, (Edge_t){5, 7},
        (Edge_t){0, 4}, (Edge_t){1, 5}, (Edge_t){2, 6}, (Edge_t){3, 7},
    };
    for (int i = 0; i < edges_len; i++) {
        cube.edges[i] = edges[i];
    }
    return cube;
}

