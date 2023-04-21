#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <stdexcept>
#include <optional>

#include "Renderer.hpp"
#include "Model.hpp"
#include "Camera.hpp"

// this function loads the text from a shader file into a string
std::string loadShaderSource(const std::string& filePath)
{
    std::ifstream inFile(filePath);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    return buffer.str();
}

// this function takes a string containing shader source code and
// compiles it so that it can be run on the GPU
static unsigned int compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::string typeStr = "";
        switch (type) {
        case GL_VERTEX_SHADER:
            typeStr = "vertex";
            break;
        case GL_TESS_CONTROL_SHADER:
            typeStr = "tessellation control";
            break;
        case  GL_TESS_EVALUATION_SHADER:
            typeStr = "tessellation evaluation";
            break;
        case GL_GEOMETRY_SHADER:
            typeStr = "geometry";
            break;
        case GL_FRAGMENT_SHADER:
            typeStr = "fragment";
            break;
        default:
            break;
        }

        std::cout
            << "failed to compile "
            << typeStr
            << " shader:"
            << std::endl
            << message
            << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

// this function takes a vertex and fragment as input and creates
// an opengl shader program
GLuint createShader(const std::string& vShdrFilePath, const std::string& fShdrFilePath)
{
    unsigned int program = glCreateProgram();

    std::string vShdrSrc = loadShaderSource(vShdrFilePath);
    std::string fShdrSrc = loadShaderSource(fShdrFilePath);
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vShdrSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fShdrSrc);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

GLuint createShader(
    const std::string& vShdrFilePath,
    const std::string& gShdrFilePath,
    const std::string& fShdrFilePath
) {
    unsigned int program = glCreateProgram();

    std::string vShdrSrc = loadShaderSource(vShdrFilePath);
    std::string gShdrSrc = loadShaderSource(gShdrFilePath);
    std::string fShdrSrc = loadShaderSource(fShdrFilePath);
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vShdrSrc);
    unsigned int gs = compileShader(GL_GEOMETRY_SHADER, gShdrSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fShdrSrc);

    glAttachShader(program, vs);
    glAttachShader(program, gs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);

    return program;
}

GLuint createShader(
    const std::string& vShdrFilePath,
    const std::string& tcShdrFilePath,
    const std::string& teShdrFilePath,
    const std::string& gShdrFilePath,
    const std::string& fShdrFilePath
) {
    unsigned int program = glCreateProgram();

    std::string vShdrSrc = loadShaderSource(vShdrFilePath);
    std::string tcShdrSrc = loadShaderSource(tcShdrFilePath);
    std::string teShdrSrc = loadShaderSource(teShdrFilePath);
    std::string gShdrSrc = loadShaderSource(gShdrFilePath);
    std::string fShdrSrc = loadShaderSource(fShdrFilePath);

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vShdrSrc);
    unsigned int tc = compileShader(GL_TESS_CONTROL_SHADER, tcShdrSrc);
    unsigned int te = compileShader(GL_TESS_EVALUATION_SHADER, teShdrSrc);
    unsigned int gs = compileShader(GL_GEOMETRY_SHADER, gShdrSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fShdrSrc);

    glAttachShader(program, vs);
    glAttachShader(program, tc);
    glAttachShader(program, te);
    glAttachShader(program, gs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(tc);
    glDeleteShader(te);
    glDeleteShader(gs);
    glDeleteShader(fs);

    return program;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modes) {

}

// these variables keep track of mouse state to help determine if the camera
// should be moved and by how much
double mxPrev = 0;
double myPrev = 0;
bool mousePressed = false;

// this function updates mouse state variables when left mouse button is pressed
void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        switch (action) {
        case GLFW_PRESS:
            glfwGetCursorPos(window, &mxPrev, &myPrev);
            mousePressed = true;
            break;
        case GLFW_RELEASE:
            mousePressed = false;
            break;
        default:
            break;
        }
    }
}

// this function keeps track of scrolling which is used for zooming in and out
double scrollOffset = 0;
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffset = yoffset;
}

// this function creates the window title which displays the current
// lighting calculating method
void makeWindowTitle(GLFWwindow* window) {
    std::string title = "Dylan Eck - CSCI544 Assignment 2";

    glfwSetWindowTitle(window, title.c_str());
}

