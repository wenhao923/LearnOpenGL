#include "OpenGLWindow.h"

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "stb_image.h"
#include "Texture.h"
#include "utils.h"

#include "PointLight.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// settings
const int screenWidth = 800;
const int screenHeight = 600;

int main() {
	OpenGLWindow window(screenWidth, screenHeight, "LearnOpenGL");
	if (!window.initialize())
		return -1;

	// 窗口和相机
	Camera myCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	window.setCamera(&myCamera);
	glViewport(0, 0, screenWidth, screenHeight);

	// 配置选项
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	// 显示法线
	Shader displayNormalShader("../resources/shaders/displayNormal.vert", "../resources/shaders/displayNormal.frag", "../resources/shaders/displayNormal.geom");

	stbi_set_flip_vertically_on_load(false);
	// 模型资源
	Model nanosuit("../resources/nanosuit_reflection/nanosuit.obj");
	Shader backPackShader("../resources/shaders/backPackshader.vert", "../resources/shaders/backPackshader.frag");
	
	// 天空盒资源
	vector<std::string> faces
	{
		"../resources/skybox/right.jpg",
		"../resources/skybox/left.jpg",
		"../resources/skybox/top.jpg",
		"../resources/skybox/bottom.jpg",
		"../resources/skybox/front.jpg",
		"../resources/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	Shader skyboxShader("../resources/shaders/skyboxshader.vert", "../resources/shaders/skyboxshader.frag");

	float skyvertices[] = {
		// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyvertices), &skyvertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// ubo 
	unsigned int uniformBlockIndexMod = glGetUniformBlockIndex(backPackShader.ID, "Matrices");

	glUniformBlockBinding(backPackShader.ID, uniformBlockIndexMod, 0);

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Light
	std::vector<std::shared_ptr<Light>> Lights;
	attenuation att = { 1.0f, 0.09f, 0.032f };
	auto pointLight = std::make_shared<PointLight>(glm::vec3(2.0f, 3.0f, 2.0f), att);
	Lights.push_back(pointLight);

	// GPU instance
	float quadVertices[] = {
		// 位置          // 颜色
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f, 
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
	unsigned int instanceVAO, instanceVBO;
	glGenVertexArrays(1, &instanceVAO);
	glGenBuffers(1, &instanceVBO);
	glBindVertexArray(instanceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	// glBindVertexArray(0);

	// instance Array
	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}
	unsigned int instancedArrayVBO;
	glGenBuffers(1, &instancedArrayVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instancedArrayVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);
	Shader instanceShader("../resources/shaders/instance.vert", "../resources/shaders/instance.frag");

	while (!window.shouldClose()) {
		static float lastFrame = 0.0f;
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		window.setDeltaTime(deltaTime);
		window.processInput();

		// 主屏幕pass
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 所有场景公用一个view / Projection
		glm::mat4 view = myCamera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(myCamera.Zoom), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		glm::mat4 model = glm::mat4(1.0f);

		// 模型 draw
		backPackShader.setMat4("model", glm::translate(glm::scale(model, glm::vec3(0.1f)), glm::vec3(0.0, -5.0, 0.0)));
		backPackShader.setVec3("viewPos", myCamera.Position);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		backPackShader.setInt("skybox", 3);
		//backPackShader.setFloat("time", currentFrame);
		nanosuit.Draw(backPackShader);
		// 法线 draw
		displayNormalShader.setMat4("model", glm::translate(glm::scale(model, glm::vec3(0.1f)), glm::vec3(0.0, -5.0, 0.0)));
		displayNormalShader.setMat4("view", view);
		displayNormalShader.setMat4("projection", projection);
		nanosuit.Draw(displayNormalShader);

		// instance draw
		instanceShader.use();
		glBindVertexArray(instanceVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

		// 天空盒子 draw
		glDepthFunc(GL_LEQUAL);
		glm::mat4 SkyView = glm::mat4(glm::mat3(view));
		skyboxShader.setupShader({}, model, SkyView, projection);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);

		window.swapBuffers();
		window.pollEvents();
	}

	// 清理资源

	window.terminate();
	return 0;
}