// myTexture.h
#pragma once

#include <string>
#include <stdexcept>

#include "glad.h"
#include "stb_image.h"

class myTexture {
public:
    // 构造函数：生成纹理对象
    myTexture() {
        glGenTextures(1, &ID);
    }

    // 析构函数：释放纹理资源
    ~myTexture() {
        if (ID != 0) {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }

    // 从文件加载纹理（支持自动通道检测）
    void LoadFromFile(const std::string& path, bool flipVertically = true) {
        stbi_set_flip_vertically_on_load(flipVertically);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        GLenum internalFormat;
        GLenum dataFormat;
        if (nrChannels == 1) {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrChannels == 3) {
            internalFormat = GL_SRGB;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 4) {
            internalFormat = GL_SRGB_ALPHA;
            dataFormat = GL_RGBA;
        }
        else {
            stbi_image_free(data);
            throw std::runtime_error("Unsupported texture format: " + std::to_string(nrChannels) + " channels");
        }

        Bind();
        // 设置默认纹理参数
        SetWrap(GL_REPEAT);
        SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    // 绑定到指定纹理单元
    void Bind(GLuint unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    // 设置环绕方式
    void SetWrap(GLint wrapS, GLint wrapT = GL_REPEAT) {
        Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    }

    // 设置过滤方式
    void SetFilter(GLint minFilter, GLint magFilter = GL_LINEAR) {
        Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    // 获取纹理ID
    GLuint GetID() const { return ID; }

private:
    GLuint ID = 0; // 纹理ID
};