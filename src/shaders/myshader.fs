#version 330 core

uniform bool coord_sys, coord_net;
uniform vec4 start_color, end_color;
uniform int trail_index;

out vec4 final_color;
in vec4 position;

void main(){

    if(coord_net)
        final_color = vec4(0.4f, 0.4f, 0.4f, 0.1f);
    else if(coord_sys)
        final_color  = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    else
        final_color = mix(end_color, start_color, trail_index/50.f);
}
