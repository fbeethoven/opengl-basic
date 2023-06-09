#include "animation.h"
#include "utils/file_handler.h"
#include "graphics.h"


Mat4 get_matrix(Tokenizer *tokenizer) {
    Mat4 C = {0};
    float result[16] = {0};
    for (
        Token token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "inverse_bind_matrix") == 0
        ) {
            token = token_next(tokenizer);
            token_expected(tokenizer, token, Token_Colon);
            token = token_next(tokenizer);
            token_expected(tokenizer, token, Token_OpenBra);

            for (int i=0; i<16; i++) {
                token = token_next(tokenizer);
                token_expected(tokenizer, token, Token_Float);

                result[i] = atof(token.info);                

                token = token_next(tokenizer);
            }
            break;
        }
    }

    memcpy(&C, result, sizeof(float)*16);
    return C;
} 


void joint_push_child(Joint *parent, Joint *child) {
    printf("Adding children in function\n");
    child->next = parent->children;
    parent->children = child;
}


float tokenizer_float(Tokenizer *tokenizer) {
    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Float);
    return atof(token.info);
}

Vec3 tokenizer_vec3(Tokenizer *tokenizer) {
    Vec3 result = {0};

    result.x = tokenizer_float(tokenizer);
    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    result.y = tokenizer_float(tokenizer);
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    result.z = tokenizer_float(tokenizer);
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_CloseBra);

    return result;
}

Vec4 tokenizer_vec4(Tokenizer *tokenizer) {
    Vec4 result = {0};

    result.x = tokenizer_float(tokenizer);
    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    result.y = tokenizer_float(tokenizer);
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    result.z = tokenizer_float(tokenizer);
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    result.w = tokenizer_float(tokenizer);
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_CloseBra);

    return result;
}

KeyFrame joint_animation_translation(Tokenizer *tokenizer) {
    KeyFrame joint_translation = {0};

    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_translation.time_stamp = new_array_list(float);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }

        if (token.kind == Token_Float) {
            *arr_push(joint_translation.time_stamp, float) = atof(token.info);
        }
    }

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_translation.value = new_array_list(Vec3);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }
        if (token.kind == Token_OpenBra) {
            *arr_push(joint_translation.value, Vec3) = (
                tokenizer_vec3(tokenizer)
            );
        }
    }
    return joint_translation;
}

KeyFrame joint_animation_rotation(Tokenizer *tokenizer) {
    KeyFrame joint_rotation = {0};

    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_rotation.time_stamp = new_array_list(float);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }

        if (token.kind == Token_Float) {
            *arr_push(joint_rotation.time_stamp, float) = atof(token.info);
        }
    }

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_rotation.value = new_array_list(Vec4);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }
        if (token.kind == Token_OpenBra) {
            *arr_push(joint_rotation.value, Vec4) = (
                tokenizer_vec4(tokenizer)
            );
        }
    }
    return joint_rotation;
}

KeyFrame joint_animation_scale(Tokenizer *tokenizer) {
    KeyFrame joint_scale = {0};

    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_scale.time_stamp = new_array_list(float);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }

        if (token.kind == Token_Float) {
            *arr_push(joint_scale.time_stamp, float) = atof(token.info);
        }
    }

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Coma);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);
    joint_scale.value = new_array_list(Vec3);
    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseBra) {
            break;
        }
        if (token.kind == Token_OpenBra) {
            *arr_push(joint_scale.value, Vec3) = (
                tokenizer_vec3(tokenizer)
            );
        }
    }
    return joint_scale;
}

