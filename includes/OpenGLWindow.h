// OpenGLWindow.h
#pragma once

#include "glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "camera.h"

class OpenGLWindow {
public:
    OpenGLWindow(int width, int height, const char* title) 
        : screenWidth(width), screenHeight(height), windowTitle(title),
        lastX(width / 2.0f), lastY(height / 2.0f), firstMouse(true), camera(nullptr) {}

    bool initialize() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowUserPointer(window, this); // 设置用户指针
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, staticCursorPosCallback);
        glfwSetScrollCallback(window, staticScrollCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glEnable(GL_DEPTH_TEST);
        return true;
    }

    void setCursorPosCallback(GLFWcursorposfun callback) {
        glfwSetCursorPosCallback(window, callback);
    }

    void setScrollCallback(GLFWscrollfun callback) {
        glfwSetScrollCallback(window, callback);
    }

    void processInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->ProcessKeyboard(RIGHT, deltaTime);
        // Change parallax height scale
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            height_scale -= 0.001;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            height_scale += 0.001;

    }

    bool shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void swapBuffers() {
        glfwSwapBuffers(window);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    void terminate() {
        glfwTerminate();
    }

    GLFWwindow* getWindow() {
        return window;
    }

    void setCamera(Camera* cam) { camera = cam; }

    void setDeltaTime(float delta) { deltaTime = delta; }

	float getFloatHeight() const { return height_scale; }

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    static void staticCursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
        OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(glfwWindow));
        if (instance && instance->camera) {
            float x = static_cast<float>(xpos);
            float y = static_cast<float>(ypos);
            if (instance->firstMouse) {
                instance->lastX = x;
                instance->lastY = y;
                instance->firstMouse = false;
            }
            float xoffset = x - instance->lastX;
            float yoffset = instance->lastY - y; // reversed
            instance->lastX = x;
            instance->lastY = y;
            instance->camera->ProcessMouseMovement(xoffset, yoffset);
        }
    }

    static void staticScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
        OpenGLWindow* instance = static_cast<OpenGLWindow*>(glfwGetWindowUserPointer(glfwWindow));
        if (instance && instance->camera) {
            instance->camera->ProcessMouseScroll(static_cast<float>(yoffset));
        }
    }

    int screenWidth, screenHeight;
    const char* windowTitle;
    GLFWwindow* window;
    float lastX, lastY;
    bool firstMouse;
    Camera* camera; // 持有Camera指针
    float deltaTime = 0.0f; // 新增：帧时间差
	float height_scale = 0.1;
};
