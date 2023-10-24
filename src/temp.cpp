
#include <iostream>
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;

const GLchar* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 position;
    out vec3 newPosition;

    uniform float dt;
    uniform float sigma;
    uniform float rho;
    uniform float beta;

    uniform mat4 projectionMatrix;
    uniform mat4 modelViewMatrix;

    void main()
    {
        // Get the original position of the vertex
        vec3 originalPosition = position;

        // Calculate the Lorenz system
        float dx = sigma * (originalPosition.y - originalPosition.x);
        float dy = originalPosition.x * (rho - originalPosition.z) - originalPosition.y;
        float dz = originalPosition.x * originalPosition.y - beta * originalPosition.z;

        // Update the vertex position
        newPosition = originalPosition + vec3(dx, dy, dz) * dt;

        // Set the transformed position
        gl_Position = projectionMatrix * modelViewMatrix * vec4(newPosition, 1.0);
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 330 core

    out vec4 color;

    void main()
    {
        color = vec4(1.0, 0.0, 0.0, 1.0);  // Red color
    }
)";

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lorenz Attractor Example", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetKeyCallback(window, keyCallback);

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed\n" << infoLog << std::endl;
        return -1;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed\n" << infoLog << std::endl;
        return -1;
    }

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed\n" << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define vertex data
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,  // Vertex 1
         0.5f, -0.5f, 0.0f,  // Vertex 2
         0.0f,  0.5f, 0.0f   // Vertex 3
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Set up shader uniforms
    GLuint dtLocation = glGetUniformLocation(shaderProgram, "dt");
    GLuint sigmaLocation = glGetUniformLocation(shaderProgram, "sigma");
    GLuint rhoLocation = glGetUniformLocation(shaderProgram, "rho");
    GLuint betaLocation = glGetUniformLocation(shaderProgram, "beta");
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    GLuint modelViewMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewMatrix");

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Update shader uniform values
        float dt = 0.01;      // Time step size
        float sigma = 10.0;
        float rho = 28.0;
        float beta = 8.0 / 3.0;
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        glUseProgram(shaderProgram);
        glUniform1f(dtLocation, dt);
        glUniform1f(sigmaLocation, sigma);
        glUniform1f(rhoLocation, rho);
        glUniform1f(betaLocation, beta);
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

