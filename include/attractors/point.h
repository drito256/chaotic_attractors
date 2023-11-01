#ifndef POINT_H
#define POINT_H

#include "../glm/glm.hpp"

class Point{
    public:
        float x, y, z;

        Point();
        Point(float x, float y, float z);
        Point(glm::vec3 point);
        void update(int chosen_equation, float dt);
};

#endif
