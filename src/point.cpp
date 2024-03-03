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

glm::vec3 Point::update(int chosen_equation,  float dt, float *constants){
    float dx, dy, dz;
    float x = this->x;
    float y = this->y;
    float z = this->z;

    switch(chosen_equation){
        case 0: // lorenz
            dx = ((y - x) * (*constants));
            dy = (x * (*(constants + 1) - z) - y);
            dz = (x * y - (*(constants + 2)) * z);
        break;
        
        case 1: // aizawa
            dx = (z - (*constants)) * x - (*(constants+1)) * y;
            dy = *(constants+1) * x + (z - (*(constants+2))) * y;
            dz = *(constants+3) + (*(constants+4)) * z - z * z * z - x * x + (*(constants+5)) * z * x * x * x;
        break;

        case 2: // rossler
            dx = -(y + z);
            dy = x + (*(constants)) * y;
            dz = *(constants) + z * (x - (*(constants + 1)));
        break;

        case 3: // chen
            dx = *(constants) * x  - y * z;
            dy = -(*(constants + 1)) * y + x * z;
            dz = -(*(constants + 2)) * z + x * y / (*(constants+3));
        break;

        case 4: // four - wing
            dx = *(constants) * x + y * z;
            dy = *(constants+1) * x + (*(constants+2)  * y) - x * z;
            dz = -z - x * y;
        break;

        case 5: // halvorsen
            dx = -(*constants) * x - (*(constants+1)) * y - (*(constants+1)) * z - y * y;
            dy = -(*constants) * y - (*(constants+1))  * z - (*(constants+1)) * x - z * z;
            dz = -(*constants) * z - (*(constants+1))  * x -  (*(constants+1)) * y - x * x;
        break;

        case 6: // nose - hoover
            dx = y;
            dy = -x + y * z;
            dz = *constants - y * y;
        break;

        case 7: // sprott
            dx = y + (*constants) * x * y + x * z;
            dy = *(constants+1) - (*(constants+2)) * x * x + y * z;
            dz = x - x * x - y * y;
        break;

        case 8: // dadras
           dx = y - (*constants) * x + (*(constants+1)) * y * z;
           dy = *(constants+2) * y - x * z + z;
           dz = *(constants+3) * x * y - (*(constants+4)) * z;
        break;

        case 9: // thomas
          dx = glm::sin(y) - (*constants) * x;
          dy = glm::sin(z) - (*constants) * y;
          dz = glm::sin(x) - (*constants) * z;
        break;
    }

    dx *= dt;
    dy *= dt;
    dz *= dt;

    this->x += dx;
    this->y += dy;
    this->z += dz;

    return glm::vec3(dx, dy, dz);
}
