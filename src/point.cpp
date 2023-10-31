#include "../include/attractors/point.h"


Point::Point(){
}

Point::Point(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point(glm::vec3 point){
    this->x = point.x;
    this->y = point.y;
    this->z = point.z;
}

void Point::update(float dx, float dy, float dz){
    this->x += dx;
    this->y += dy;
    this->z += dz;
}
