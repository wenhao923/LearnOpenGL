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
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_CULL_FACE);

	stbi_set_flip_vertically_on_load(false);

	// 模型资源
	float walls[] = {
		-1.0f, -1.0f, 0.0f, 0.0, 0.0,
		1.0f, -1.0f, 0.0f, 1.0, 0.0,
		1.0f, 1.0f, 0.0f, 1.0, 1.0,

		1.0f, 1.0f, 0.0f, 1.0, 1.0,
		-1.0f, 1.0f, 0.0f, 0.0, 1.0,
		-1.0f, -1.0f, 0.0f, 0.0, 0.0
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(walls), walls, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 贴图资源
	myTexture wallTexture, wallTextureNormal;
	wallTexture.LoadFromFile("../resources/brickwall.jpg", "texture_diffuse", false, true);
	wallTextureNormal.LoadFromFile("../resources/brickwall_normal.jpg", "texture_normal", false, false);

	// shader资源
	Shader wallShader("../resources/shaders/wall.vert", "../resources/shaders/wall.frag");
	wallShader.use();
	wallTexture.Bind(0);
	wallShader.setInt("diffuseTexture", 0);
	wallTextureNormal.Bind(1);
	wallShader.setInt("normalTexture", 1);

	// ubo 
	unsigned int uniformBlockIndexMod = glGetUniformBlockIndex(wallShader.ID, "Matrices");
	glUniformBlockBinding(wallShader.ID, uniformBlockIndexMod, 0);

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

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

		// 所有场景公用一个UBO(View + Projection)
		glm::mat4 view = myCamera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(myCamera.Zoom), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// 创建光源数组
		std::vector<std::shared_ptr<Light>> Lights;
		// 创建点光源
		attenuation att = { 1.0f, 0.09f, 0.032f };
		auto pointLight = std::make_shared<PointLight>(glm::vec3(sin(currentFrame), 0.0f, 0.0), att);
		Lights.push_back(pointLight);

		glBindVertexArray(VAO);
		wallShader.use();
		wallShader.setupShader(Lights, glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.0)) , view, projection);
		wallShader.setVec3("viewPos", myCamera.Position);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		window.swapBuffers();
		window.pollEvents();
	}

	// 清理资源

	window.terminate();
	return 0;
}