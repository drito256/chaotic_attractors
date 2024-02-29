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

#include "../include/attractors/shader.h"
#include "../include/attractors/camera.h"
#include "../include/attractors/point.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void reinit_points(Point* points, Point* triangles, int vbo1, int vao1, int vbo2, int vao2);

float user_input_xz = 0.0f , user_input_y = 0.0f;
float camera_speed_xz, camera_speed_y;

float fov;

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
    std::vector<int> constant_num = {3, 6, 2, 4, 3, 2, 1, 3, 5, 1 };
    std::vector<std::vector<float>> constants = 
                                          {{10.f, 28.f, 8.f/3.f},
                                          {0.5f, 3.5f, 0.7f, 0.6f, 0.95f, 0.1f},
                                          {0.2f, 5.7f},
                                          {5.f, 10.f, 0.38f, 3.f},
                                          {0.2f, 0.01f, 0.4f},
                                          {1.89f, 4.f},
                                          {1.5f},
                                          {2.07f, 1.f, 1.79f},
                                          {3.f, 2.7f, 1.7f, 2.f, 9.f},
                                          {-0.208186f}
                                          };
    float equation_constant[6];
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

    Shader shader("shaders/myshader.vs", "shaders/myshader.fs");

    Camera camera(glm::vec3(1.f, 1.f, 1.f), 100.f, 45.f);
    camera_speed_y = camera.get_speed_y();
    camera_speed_xz = camera.get_speed_xz();
    fov = camera.get_fov(); 

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
    glLineWidth(2.0f);

    float dt = 0.01f;
    glm::vec4 start_color = {1.0f,1.0f,1.0f,1.0f};
    glm::vec4 end_color = {0.0f,0.0f,0.0f,1.0f};
    int theta;
    float y_axis;
    int chosen_equation = 0;

    const char* attractor_name[10] = {
        "Lorenz attractor",
        "Aizawa attractor",
        "Rossler attractor",
        "Chen attractor",
        "Four - wing attractor",
        "Halvorsen attractor",
        "Nose - hoover attractor",
        "Sportt attractor",
        "Dadras attractor",
        "Thomas attractor"
    };

    int camera_radius[10] = {
        100, 5, 50, 200, 10, 30, 40, 15, 30, 20
    };
    bool show_net = true;

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

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        camera.set_fov(fov);
        glm::mat4 projection = glm::perspective(glm::radians(camera.get_fov()), (float)width/height, 0.01f, 1000.0f); 
        shader.setMat4("projection", projection);
        
        camera.update_pos(user_input_y, user_input_xz);
        theta = (int)(user_input_xz*180.f)/glm::pi<float>(); //conversion into degrees
        y_axis = camera.get_position().y;

	    glm::mat4 view = glm::lookAt(camera.get_radius() * camera.get_position(), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 1.0, 0.0)); 

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
            
            point[i].update(chosen_equation, dt);
            //returning (dx,dy,dz) of chosen equation so other two points of triangle can be calculated
            glm::vec3 d = triangle[i*3].update(chosen_equation, dt);

            glm::vec3 perpendicular = glm::cross(glm::vec3(triangle[i*3].x,triangle[i*3].y,triangle[i*3].z),glm::vec3(triangle[i*3].x+0.01f,triangle[i*3].y,triangle[i*3].z));
            
            glm::vec3 unit_p = glm::normalize(perpendicular)/50.f;    
            glm::vec3 unit_d = glm::normalize(d)/20.f;

            //2nd triangle point
            triangle[i*3+1].x = triangle[i*3].x - unit_d.x + unit_p.x;
            triangle[i*3+1].y = triangle[i*3].y - unit_d.y + unit_p.y;
            triangle[i*3+1].z = triangle[i*3].z - unit_d.z + unit_p.z;
            
            //3rd triangle point
            triangle[i*3+2].x = triangle[i*3].x - unit_d.x - unit_p.x;
            triangle[i*3+2].y = triangle[i*3].y - unit_d.y - unit_p.y;
            triangle[i*3+2].z = triangle[i*3].z - unit_d.z - unit_p.z;
            
            std::cout << unit_d.y << std::endl;
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
        if(show_net){
            shader.setBool("coord_net", true);
            shader.setBool("coord_sys", false);
            glLineWidth(2.0f); 
            glBindVertexArray(coord_sys_vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER, coord_sys_vbo[1]);
            glDrawArrays(GL_LINES, 0, 100);
        }

        float* ptr1 = &start_color.x;
        float* ptr2 = &end_color.x;

        ImGui::Begin("Properties");
        ImGui::SliderFloat("Timestamp", &dt,0.f,0.0501f);
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
        ImGui::Text("%.3f", y_axis);
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::Checkbox("Show coordinate system net", &show_net);

        if(ImGui::Button("Reset simulation")){
           for(int j=0;j<100;j++)
               trail[j].clear();
           reinit_points(point, triangle, vbo, vao, vbo4, vao4);
        }
        ImGui::End();

        ImGui::Begin("Attractors");
        
        for(int i = 0; i < 10; i++){
            if(ImGui::Selectable(attractor_name[i]))
            {
                //empty trail before switching so that it doesnt connect with new points
                for(int j=0;j<100;j++)
                    trail[j].clear();

                chosen_equation = i;
                reinit_points(point, triangle, vbo, vao, vbo4, vao4);
                camera.set_radius(camera_radius[i]);
            }
        }
        ImGui::End();

        //Sliders that enable user to change equation constants
        ImGui::Begin("Constants");
        for(int i = 0; i < constant_num.at(chosen_equation); i++){
                char str[20];
                sprintf(str, "Constant %d", i + 1);
                ImGui::SliderFloat(str, &equation_constant[i], 0.f, 20.f);            
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
    glDeleteVertexArrays(1, &coord_sys_vao[0]);
    glDeleteBuffers(1, &coord_sys_vao[0]);
    glDeleteVertexArrays(1, &coord_sys_vao[1]);
    glDeleteBuffers(1, &coord_sys_vao[1]);
    glDeleteVertexArrays(1, &vao3);
    glDeleteBuffers(1, &vbo3);
    glDeleteVertexArrays(1, &vao4);
    glDeleteBuffers(1, &vbo4);

    return 0;
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && user_input_y < 0.98f)
	    user_input_y += camera_speed_y;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && user_input_y > -0.98f)
	    user_input_y -= camera_speed_y;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	    user_input_xz -= camera_speed_xz;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	    user_input_xz += camera_speed_xz;

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

//this looks ugly, should probably be rewritten
void reinit_points(Point* points, Point* triangles, int vbo1, int vao1, int vbo2, int vao2){
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

    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * 100, points, GL_DYNAMIC_DRAW);
   
    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * 300, triangles, GL_DYNAMIC_DRAW);
 

