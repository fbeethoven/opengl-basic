#ifndef CONFIG_H 
#define CONFIG_H


#define BOX_COLLITION 1  // if 0 then SPHERE_COLLITION 


// Renderer
#define RENDERER_FOV 1.19
#define RENDERER_NEAR_PLANE 0.1
#define RENDERER_FAR_PLANE 1000

#define RENDERER_RED 0.4
#define RENDERER_GREEN 0.4
#define RENDERER_BLUE 0.4


// Camera

// Parameters for Random Entities Game/Experiment
#if 0
 #define CAMERA_POSITION   0.0, 60.0, 0.0
 #define CAMERA_CENTRE     0.0, 55.0, 0.0
 #define CAMERA_PITCH      2.2
 #define CAMERA_YAW        1.57
#endif

#if 1
#define CAMERA_POSITION 0.0, 1.0, 5.0
#define CAMERA_CENTRE   0.0, 1.0, 0.0
#define CAMERA_PITCH    1.57
#define CAMERA_YAW      -1.57
#endif


// Light
#define LIGHT_DIR 0.0, -1.0, 0.0
#define LIGHT_COLOR 0.8, 0.8, 1.0


#endif  // CONFIG_H
