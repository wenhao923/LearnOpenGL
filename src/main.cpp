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

	glEnable(GL_DEPTH_TEST);
	stbi_set_flip_vertically_on_load(true);

	myTexture planeMap;
	try {
		planeMap.LoadFromFile("../resources/marble.jpg", "texture_diffuse");
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);

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
	planeTexture.id = planeMap.GetID();
	planeTexture.type = planeMap.GetType();
	auto veticesVec = convertToVector(planeVertices, 5 * 6, 5);
	Mesh plane(veticesVec, generateIndices(veticesVec), {planeTexture});
	Shader planeShader("../resources/shaders/shader.vert", "../resources/shaders/objectshader.frag");

	stbi_set_flip_vertically_on_load(true);
	// 准备资源：顶点数据、texture
	Model ourModel("../resources/backpack/backpack.obj");
	// 创建模型对应的Shader
	Shader backPackShader("../resources/shaders/backPackshader.vert", "../resources/shaders/backPackshader.frag");

	// 创建光源数组
	std::vector<std::shared_ptr<Light>> Lights;
	// 创建点光源
	attenuation att = { 1.0f, 0.09f, 0.032f };
	auto pointLight = std::make_shared<PointLight>(glm::vec3(1.2f, 1.0f, 2.0f), att);
	Lights.push_back(pointLight);

	while (!window.shouldClose()) {
		static float lastFrame = 0.0f;
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		window.setDeltaTime(deltaTime);
		window.processInput();

		glClearColor(0.01f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 所有场景公用一个view / Projection
		glm::mat4 view = myCamera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(myCamera.Zoom), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		
		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

		backPackShader.use();
		backPackShader.setupShader(Lights, model, view, projection);
		backPackShader.setVec3("viewPos", myCamera.Position);

		ourModel.Draw(backPackShader);

		planeShader.use();
		planeShader.setupShader(Lights, model, view, projection);
		planeShader.setVec3("viewPos", myCamera.Position);

		plane.Draw(planeShader);

		window.swapBuffers();
		window.pollEvents();
	}

	// 清理资源
	glDeleteProgram(backPackShader.ID);

	window.terminate();
	return 0;
}