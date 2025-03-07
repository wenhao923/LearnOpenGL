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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	// Texture
	myTexture planeTex, cubeTex, grassTex;
	try {
		cubeTex.LoadFromFile("../resources/marble.jpg", "texture_diffuse");
		planeTex.LoadFromFile("../resources/metal.png", "texture_diffuse");
		grassTex.LoadFromFile("../resources/blending_transparent_window.png", "texture_diffuse", false);
		grassTex.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	// FBO
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// 生成纹理
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 将它附加到当前绑定的帧缓冲对象
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//unsigned int depthBuffer;
	//glGenTextures(1, &depthBuffer);
	//glBindTexture(GL_TEXTURE_2D, depthBuffer);
	//glTexImage2D(
	//	GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	//);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	// RBO
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 材质，可以通用
	Shader depethShader("../resources/shaders/shader.vert", "../resources/shaders/objectshader.frag");
	
	Shader fboShader("../resources/shaders/fboshader.vert", "../resources/shaders/fboshader.frag");

	// 地面
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	Texture planeTexture;
	planeTexture.id = planeTex.GetID();
	planeTexture.type = planeTex.GetType();
	planeTexture.path = planeTex.GetPath();
	auto verticesVec = convertToVector(planeVertices, 6 * 5, 5);
	Mesh plane(verticesVec, generateIndices(verticesVec), {planeTexture});

	// cube
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right       
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left

		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		 // Bottom face
		 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 // Top face
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};
	Texture cubeTexture;
	cubeTexture.id = cubeTex.GetID();
	cubeTexture.type = cubeTex.GetType();
	cubeTexture.path = cubeTex.GetPath();
	verticesVec = convertToVector(cubeVertices, 36 * 5, 5);
	Mesh cube(verticesVec, generateIndices(verticesVec), { cubeTexture });

	// grass
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	Texture grassTexture;
	grassTexture.id = grassTex.GetID();
	grassTexture.type = grassTex.GetType();
	grassTexture.path = grassTex.GetPath();
	verticesVec = convertToVector(transparentVertices, 6 * 5, 5);
	Mesh grass(verticesVec, generateIndices(verticesVec), { grassTexture });

	// Light
	std::vector<std::shared_ptr<Light>> Lights;

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	while (!window.shouldClose()) {
		static float lastFrame = 0.0f;
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		window.setDeltaTime(deltaTime);
		window.processInput();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 所有场景公用一个view / Projection
		glm::mat4 view = myCamera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(myCamera.Zoom), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		// render the loaded model
		depethShader.use();
		depethShader.setupShader(Lights, model, view, projection);
		depethShader.setVec3("viewPos", myCamera.Position);

		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		depethShader.setMat4("model", model);
		cube.Draw(depethShader);

		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		depethShader.setMat4("model", model);
		cube.Draw(depethShader);	

		model = glm::mat4(1.0f);
		depethShader.setMat4("model", model);
		plane.Draw(depethShader);

		vector<glm::vec3> position
		{
			glm::vec3(-1.5f, 0.0f, -0.48f),
			glm::vec3(1.5f, 0.0f, 0.51f),
			glm::vec3(0.6f, 0.0f, 0.7f),
			glm::vec3(-0.3f, 0.0f, -2.3f),
			glm::vec3(0.5f, 0.0f, -0.6f)
		};

		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < position.size(); i++)
		{
			float distance = glm::length(myCamera.Position.z - position[i].z);
			sorted[distance] = position[i];
		}

		glDepthMask(GL_FALSE);
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			depethShader.setMat4("model", model);
			grass.Draw(depethShader);
		}

		// pass2
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		fboShader.use();
		fboShader.setInt("material.texture_diffuse1", 0);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		window.swapBuffers();
		window.pollEvents();
	}

	// 清理资源

	window.terminate();
	return 0;
}