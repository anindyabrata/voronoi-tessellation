#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include "ogutils.cpp"

class SimulationViewer{
	public:
	SimulationViewer(){
		init();
	}
	~SimulationViewer(){
		term();
	}
	void render(){
		rend();
	}

	GLuint program;
	GLuint VAO;
	void initData()
	{
		const char* vert = GLSL
			(
			 450 core,
			 layout(location = 0) in vec3 position;
			 void main()
			 {
			 gl_Position = vec4(position, 1.0);
			 }
			);

		const char* frag = GLSL
			(
			 450 core,
			 out vec4 FragColor;
			 void main()
			 {
			 FragColor = vec4(0.6, 1.0, 1.0, 1.0);
			 }
			);

		program = LoadProgram(vert, NULL, frag);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint vertex_buffer = 0;
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		float data[] =
		{
			0.0f,0.8f,0,
			-0.8f, 0.0f,0,
			0.8f,0.0f,0
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

		GLuint index_buffer = 0;
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		unsigned int indexes[] =
		{
			0,1,2
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}

	GLFWwindow* window = NULL;
	void init()
	{
		if(!glfwInit())
			return;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(640, 480, "Simulation window", NULL, NULL);
		if(!window)
		{
			std::cerr << "Error creating window" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window);

		//printGLVersionInfo();

		glewExperimental = true;
		GLenum err = glewInit();
		if(err!= GLEW_OK)
		{
			std::cerr << "Glew init failed!" << std::endl;
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		}
		glEnable(GL_DEPTH_TEST);

		initData();
	}
	void rend(){
		while(!glfwWindowShouldClose(window))
		{
			glClearColor( 0.1, 0.2, 0.12, 1.0 );
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

	}
	void term(){
		glfwTerminate();
		return;
	}
};
