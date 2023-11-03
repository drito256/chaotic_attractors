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

glm::vec3 Point::update(int chosen_equation,  float dt){
    float dx, dy, dz;
    float x = this->x;
    float y = this->y;
    float z = this->z;

    switch(chosen_equation){
        case 0: // lorenz
            dx = ((y - x) * 10.0f);
            dy = (x * (28.f - z) - y);
            dz = (x * y - 8.f / 3.f * z);
        break;
        
        case 1: // aizawa
            dx = (z - 0.5f) * x - 3.5f * y;
            dy = 3.5f * x + (z - 0.7f) * y;
            dz = 0.6f + 0.95f * z - z * z * z - x * x + 0.1f * z * x * x * x;
        break;

        case 2: // rossler
            dx = -(y + z);
            dy = x + 0.2f * y;
            dz = 0.2f + z * (x - 5.7f);
        break;

        case 3: // chen
            dx = 5.f * x  - y * z;
            dy = -10.f * y + x * z;
            dz = -0.38f * z + x * y / 3.f;
        break;

        case 4: // four - wing
            dx = 0.2f * x + y * z;
            dy = 0.01f * x + (-0.4f  * y) - x * z;
            dz = -z -  x * y;
        break;

        case 5: // halvorsen
            dx = -1.89f * x - 4 * y - 4 * z - y * y;
            dy = -1.89f * y - 4 * z - 4 * x - z * z;
            dz = -1.89f * z - 4 * x - 4 * y - x * x;
        break;

        case 6: // nose - hoover
            dx = y;
            dy = -x + y * z;
            dz = 1.5f - y * y;
        break;

        case 7: // sprott
            dx = y + 2.07f * x * y + x * z;
            dy = 1 - 1.79f * x * x + y * z;
            dz = x - x * x - y * y;
        break;

        case 8: // dadras
           dx = y - 3.f * x + 2.7f * y * z;
           dy = 1.7f * y - x * z + z;
           dz = 2.f * x * y - 9.f * z;
        break;

        case 9: // thomas
          dx = glm::sin(y) - 0.208186f * x;
          dy = glm::sin(z) - 0.208186f * y;
          dz = glm::sin(x) - 0.208186f * z;
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