void joint_animation(Tokenizer *tokenizer, ArrayList *joints) {
    Joint *joint = &arr_get(joints, Joint, joints->counter -1);
    Animation *anim = &joint->animation;
    
    Token token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenCurl);

    token = token_next(tokenizer);
    if (token.kind == Token_CloseCurl) {
        return;
    }

    for (;
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        if (token.kind == Token_CloseCurl) {
            return;
        }

        if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "translation") == 0
        ) {
            anim->joint_translation = joint_animation_translation(tokenizer);
            token = token_next(tokenizer);
            token_expected(tokenizer, token, Token_CloseBra);
        }
        if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "rotation") == 0
        ) {
            anim->joint_rotation = joint_animation_rotation(tokenizer);
            token = token_next(tokenizer);
            token_expected(tokenizer, token, Token_CloseBra);
        }
        if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "scale") == 0
        ) {
            anim->joint_scale = joint_animation_scale(tokenizer);
            token = token_next(tokenizer);
            token_expected(tokenizer, token, Token_CloseBra);
        }
    }
}

void joint_add_children(
    Tokenizer *tokenizer, ArrayList *joints, int i
) {
    Joint *parent = &arr_get(joints, Joint, i);
    for (
        Token token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {

        if (token.kind == Token_CloseBra) {
            break;
        }

        if (token.kind == Token_Int) {
            int j = atoi(token.info);
            Joint *child = &arr_get(joints, Joint, j);
            joint_push_child(parent, child);
        }
    }

}


ArrayList *get_anim_bones() {
    char *data = read_file("assets/models/trooper.anim");
    ArrayList *joints = new_array_list(Joint);

    Tokenizer tokenizer = {0};
    tokenizer.data = data;


    for (
        Token token = token_next(&tokenizer);
        tokenizer.data[tokenizer.cursor] && token_is_valid(&token);
        token = token_next(&tokenizer)
    ) {
        if (token.kind == Token_OpenCurl) {
            Joint *new_joint = arr_push(joints, Joint);
            new_joint->inverse_bind_matrix = get_matrix(&tokenizer);
        }
        else if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "animation") == 0
        ) {
            joint_animation(&tokenizer, joints);
        }
    }

    
    tokenizer_reset(&tokenizer);
    int i = 0;

    for (
        Token token = token_next(&tokenizer);
        tokenizer.data[tokenizer.cursor] && token_is_valid(&token);
        token = token_next(&tokenizer)
    ) {

        if (
            token.kind == Token_StrLiteral &&
            strcmp(token.info, "children") == 0
        ) {
            joint_add_children(&tokenizer, joints, i++);
        }
    }

    free(data);
    return joints;
}

Vec3 animation_get_translation(Animation anim, float dt) {
    KeyFrame translation = anim.joint_translation;
    if (!translation.time_stamp) {
        return newVec3(0.0, 0.0, 0.0);
    }
    float *time_stamp = ((float *)translation.time_stamp->data);

    float prev_time = time_stamp[0];
    float next_time = time_stamp[1];
    int i;
    for (i=0; next_time<dt; i++) {
        if ( (i+1) >= translation.time_stamp->counter) {
            return arr_get(translation.value, Vec3, 0);
        }
        prev_time = next_time;
        next_time = time_stamp[i + 1];
    }

    Vec3 before = arr_get(translation.value, Vec3, i);
    Vec3 after = arr_get(translation.value, Vec3, i+1);

    if (prev_time > next_time) {
        printf("Issue in translation\n");
        printf("Previous time %f | Next time %f\n", prev_time, next_time);
        exit(1);
    }

    float time_step = (dt - prev_time) / (next_time - prev_time);

    return vec3_lerp(before, after, time_step);
}

Vec4 animation_get_rotation(Animation anim, float dt) {
    KeyFrame rotation = anim.joint_rotation;
    if (!rotation.time_stamp) {
        return newVec4(0.0, 0.0, 0.0, 1.0);
    }
    float *time_stamp = ((float *)rotation.time_stamp->data);

    float prev_time = time_stamp[0];
    float next_time = time_stamp[1];
    int i;
    for (i=0; next_time<dt; i++) {
        if ( (i+1) >= rotation.time_stamp->counter) {
            return arr_get(rotation.value, Vec4, 0);
        }
        prev_time = next_time;
        next_time = time_stamp[i + 1];
    }

    Vec4 before = arr_get(rotation.value, Vec4, i);
    Vec4 after = arr_get(rotation.value, Vec4, i+1);

    if (prev_time > next_time) {
        printf("Issue in rotation animation\n");
        printf("Previous time %f | Next time %f\n", prev_time, next_time);
        exit(1);
    }

    float time_step = (dt - prev_time) / (next_time - prev_time);

    return vec4_lerp(before, after, time_step);
}

