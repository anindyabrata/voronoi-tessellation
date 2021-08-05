#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include "ogutils.cpp"
#include "linalg.h"
#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	class SimulationViewer{
		public:
		SimulationViewer(VoronoiViewable& _vv):vv(_vv){
			init();
		}
		~SimulationViewer(){
			term();
		}
		void render(){
			rend();
		}
		VoronoiViewable vv;
		GLuint program;
		GLuint VAO;
		linalg::aliases::float4x4 transform = linalg::identity;
		void initData()
		{
			const char* vert = GLSL
				(
				 450 core,
				 layout(location = 0) in vec3 position;
				 uniform mat4 mulmat;
				 void main()
				 {
				 gl_Position = mulmat * vec4(position, 1.0);
				 }
				);

			const char* frag = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.6, 1.0, 1.0, 0.7);
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
			auto static_vertices = vv.getStaticVertices();
			float *svs = &static_vertices[0];
			//glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, static_vertices.size() * sizeof(float), svs, GL_STATIC_DRAW);

			GLuint index_buffer = 0;
			glGenBuffers(1, &index_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
			unsigned int indexes[] =
			{
				0,1,2
			};
			auto cell_indices = vv.getCompletedCellTris();
			unsigned int *indices = &cell_indices[0];
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cell_indices.size() * sizeof(unsigned int), indices, GL_STATIC_DRAW);

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

			// https://andersriggelsen.dk/glblendfunc.php
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_SUBTRACT);

			initData();
		}
		void handleKeyboardEvents(){
			float rotation_speed = 0.001;
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				transform = linalg::mul(linalg::rotation_matrix<float>({1, 0, 0, rotation_speed}), transform);
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				transform = linalg::mul(linalg::rotation_matrix<float>({0, 1, 0, rotation_speed}), transform);
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				transform = linalg::mul(linalg::rotation_matrix<float>({0, 0, 1, rotation_speed}), transform);
		}
		linalg::aliases::float4 getTransformedCameraPos(){
			linalg::aliases::float4 pos = {0, 0, -1, 1};
			pos = linalg::mul(linalg::inverse(transform), pos);
			return pos;
		}
		void rend(){
			while(!glfwWindowShouldClose(window))
			{
				glClearColor( 0.1, 0.2, 0.12, 1.0 );
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				handleKeyboardEvents();

				glUseProgram(program);

				// translate + rotate + scale matrix
				unsigned int mulmatid = glGetUniformLocation(program, "mulmat");
				float tr[4*4]; for(int r = 4; r--;) for(int c = 4; c--; ) tr[r * 4 + c] = transform[r][c];
				glUniformMatrix4fv(mulmatid, 1, GL_FALSE, tr);

				// sort faces by distance from camera

				// draw faces on screen
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				glfwSwapBuffers(window);

				glfwPollEvents();
			}

		}
		void term(){
			glfwTerminate();
		}
	};
}