int main(void)
{
    // initialize glfw and glad
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit()) exit(EXIT_FAILURE);

    int width = 720;
    int height = 480;
    GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    gladLoadGL();

    // sphere model used for skybox and rendering lights
    Model sphere = Model::fromOBJ("../../models/sphere.obj");
    GLuint sphereVao = sphere.vao;

    // quad used for lit portion of ground plane
    Model quad = Model::fromOBJ("../../models/quad.obj");
    GLuint quadVao = quad.vao;
    quad.modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(21.0f));

    // main model used to show the differences between different methods of
    // calculating lighting
    Model model = Model::fromOBJ("../../models/suzzane.obj");
    GLuint modelVao = model.vao;
    model.modelMatrix = glm::scale(
        glm::rotate(
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(0.0f, 3.0f, 0.0f)
            ),
            glm::radians(-30.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        ),
        glm::vec3(2.0f)
    );

    // shader program for rendering light sources
    GLuint basicProg = createShader(
        "./shaders/basic.vert.glsl",
        "./shaders/basic.frag.glsl"
    );

    // shader program for the main model and lit portion of the ground plain
    // uses per-fragment shading
    unsigned int modelProg = createShader(
        "./shaders/model.vert.glsl",
        "./shaders/model.frag.glsl"
    );

    // variables used to handle camera movement
    float angle = glm::radians(180.0f);
    double mx, my, dmx, dmy, dthetax, dthetay;
    float minCameraY = 0.1;
    float scrollSensitivity = 5.0;

    Camera camera;
    camera.position = glm::vec3(-5.0f, 5.0f, 8.0f);
    camera.target = glm::vec3(0.0f, 3.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // viewport matrix
    float half_width = width / 2.0f;
    float half_height = height / 2.0f;
    glm::mat4 viewportMatrix = glm::mat4(
        glm::vec4(half_width, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, half_height, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(half_width+0, half_height+0, 0.0f, 1.0f)
    );

    // this uniform buffer object stores the view and projection matrices
    // these matrices are used by all shaders
    GLuint globalUBOHandle;
    glGenBuffers(1, &globalUBOHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBOHandle);

    glBufferData(GL_UNIFORM_BUFFER, 192, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBOHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &camera.projectionMatrix[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, &viewportMatrix[0][0]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // this uniform buffer is used to store material properties
    // only used for objects that lighting is applied to
    glm::vec3 ambientReflectivity = glm::vec3(1.0f);
    glm::vec3 diffuseReflectivity = glm::vec3(1.0f);
    glm::vec3 specularReflectivity = glm::vec3(0.8f);
    float shininess = 100.0f;

    GLuint materialUBOHandle;
    glGenBuffers(1, &materialUBOHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, materialUBOHandle);

    glBufferData(GL_UNIFORM_BUFFER, 48, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUBOHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &ambientReflectivity[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &diffuseReflectivity[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, 12, &specularReflectivity[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 44, 4, &shininess);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // this uniform buffer object is used to store lighting information
    // only materials that lighting is applied to
    glm::vec3 lightPosition = glm::vec3(0.0f, 5.9f, 4.0f);
    glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, -0.7f);
    glm::vec3 ambientIntensity = glm::vec3(0.0f);
    float attenuationRadius = 30.0f;
    float innerCutoff = 0.93f;
    float outerCutoff = 0.90f;

    GLuint lightingUBOHandle;
    glGenBuffers(1, &lightingUBOHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, lightingUBOHandle);

    glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightingUBOHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &lightPosition);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &lightDirection);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, 12, &ambientIntensity);
    glBufferSubData(GL_UNIFORM_BUFFER, 44, 4, &attenuationRadius);
    glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &innerCutoff);
    glBufferSubData(GL_UNIFORM_BUFFER, 52, 4, &outerCutoff);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    sphere.modelMatrix =  glm::scale(
    glm::translate(
        glm::mat4(1.0f),
        lightPosition),
        glm::vec3(0.1f)
    );

    // set up renderer
    Renderer renderer;
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // main rendering loop
    while (!glfwWindowShouldClose(window)) {
        makeWindowTitle(window);

        // process inputs and handle camera input
        glm::mat4 view = camera.getViewMatrix();

        // compute camera movement from mouse dragging
        if (mousePressed) {
            glfwGetCursorPos(window, &mx, &my);

            dmx = -(mx - mxPrev);
            dmy = -(my - myPrev);

            mxPrev = mx;
            myPrev = my;

            dthetax = dmx * (glm::two_pi<double>() / width);
            dthetay = dmy * (glm::two_pi<double>() / height);

            glm::vec4 viewDir = -glm::transpose(view)[2];
            glm::vec3 rightVec = glm::transpose(view)[0];

            float cosAngle = glm::dot(viewDir, glm::vec4(camera.upVector, 1.0f));
            glm::mat4 xRot = glm::mat4(1.0f);
            xRot = glm::rotate(xRot, (float)dthetax, glm::vec3(0, 1, 0));
            camera.position = (xRot * (glm::vec4(camera.position, 1.0f) - glm::vec4(camera.target, 1.0f))) + glm::vec4(camera.target, 1.0f);

            glm::mat4 yRot = glm::mat4(1.0f);
            yRot = glm::rotate(yRot, (float)dthetay, rightVec);
            camera.position = (yRot * (glm::vec4(camera.position, 1.0f) - glm::vec4(camera.target, 1.0f))) + glm::vec4(camera.target, 1.0f);
        }

        // compute camera movement from scrolling
        if (abs(scrollOffset) > 0.5) {
            glm::vec4 viewDir = -glm::transpose(view)[2];
            glm::vec3 offset = glm::vec3(glm::normalize(viewDir * (float)scrollOffset));
            offset *= scrollSensitivity;
            camera.position += offset;
            scrollOffset = 0;
        }

        // set projection to main model projection (finite far plane)
        camera.setProjection(
            45.0f,
            (float)width / (float)height,
            0.3f,
            std::optional<float>(100.0f)
        );

        glBindBuffer(GL_UNIFORM_BUFFER, globalUBOHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &camera.projectionMatrix[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // update global ubo to account for camera movement
        glBindBuffer(GL_UNIFORM_BUFFER, globalUBOHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &camera.projectionMatrix[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.setShaderProgram(modelProg);
        renderer.drawModel(model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}