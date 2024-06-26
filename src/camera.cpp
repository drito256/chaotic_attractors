#include "../include/attractors/camera.h"

Camera::Camera(glm::vec3 pos, float radius, float fov){
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    this->pos.z = pos.z;
    this->radius = radius;
    this->fov = fov;
}

glm::vec3 Camera::get_position(){
    return this->pos;
}

glm::mat4 Camera::get_view_matrix(){
    return glm::lookAt(this->get_radius() * this->get_position(),
                       glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

glm::mat4 Camera::get_projection_matrix(int width, int height){
    return glm::perspective(glm::radians(this->get_fov()),
                           (float)width/height, 0.01f, 500.f);
}

float Camera::get_radius(){
    return this->radius;
}
void Camera::set_radius(float radius){
    this->radius = glm::abs(radius);
}

float Camera::get_speed_xz(){
    return this->speed_xz;
}
void Camera::set_speed_xz(float speed){
    this->speed_xz = speed;
}

float Camera::get_speed_y(){
    return this->speed_y;
}
void Camera::set_speed_y(float speed){
    this->speed_y = speed;
}

float Camera::get_fov(){
    return this->fov;
}
void Camera::set_fov(float fov){
    this->fov = fov;
}

void Camera::update_pos(float user_input_y, float user_input_xz){
    this->pos.x = cos(user_input_xz) + sin(user_input_xz);
    this->pos.y = user_input_y;
    this->pos.z = -sin(user_input_xz) + cos(user_input_xz);
}