Vec3 animation_get_scale(Animation anim, float dt) {
    KeyFrame scale = anim.joint_scale;
    if (!scale.time_stamp) {
        return newVec3(1.0, 1.0, 1.0);
    }
    float *time_stamp = ((float *)scale.time_stamp->data);

    float prev_time = time_stamp[0];
    float next_time = time_stamp[1];
    int i;
    for (i=0; next_time<dt; i++) {
        if ( (i+1) >= scale.time_stamp->counter) {
            return arr_get(scale.value, Vec3, 0);
        }
        prev_time = next_time;
        next_time = time_stamp[i + 1];
    }

    Vec3 before = arr_get(scale.value, Vec3, i);
    Vec3 after = arr_get(scale.value, Vec3, i+1);

    if (prev_time > next_time) {
        printf("Issue in scale animation\n");
        printf("Previous time %f | Next time %f\n", prev_time, next_time);
        exit(1);
    }

    float time_step = (dt - prev_time) / (next_time - prev_time);

    return vec3_lerp(before, after, time_step);
}

AnimatedModel *get_animated_model() {

    AnimatedModel *anim_model = malloc(sizeof(AnimatedModel));
    ArrayList *joints = new_array_list(int);
    ArrayList *weights = new_array_list(float);


    char *data = read_file("assets/models/trooper.anim");

    Tokenizer tokenizer = {0};
    tokenizer.data = data;

    for (
        Token token = token_next(&tokenizer);
        tokenizer.data[tokenizer.cursor] && token_is_valid(&token);
        token = token_next(&tokenizer)
    ) {
        if (
            token.kind == Token_Declaration &&
            strcmp(token.info, "j") == 0
        ) {

            for (int j=0; j<4; j++) {
                token = token_next(&tokenizer);
                token_expected(&tokenizer, token, Token_Int);
                *arr_push(joints, int) = atoi(token.info);
            }
        }
        else if (
            token.kind == Token_Declaration &&
            strcmp(token.info, "w") == 0
        ) {
            for (int j=0; j<4; j++) {
                token = token_next(&tokenizer);
                token_expected(&tokenizer, token, Token_Float);
                *arr_push(weights, float) = atof(token.info);
            }
        }
    }
    anim_model->joints = joints;
    anim_model->weights = weights;

    free(data);

    return anim_model;
}

Mat4 animation_get_joint_transform(Joint *joint, float dt) {
    Animation anim = joint->animation;

    Vec3 translation = animation_get_translation(anim, dt);
    Vec4 rotation = animation_get_rotation(anim, dt);
    Vec3 scale = animation_get_scale(anim, dt);
    scale = newVec3(0.0, 0.0858, 0.0194);
    // scale = newVec3(0.5, 0.5, 0.5);

    Mat4 C = Mat4I();
    C = mat4_translate(&translation, &C);

    Quat q = quat_new(rotation.x, rotation.y, rotation.z, rotation.w);
    q = quat_normalize(q);
    Mat4 R = quat_to_mat4(q);

    // R = mat4_transpose(&R);
    C = mat4_multiply(&R, &C);
    // C = mat4_multiply(&C, &R);
    mat4_scale(&scale, &C);

    return C;
}

