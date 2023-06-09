#ifndef ANIMATION_H
#define ANIMATION_H


#include "common.h"
#include "memory.h"
#include "graphics.h"


typedef struct Transform {
    Vec3 translation;
    Vec4 rotation;
    Vec3 scale;
} Transform;


typedef struct KeyFrame {
    ArrayList *time_stamp;
    ArrayList *value;
} KeyFrame;


typedef struct Animation {
    KeyFrame joint_translation;
    KeyFrame joint_rotation;
    KeyFrame joint_scale;
} Animation;

typedef struct AnimationController {
    float current_time;
    ArrayList *joints;
} AnimationController;

typedef struct Joint Joint;

typedef struct Joint {
    Joint *children;
    Joint *next;

    Mat4 local_transform;
    Mat4 inverse_bind_matrix;
    Animation animation;
} Joint;

typedef struct AnimatedModel {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
    unsigned int uv;
    unsigned int color;
    unsigned int normal;

    int vertex_count;

    unsigned int texture_id;

	unsigned int jbo;
	unsigned int wbo;

    ArrayList *joints;
    ArrayList *weights;


} AnimatedModel;


ArrayList *get_anim_bones();
void animation_update(AnimationController *anim_control, float dt);
AnimatedModel *get_animated_model();
void load_skeleton(AnimatedModel *anim_model);
void render_animation_entities(
    Renderer *rh, AnimationController *animation_controller
);


#endif  // ANIMATION_H
