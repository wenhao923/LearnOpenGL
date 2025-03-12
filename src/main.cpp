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

	stbi_set_flip_vertically_on_load(false);
	// 模型资源
	Model planet("../resources/planet/planet.obj");
	Shader planetShader("../resources/shaders/planet.vert", "../resources/shaders/planet.frag");	
	Model rock("../resources/rock/rock.obj");
	Shader asteroidShader("../resources/shaders/asteroid.vert", "../resources/shaders/asteroid.frag");

	// ubo 
	unsigned int uniformBlockIndexMod = glGetUniformBlockIndex(planetShader.ID, "Matrices");
	unsigned int uniformBlockIndexAst = glGetUniformBlockIndex(asteroidShader.ID, "Matrices");
	glUniformBlockBinding(planetShader.ID, uniformBlockIndexMod, 0);
	glUniformBlockBinding(asteroidShader.ID, uniformBlockIndexAst, 0);

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

		// Planet模型 draw
		planetShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		planetShader.setMat4("model", model);
		planet.Draw(planetShader);

		// Rock 模型 draw
		asteroidShader.use();
		unsigned int amount = 10000;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[amount];
		srand(glfwGetTime()); // 初始化随机种子    
		float radius = 80.0f;
		float offset = 20.0f;
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. 缩放：在 0.05 和 0.25f 之间缩放
			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. 添加到矩阵的数组中
			modelMatrices[i] = model;
		}

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
		{
			unsigned int VAO = rock.getMeshes()[i].getVAO();
			glBindVertexArray(VAO);
			// 顶点属性
			GLsizei vec4Size = sizeof(glm::vec4);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
		for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
		{
			asteroidShader.setInt("texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, rock.getTextures()[0].id);
			glBindVertexArray(rock.getMeshes()[i].getVAO());
			glDrawElementsInstanced(
				GL_TRIANGLES, rock.getMeshes()[i].indices.size(), GL_UNSIGNED_INT, 0, amount
			);
		}

		// instance draw
		//instanceShader.use();
		//glBindVertexArray(instanceVAO);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

		window.swapBuffers();
		window.pollEvents();
	}

	// 清理资源

	window.terminate();
	return 0;
}