#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/* general */
static const int k_invalid_index = -1;

/* display info */
static const char default_display_title[] = "Working Title";
static const int desired_fps = 60;
static const int desired_interval = 1000 / desired_fps;
static const int default_xres = 1000;
static const int default_yres = 100;

/* Camera. */
static const Vector3f default_cam_position({0.0, 0.0, 10.0});
static const Vector3f default_cam_lookat({0.0, 0.0, 0.0});
static const Vector3f default_cam_axis({0.0, 0.0, -1.0});
static const Vector3f default_cam_up({0.0, 1.0, 0.0});
static const float default_cam_fov = 90.0;
static const float default_cam_left = -1.0;
static const float default_cam_right = 1.0;
static const float default_cam_bottom = -1.0;
static const float default_cam_top = 1.0;
static const float default_cam_near = 1.0;
static const float default_cam_far = 20.0;
static const int default_cam_x_res = 1000;
static const int default_cam_y_res = 1000;

#endif