#include <iostream>
#include <queue>
#include <vector>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"

#include "shader.h"

class Point{
    public:
        float x;
        float y;
        float z;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void init_points(Point* points, Point* triangles);

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f /2.0f;
float lastY = 600.0f / 2.0f;
float user_input_xz = 0.0f , user_input_y = 0.0f;
float camera_speed = glm::pi<float>() / 100.0f;

float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main(){
    time_t t;
    srand((unsigned) time(&t)); 

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(800, 600, "test_app", NULL, NULL);
    if(window == NULL){
        std:: cout << "Failed to create glfw window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, 0);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float coord_lines[18] =
          {-1000.0f, 0.0f, 0.0f, 1000.0f,0.0f,0.0f,  // x
           0.0f, -1000.0f, 0.0f, 0.0f,1000.0f,0.0f,  // y 
           0.0f, 0.0f, -1000.0f, 0.0f,0.0f,1000.0f}; // z 

    float xz_lines[300] = { };

    for(int i=0;i<150;i+=6){
        xz_lines[i] = (-20.0f +  i * 5/18.f)*5;
        xz_lines[i+1] = 0.f;
        xz_lines[i+2] = -100.0f;
        xz_lines[i+3] = (-20.0f + i * 5/18.f)*5;
        xz_lines[i+4] = 0.f;
        xz_lines[i+5] = 100.f;
    }

    for(int i=150;i<300;i+=6){
        xz_lines[i] = -100.f;
        xz_lines[i+1] = 0.f;
        xz_lines[i+2] = (-20.0f + (i-150) * 5/18.f)*5;
        xz_lines[i+3] = 100.0f;
        xz_lines[i+4] = 0.f;
        xz_lines[i+5] = (-20.0f + (i-150) * 5/18.f)*5;
    }

    Point point[100];
    std::vector<Point> trail[100];
    Point triangle[300];

    for(int i=0;i < 100; i++){
        point[i].x = (float)rand()/RAND_MAX/10.0f;
        point[i].y = (float)rand()/RAND_MAX/10.0f;
        point[i].z = (float)rand()/RAND_MAX/10.0f;
        
        if(rand() % 2 == 0)
            point[i].x *= -1.0f;
        if(rand() % 2 == 0)
            point[i].y *= -1.0f;
        if(rand() % 2 == 0)
            point[i].z *= -1.0f;

        triangle[i*3].x = point[i].x;
        triangle[i*3].y = point[i].y;
        triangle[i*3].z = point[i].z;
    }

    Shader shader("myshader.vs", "myshader.fs");
    
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_DYNAMIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);    
   
    
    // one for main coord lines, and other for all other
    unsigned int coord_sys_vao[2], coord_sys_vbo[2];
    glGenVertexArrays(1, &coord_sys_vao[0]);
    glGenBuffers(1, &coord_sys_vbo[0]);
    
    glBindVertexArray(coord_sys_vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, coord_sys_vao[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord_lines), coord_lines, GL_STATIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenVertexArrays(1, &coord_sys_vao[1]);
    glGenBuffers(1, &coord_sys_vbo[1]);
    
    glBindVertexArray(coord_sys_vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, coord_sys_vao[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xz_lines), xz_lines, GL_STATIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    unsigned int vao3, vbo3;
    
    glGenVertexArrays(1, &vao3);
    glGenBuffers(1, &vbo3);
    
    glBindVertexArray(vao3);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(trail), trail, GL_DYNAMIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int vao4, vbo4;
    
    glGenVertexArrays(1, &vao4);
    glGenBuffers(1, &vbo4);
    
    glBindVertexArray(vao4);
    glBindBuffer(GL_ARRAY_BUFFER, vbo4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_DYNAMIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glClearColor(33.f/255.f, 47.f/255.f, 61.f/255.f, 1.0f);
    glPointSize(5.0f);
    glLineWidth(2.0f);

    float dt = 0.01f;
    float dx = 0.f,dy= 0.f,dz = 0.f;
    glm::vec4 start_color = {1.0f,1.0f,1.0f,1.0f};
    glm::vec4 end_color = {0.0f,0.0f,0.0f,1.0f};
    int theta, y_axis;
    int chosen_equation = 1;

    while(!glfwWindowShouldClose(window)){
        glLineWidth(2.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = static_cast<float> (glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
       	
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) 800.f/600.f, 0.01f, 1000.0f); 
        shader.setMat4("projection", projection);

        float cam_x1 = cos(user_input_xz) + sin(user_input_xz);
        float cam_z1 = -sin(user_input_xz) + cos(user_input_xz);

        float cam_x2 = cam_x1;
        float cam_y2 = user_input_y;
        float cam_z2 = cam_z1;

        if(user_input_y > 1.f) user_input_y=1.f;
        if(user_input_y < -1.f) user_input_y=-1.f;

        float radius = sqrt(cam_x2*cam_x2 + cam_y2*cam_y2 + cam_z2*cam_z2)*50;
        theta = (int)(user_input_xz*180.f)/glm::pi<float>();
        y_axis = (int)(user_input_y*180.f)/glm::pi<float>();

	    glm::mat4 view = glm::lookAt(radius* glm::vec3( cam_x2 , cam_y2, cam_z2), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 1.0, 0.0)); 
        shader.setMat4("view", view);
	    glm::mat4 model = glm::mat4(1.0f);	
	    shader.setMat4("model", model);
    

        shader.setBool("coord_sys",false);
        shader.setBool("coord_net", false);
        for(int i=0;i < 100; i++){
            float x = point[i].x;
            float y = point[i].y;
            float z = point[i].z;

            trail[i].push_back((Point){x,y,z});
            if(trail[i].size() > 50){
               trail[i].erase(trail[i].begin());
            }
            
            switch(chosen_equation){
                case 1:
                   dx = ((y-x)*10.0f);
                   dy = (x * (28.0f - z) - y);
                   dz = (x * y - 8.0f/3.0f * z);
                break;

                case 2:
                    dx = (z-0.5f) * x - 3.5f * y;
                    dy = 3.5f*x + (z-0.7f)*y;
                    dz = 0.6f + 0.95f*z - z*z*z - x*x + 0.1f*z*x*x*x;
                break;
                
                case 3:
                break;

                case 4:
                break;

            } 

            dx*=dt;
            dy*=dt;
            dz*=dt;
            point[i].x += dx;
            point[i].y += dy;
            point[i].z += dz;
            
            triangle[i*3].x += dx;
            triangle[i*3].y += dy;
            triangle[i*3].z += dz;

            glm::vec3 perpendicular =
            glm::cross(glm::vec3(triangle[i*3].x,triangle[i*3].y,triangle[i*3].z),glm::vec3(triangle[i*3].x+0.01f,triangle[i*3].y,triangle[i*3].z));
            
            glm::vec3 unit_p = glm::normalize(perpendicular)/100.f;    
            glm::vec3 unit_d = glm::normalize(glm::vec3(dx,dy,dz))/20.f;


            triangle[i*3+1].x = triangle[i*3].x - unit_d.x + unit_p.x;
            triangle[i*3+1].y = triangle[i*3].y - unit_d.y + unit_p.y;
            triangle[i*3+1].z = triangle[i*3].z - unit_d.z + unit_p.z;

            triangle[i*3+2].x = triangle[i*3].x - unit_d.x - unit_p.x;
            triangle[i*3+2].y = triangle[i*3].y - unit_d.y - unit_p.y;
            triangle[i*3+2].z = triangle[i*3].z - unit_d.z - unit_p.z;

            glBindVertexArray(vao3);
            glBindBuffer(GL_ARRAY_BUFFER, vbo3);

            glBufferSubData(GL_ARRAY_BUFFER, 0, trail[i].size() * sizeof(Point), trail[i].data());
            for(int k =0;k<(int)trail[i].size()-1;k++){
                shader.setInt("trail_index", k+1);
                glDrawArrays(GL_LINES, k , 2);
            }
        }
            
        shader.setVec4("start_color", start_color);
        shader.setVec4("end_color", end_color);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point), point);
        //glDrawArrays(GL_POINTS, 0 ,100); 

        glBindVertexArray(vao4);
        glBindBuffer(GL_ARRAY_BUFFER, vbo4);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle), triangle);
        glDrawArrays(GL_TRIANGLES, 0 ,300); 


