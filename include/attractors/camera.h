#ifndef CAMERA_H
#define CAMERA_H

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"

class Camera{
    private:
        glm::vec3 pos;
        float radius;
        float fov;
        float speed_xz = glm::pi<float>() / 100.f;
        float speed_y = 0.02f;

   public:
        Camera(glm::vec3 pos, float radius = 50.f, float fov = 45.f);
        glm::vec3 get_position();
        float get_radius();
        void set_radius(float radius);

        float get_speed_xz();
        void set_speed_xz(float speed);

        float get_speed_y();
        void set_speed_y(float speed);

        float get_fov();
        void set_fov(float fov);

        void update_pos(float user_input_y, float user_input_xz);
        void get_input();

};
#endif
