#include <iostream>
#include <queue>
#include <vector>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 70.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fov =  0.0f;

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
 //   glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, 0);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize glad" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    float coord_lines[18] =
          {0.0f, 0.0f, 0.0f, 100.0f,0.0f,0.0f,  // x
           0.0f, 0.0f, 0.0f, 0.0f,100.0f,0.0f,  // y 
           0.0f, 0.0f, 0.0f, 0.0f,0.0f,100.0f}; // z 

//    exit(0);
    Shader shader("myshader.vs", "myshader.fs");
    
  

    unsigned int vao2, vbo2;
    glGenVertexArrays(1, &vao2);
    glGenBuffers(1, &vbo2);
    
    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord_lines), coord_lines, GL_STATIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
    glPointSize(5.0f);
    glLineWidth(2.0f);

    float dt = 0.005f;
    float radius = 10.0f;
    while(!glfwWindowShouldClose(window)){

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
	
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) 800.f/600.f, 0.1f, 100.0f); 
        shader.setMat4("projection", projection);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 20.0), glm::vec3(0.0,0.0,0.0), glm::vec3(0.0, 1.0, 0.0)); 
    shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);	
	shader.setMat4("model", model);

	//coordinate system
	glBindVertexArray(vao2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glDrawArrays(GL_LINES, 0, 6);


        std::cout << "Camera: " << cameraPos.x  << " " << cameraPos.y << " "<< cameraPos.z << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao2);
    glDeleteBuffers(1, &vbo2);

    return 0;
}

   
    
    

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

/*void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

}*/

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov -= (float)yoffset;
    if(fov < 1.0f)
        fov = 1.0f;
    if(fov > 45.0f)
        fov = 45.0f;
}