void animation_update_joint_transforms(
    Mat4 parent_transform, Joint *joint, float dt
) {
    Mat4 local_transform = animation_get_joint_transform(joint, dt);
    Mat4 C = joint->inverse_bind_matrix;


    //  Parent * Local * InverseBind
    //  InverseBind * Local * Parent
    
    // local_transform = mat4_multiply(&local_transform, &parent_transform);
    // local_transform = mat4_multiply(&local_transform, &C);

    // C = mat4_inverse(&C);
    local_transform = mat4_multiply(&parent_transform, &local_transform);
    local_transform = mat4_multiply(&local_transform, &C);

    // C = mat4_inverse(&C);
    // local_transform = mat4_multiply(&parent_transform, &local_transform);
    // local_transform = mat4_multiply(&local_transform, &C);

    print_mat4("Updated Local Transform", &local_transform);
    // C = mat4_inverse(&C);
    // C = mat4_transpose(&C);
    joint->local_transform = local_transform;

    for(Joint *child=joint->children; child; child=child->next) {
        animation_update_joint_transforms(local_transform, child, dt);
    }
}

void animation_update(AnimationController *anim_control, float dt) {
    anim_control->current_time += dt;
    Joint *root = &arr_get(anim_control->joints, Joint, 0); 
    Mat4 parent_transform = Mat4I();

    animation_update_joint_transforms(
        parent_transform, root, anim_control->current_time
    );
}


void load_skeleton(AnimatedModel *anim_model) {
    glBindVertexArray(anim_model->vao);
    store_int_in_attributes(&anim_model->jbo, 3, 4,
        anim_model->joints->counter * sizeof(int),
        (int *)anim_model->joints->data
    );
    log_if_err("Issue loading joints\n");
    glBindVertexArray(anim_model->vao);
    store_float_in_attributes(&anim_model->wbo, 4, 4,
        anim_model->weights->counter * sizeof(float),
        (float *)anim_model->weights->data
    );
    log_if_err("Issue loading weights\n");
}


void render_animation_entities(
    Renderer *rh, AnimationController *animation_controller
) {
    Vec3 light_color = rh->light->color;
    for (int i=0; i<100; i++) {
        // Entity entity = rh->debug_entities[i];
        Entity entity = LIST_GET(rh->debug_entities, i);

        if (entity.active == 0) {
            continue;
        }
        int shader;
        shader = rh->anim_shader;
        shader_push(shader);

        ArrayList *joints = animation_controller->joints;
        ArrayList *tmp = new_array_list(Mat4);
        for (int m=0; m<joints->counter; m++) {
            Joint joint = arr_get(joints, Joint, m);
            *arr_push(tmp, Mat4) = joint.local_transform;
        }
        int uniform_location = glGetUniformLocation(
            shader, "joint_transform");
        glUniformMatrix4fv(
            uniform_location, tmp->counter, GL_FALSE, (float *)tmp->data
        );
        arr_free(tmp);
        log_if_err("Animation Joint Transpose\n");
        if (
            !vec3_is_equal(entity.color, newVec3(0.0, 0.0, 0.0)) &&
            !vec3_is_equal(entity.color, light_color)
        ) {
            log_if_err(
                "Entity Renderer found an issue before loading light\n"
            );
            rh->light->color = entity.color;
            shader_load_light(shader, rh->light);
            log_if_err("Entity Renderer found problem loading lights");
        }

        glBindVertexArray(entity.model->vao);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        log_if_err("There was an issue with attributes\n");
        

        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position, entity.rotation, entity.scale
        );

        shader_load_matrix(
            shader, "transformation_matrix", &transformation_matrix
        );

        if ( (entity.fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
        glDrawElements(
            GL_TRIANGLES, entity.model->vertex_count,
            GL_UNSIGNED_INT, 0
        );

        if (!vec3_is_equal(entity.color, light_color)) {
            log_if_err("Issue before loading light\n");
            rh->light->color = light_color;
            shader_load_light(shader, rh->light);
            log_if_err("There was a problem loading lights");
        }
	}
}