        //main lines of coordinate system
        shader.setBool("coord_sys", true);
        glLineWidth(5.0f); 
	    glBindVertexArray(coord_sys_vao[0]);
        glBindBuffer(GL_ARRAY_BUFFER, coord_sys_vbo[0]);
	    glDrawArrays(GL_LINES, 0, 6);
       
        //all other lines
        shader.setBool("coord_net", true);
        shader.setBool("coord_sys", false);
        glLineWidth(2.0f); 
	    glBindVertexArray(coord_sys_vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, coord_sys_vbo[1]);
	    glDrawArrays(GL_LINES, 0, 100);


        float* ptr1 = &start_color.x;
        float* ptr2 = &end_color.x;

        ImGui::Begin("Properties");
        ImGui::SliderFloat("Timestamp", &dt,0.f,0.05f);
        ImGui::ColorEdit4("Head color", ptr1);
        ImGui::ColorEdit4("Tail color", ptr2);

        ImGui::NewLine();
        ImGui::Separator();

        ImGui::Text("Camera Theta angle: ");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 100, 255)));
        ImGui::Text("%d", theta);
        ImGui::PopStyleColor();

        ImGui::Text("Camera Y-axis: ");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 100, 255)));
        ImGui::Text("%d", y_axis);
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::End();

        ImGui::Begin("Attractors");

        if(ImGui::Selectable("Lorentz attractor")){
            chosen_equation = 1;
//            init_points(point, triangle);
        }
        else if(ImGui::Selectable("Aizawa attractor")){
//            chosen_equation = 2;
//            init_points(point, triangle);
        }
        else if(ImGui::Selectable("3")){
//            chosen_equation = 3;
//            init_points(point, triangle);
        }
        else if(ImGui::Selectable("4")){
//            chosen_equation = 4;
//            init_points(point, triangle);
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return 0;
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	    user_input_y += camera_speed;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	    user_input_y -= camera_speed;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	    user_input_xz -= camera_speed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	    user_input_xz += camera_speed;

}
   
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov -= (float)yoffset;
    if(fov < 1.0f)
        fov = 1.0f;
    if(fov > 45.0f)
        fov = 45.0f;
}

void init_points(Point* points, Point* triangles){
    for(int i=0;i < 100; i++){
        points[i].x = (float)rand()/RAND_MAX/10.0f;
        points[i].y = (float)rand()/RAND_MAX/10.0f;
        points[i].z = (float)rand()/RAND_MAX/10.0f;
        
        if(rand() % 2 == 0)
            points[i].x *= -1.0f;
        if(rand() % 2 == 0)
            points[i].y *= -1.0f;
        if(rand() % 2 == 0)
            points[i].z *= -1.0f;

        triangles[i*3].x = points[i].x;
        triangles[i*3].y = points[i].y;
        triangles[i*3].z = points[i].z;
    }
}

