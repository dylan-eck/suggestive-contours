#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
#include <unordered_map>
#include <unordered_set>

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


double mxPrev = 0;
double myPrev = 0;
bool mousePressed = false;
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

double scrollOffset = 0;
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffset = yoffset;
}


void makeWindowTitle(GLFWwindow* window) {
    std::string title = "suggestive-contours";
    glfwSetWindowTitle(window, title.c_str());
}

int main(void)
{
    // initialize glfw and glad
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit()) exit(EXIT_FAILURE);

    int width = 1280;
    int height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    gladLoadGL();

    Model model = Model::fromOBJ("../../models/bunny.obj");
    GLuint modelVao = model.vao;
    model.modelMatrix = glm::scale(
        glm::rotate(
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(0.0f, 3.0f, 0.0f)
            ),
            glm::radians(0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        ),
        glm::vec3(20.0f)
    );

    model.initGPUresources();

    GLuint sg_img_prog = createShader(
        "./shaders/sg_img.vert.glsl",
        "./shaders/sg_img.frag.glsl"
    );

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

    float radius = 5.0f;
    float theta = glm::radians(140.0f);
    float x = radius * glm::cos(theta);
    float z = radius * glm::sin(theta);

    camera.position = glm::vec3(x, 5.0f, z);
    camera.target = glm::vec3(0.0f, 3.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    camera.setProjection(
        45.0f,
        (float)width / (float)height,
        0.3f,
        std::optional<float>(100.0f)
    );

    float half_width = width / 2.0f;
    float half_height = height / 2.0f;
    glm::mat4 viewportMatrix = glm::mat4(
        glm::vec4(half_width, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, half_height, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(half_width+0, half_height+0, 0.0f, 1.0f)
    );

    // transform matrices UBO setup
    GLuint globalUBOHandle;
    glGenBuffers(1, &globalUBOHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBOHandle);

    glBufferData(GL_UNIFORM_BUFFER, 192, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBOHandle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &camera.projectionMatrix[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, &viewportMatrix[0][0]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // material info UBO setup
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

    // lighting info UBO setup
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

    // viewport framebuffer setup
    GLuint vpFramebuffer;
    glGenFramebuffers(1, &vpFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, vpFramebuffer);

    GLuint vpTexture;
    int vpWidth = 400;
    int vpHeight = 400;
    glGenTextures(1, &vpTexture);
    glBindTexture(GL_TEXTURE_2D, vpTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vpWidth, vpHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vpTexture, 0);

    GLuint vpRbo;
    glGenRenderbuffers(1, &vpRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, vpRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, vpWidth, vpHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, vpRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("error: viewport frame buffer incomplete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // based on:
    // https://learnopengl.com/Advanced-OpenGL/Framebuffers

    // post processing frambuffer setup
    GLuint postFramebufferA;
    glGenFramebuffers(1, &postFramebufferA);
    glBindFramebuffer(GL_FRAMEBUFFER, postFramebufferA);

    GLuint postTextureA;
    glGenTextures(1, &postTextureA);
    glBindTexture(GL_TEXTURE_2D, postTextureA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vpWidth, vpHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postTextureA, 0);

    GLuint postRboA;
    glGenRenderbuffers(1, &postRboA);
    glBindRenderbuffer(GL_RENDERBUFFER, postRboA);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, vpWidth, vpHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, postRboA);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("error: post processing frame buffer b incomplete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // imgui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &imgui_io = ImGui::GetIO(); (void)imgui_io;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imgui_io.IniFilename = "./config/imgui.ini";
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    Renderer renderer;

    float r = 4.81;
    float s = 0.77;
    float d = 0.05;

    bool viewportIsFocused = false;

    while (!glfwWindowShouldClose(window)) {
        makeWindowTitle(window);

        glm::mat4 view = camera.getViewMatrix();

        // compute camera movement from mouse dragging
        if (mousePressed && viewportIsFocused) {
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
        if (abs(scrollOffset) > 0.5 && viewportIsFocused) {
            glm::vec4 viewDir = -glm::transpose(view)[2];
            glm::vec3 offset = glm::vec3(glm::normalize(viewDir * (float)scrollOffset));
            offset *= scrollSensitivity;
            camera.position += offset;
            scrollOffset = 0;
        }

        camera.setProjection(
            45.0f,
            (float)vpWidth / (float)vpHeight,
            0.3f,
            std::optional<float>(100.0f)
        );

        // update global ubo to account for camera movement
        glBindBuffer(GL_UNIFORM_BUFFER, globalUBOHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &camera.projectionMatrix[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glViewport(0, 0, vpWidth, vpHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, postFramebufferA);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        renderer.setShaderProgram(modelProg);
        renderer.drawModel(model);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // post processing
        glBindFramebuffer(GL_FRAMEBUFFER, vpFramebuffer);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(sg_img_prog);
        glUniform1f(glGetUniformLocation(sg_img_prog, "r"), r);
        glUniform1f(glGetUniformLocation(sg_img_prog, "s"), s);
        glUniform1f(glGetUniformLocation(sg_img_prog, "d"), d);
        glBindVertexArray(-1);
        glBindTexture(GL_TEXTURE_2D, postTextureA);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport();

        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 600), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("Contour Parameters");
        ImGui::DragFloat("r", &r, 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat("s", &s, 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat("d", &d, 0.001f, 0.0f, 0.0f, "%.3f");
        ImGui::End();

        ImGui::SetNextWindowSizeConstraints(ImVec2(480, 480), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("viewport");
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        viewportIsFocused = ImGui::IsWindowFocused();

        if (windowSize.x != vpWidth || windowSize.y != vpHeight) {
            vpWidth = static_cast<int>(windowSize.x);
            vpHeight = static_cast<int>(windowSize.y);
            glBindTexture(GL_TEXTURE_2D, vpTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vpWidth, vpHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, vpRbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, vpWidth, vpHeight);

            glBindTexture(GL_TEXTURE_2D, postTextureA);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vpWidth, vpHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, postRboA);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, vpWidth, vpHeight);
        }
        ImGui::Image((void *)(intptr_t)vpTexture, ImVec2(vpWidth, vpHeight), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}