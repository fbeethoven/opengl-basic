#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 text_coords;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 joint_ind;
layout (location = 4) in vec4 weights;

out vec2 out_text_coords;
out vec3 surface_normal;
out vec3 to_light;
out vec3 out_light_color;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

uniform vec3 light_position;
uniform vec3 light_color;

uniform mat4 joint_transform[70];

void main() {

    vec4 total_position = vec4(0.0f);
    vec3 total_normal = vec3(0.0f);
    for(int i=0 ; i<4; i++) {
        // if (i == 0) {
        //     if (joint_ind[i] >= 66 ) {
        //         total_position =  vec4(0.0f);
        //         total_normal = normal;
        //         break;
        //     }
        //     mat4 j_transform = joint_transform[0];
        //     total_position =  j_transform * vec4(position,1.0f);
        //     total_normal = normal;
        //     break;
        // }
        if(joint_ind[i] == 0) {
            // if (i == 0) {
            // mat4 j_transform = joint_transform[joint_ind[i]];
            // total_position =  j_transform * vec4(position,1.0f);
            // total_position = vec4(position,1.0f);
            // total_normal = normal;
            // break;
            // }
            continue;
        }

        mat4 j_transform = joint_transform[joint_ind[i]];
        vec4 local_position =  j_transform * vec4(position,1.0f);
        total_position += local_position * weights[i];
        // total_position += local_position * 1.0;
        vec3 local_normal = mat3(j_transform) * normal;
        total_normal += local_normal * weights[i];
        // total_normal += local_normal * 1.0;
    }


    vec4 world_position = transformation_matrix * total_position;
    gl_Position = projection_matrix * view_matrix * world_position;

    out_text_coords = text_coords;

    vec3 surface_normal_abs = (transformation_matrix * vec4(total_normal, 0.0)).xyz;
    surface_normal = normalize(surface_normal_abs);

    // vec3 to_light_abs = light_position - world_position.xyz;
    to_light = normalize(-light_position);
    out_light_color = light_color;
}
