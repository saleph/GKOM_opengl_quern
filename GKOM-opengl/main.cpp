#include "main.h"

std::atomic<unsigned long> fps;
std::atomic<double> frameTimes;


float vertices1[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned indices1[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

float vertices2[] = {
	0.0f, 0.3f, 0.0f,  // top  
	-0.3f, 0.0f, 0.0f,  // bottom left
	0.3f, 0.0f, 0.0f  // bottom right 
};

unsigned indices2[] = {  // note that we start from 0!
	0, 1, 2   // first triangle
};

int main()
{
	initOpengl();
	GLFWwindow* window = createWindow();
	loadLibraries();

	// line/fill setting
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	unsigned VAO1 = getVao(1);
	unsigned VAO2 = getVao(2);
	unsigned texture = loadMinmapTexture("Resources\\wood.jpg");

	Shader shader1("vertexShader1.vert", "fragmentShader1.frag");
	Shader shader2("vertexShader2.vert", "fragmentShader2.frag");

	std::async(std::launch::async, currentFpsShow, window);

	// RENDER LOOP
	while (!glfwWindowShouldClose(window)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		Timer t;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(shader1, VAO1, texture);
		render(shader2, VAO2, -1);

		glfwSwapBuffers(window);
		glfwPollEvents();
		++fps;
		frameTimes = frameTimes + t.elapsed();
	}

	glfwTerminate();
	return 0;
}

void initOpengl() {
	if (glfwInit() != GL_TRUE) {
		std::cout << "GLFW initialization failed" << std::endl;
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	std::cout << ">> OpenGL initialized" << std::endl;
}

GLFWwindow *createWindow() {
	GLFWwindow* window = glfwCreateWindow(800, 600, "Tomasz Galecki OPENGL project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	// set resize callback
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	std::cout << ">> Window created" << std::endl;

	return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// resize callback
	glViewport(0, 0, width, height);
}

void loadLibraries() {
	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	std::cout << ">> GLAD initialized" << std::endl;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void currentFpsShow(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "FPS: " << fps << "\t Frame time: " << std::setprecision(2) << frameTimes * 1000.0 / fps << " ms" << std::endl;
		fps = 0;
		frameTimes = 0;
	}
}

void render(const Shader &shaderProgram, unsigned VAO, int texture) {
	float timeValue = glfwGetTime();
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(greenValue*360), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.8, 0.8, 0.5));

	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view;
	// note that we�re translating the scene in the reverse direction of where we want to move
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	float screenWidth = 800.0;
	float screenHeight = 600.0;
	projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

	shaderProgram.setMat4Uniform("model", model);
	shaderProgram.setMat4Uniform("view", view);
	shaderProgram.setMat4Uniform("projection", projection);
	shaderProgram.setMat4Uniform("transform", trans);
	shaderProgram.setFloatUniform("uniColor", greenValue);
	shaderProgram.use();

	if (texture != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	glBindVertexArray(0);
}

unsigned getVao(unsigned vaoNo) {
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use

	// same for 1 and 2
	unsigned vertSize = vaoNo == 1 ? sizeof(vertices1) : sizeof(vertices2);
	unsigned elementsSize = vaoNo == 1 ? sizeof(indices1) : sizeof(indices2);

	unsigned VBO = verticesPrepare(vaoNo == 1 ? vertices1 : vertices2, vertSize);
	unsigned EBO = elementsPrepare(vaoNo == 1 ? indices1 : indices2, elementsSize);
	// 3. then set our vertex attributes pointers
	if (vaoNo == 1) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	} else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	return VAO;
}

unsigned verticesPrepare(float *vertices, unsigned size) {
	unsigned VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	return VBO;
}

unsigned elementsPrepare(unsigned *indices, unsigned size) {
	unsigned EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	return EBO;
}

unsigned loadMinmapTexture(const char* fname) {
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr) {
		std::cout << "ERROR::TEXTURE::CAN'T_LOAD: " << fname << std::endl;
		exit(-1);
	}

	unsigned texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}