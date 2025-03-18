// myTexture.h
#pragma once

#include <string>
#include <stdexcept>

#include "glad.h"
#include "stb_image.h"

class myTexture {
public:
    // ���캯���������������
    myTexture() {
        glGenTextures(1, &ID);
    }

    // �����������ͷ�������Դ
    ~myTexture() {
        if (ID != 0) {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }

    // ���ļ���������֧���Զ�ͨ����⣩
    void LoadFromFile(const std::string& path, const std::string& type, bool flipVertically = true, bool gammaCorrection = false) {
        stbi_set_flip_vertically_on_load(flipVertically);
        this->type = type;

        size_t lastSlashPos = path.find_last_of("/");
        if (lastSlashPos != std::string::npos) {
            this->path = path.substr(lastSlashPos + 1);
        }
        
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
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 4) {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }
        else {
            stbi_image_free(data);
            throw std::runtime_error("Unsupported texture format: " + std::to_string(nrChannels) + " channels");
        }

        Bind(unit);
        // ����Ĭ���������
        SetWrap(GL_REPEAT);
        SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    // �󶨵�ָ������Ԫ
    void Bind(GLuint unit = 0) {
        this->unit = unit;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    // ���û��Ʒ�ʽ
    void SetWrap(GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT) {
        Bind(unit);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    }

    // ���ù��˷�ʽ
    void SetFilter(GLint minFilter, GLint magFilter = GL_LINEAR) {
        Bind(unit);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    // ��ȡ����ID
    GLuint GetID() const { return ID; }
    
    std::string GetType() const { return type; }

    std::string GetPath() const { return path; }

private:
    GLuint unit = 0; // ����Ԫ
    GLuint ID = 0; // ����ID
    std::string type;
    std::string path;
};