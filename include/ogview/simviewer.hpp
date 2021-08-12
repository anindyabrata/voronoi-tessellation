#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <array>
#include <iostream>
#include "ogutils.cpp"
#include "linalg.h"
#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	enum FaceGroup {completed, incomplete, beachline};
	class SimulationViewer{
		public:
		SimulationViewer(VoronoiViewable& _vv):vv(_vv){
			auto dverts = vv.getDynamicVertices();
			for(int i = 0; i < dverts.size(); i += 3) std::cout << dverts[i] << " " << dverts[i+1] << " " << dverts[i+2] << std::endl;
			auto dfaces = vv.getCompletedCellTris();
			for(int i = 0; i < dfaces.size(); ++i) std::cout << dfaces[i] << " "; std::cout << std::endl;
			init();
		}
		~SimulationViewer(){
			glfwTerminate();
		}
		void render(){
			rend();
		}
		private:
		VoronoiViewable vv;
		GLFWwindow* window = NULL;
		bool wireframe_mode = false;
		GLuint cell_tri_prog, cell_edge_prog;
		GLuint VAO, vertex_buffer;
		linalg::aliases::float4x4 transform = linalg::identity;
		void initData()
		{
			const char* vert_shader = GLSL
				(
				 450 core,
				 layout(location = 0) in vec3 position;
				 uniform mat4 mulmat;
				 void main()
				 {
				 gl_Position = mulmat * vec4(position, 1.0);
				 }
				);

			const char* cell_tri_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.6, 1.0, 1.0, 0.2);
				 }
				);

			const char* cell_edge_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.3, 0.3, 0.9, 0.9);
				 }
				);

			cell_tri_prog = LoadProgram(vert_shader, NULL, cell_tri_frag_shader);
			cell_edge_prog = LoadProgram(vert_shader, NULL, cell_edge_frag_shader);

			float data[] =
			{
				0.0f,0.8f,0,
				-0.8f, 0.0f,0,
				0.8f,0.0f,0
			};
			unsigned int indexes[] =
			{
				0,1,2
			};

			// dynamic VAO setup
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
			//glGenBuffers(1, &dindex_buffer);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dindex_buffer);
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindVertexArray(0);
		}

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
			glDepthFunc(GL_LEQUAL);

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
		void useProg(GLuint prog){
				glUseProgram(prog);

				// translate + rotate + scale matrix
				unsigned int mulmatid = glGetUniformLocation(prog, "mulmat");
				float tr[4*4]; for(int r = 4; r--;) for(int c = 4; c--; ) tr[r * 4 + c] = transform[r][c];
				glUniformMatrix4fv(mulmatid, 1, GL_FALSE, tr);
		}
		void rend(){
			while(!glfwWindowShouldClose(window))
			{
				glClearColor( 0.1, 0.2, 0.12, 1.0 );
				glClearDepth(1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glfwPollEvents();
				handleKeyboardEvents();

				// Dynamic draw
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
				auto vertices = vv.getDynamicVertices();
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
				// glBufferSubData?
				auto dfaces = vv.getCompletedCellTris();

				// update dynamic vertices
				// load faces (indices, face groups)
				std::vector<std::array<unsigned int, 3>> faces;
				for(int i = 0; i < dfaces.size(); i += 3) faces.push_back({dfaces[i], dfaces[i+1], dfaces[i+2]});
				// sort faces
				linalg::aliases::float4 cpos = getTransformedCameraPos();
				std::vector<float> dist;
				for(auto face: faces){
					float d = 0;
					for(int i = 0; i < 3; ++i) {
						int vi = face[i];
						float vf, dd = 0;
						for(int j = 0; j < 3; ++j){
							vf = vertices[vi * 3 + j] - cpos[j];
							vf *= vf;
							dd += vf;
						}
						if(dd > d) d = dd;
					}
					dist.push_back(d);
				}
				std::vector<int> face_order; for(int i = 0; i < faces.size(); ++i) face_order.push_back(i);
				auto cmp = [dist](const int a, const int b) -> bool { return dist[a] > dist[b]; };
				std::sort(face_order.begin(), face_order.end(), cmp);
				// show faces (if(face_group == 2) 
				for(int fi: face_order){
					useProg(cell_tri_prog);
					if(!wireframe_mode) glDrawElements(GL_TRIANGLES, 1 * 3, GL_UNSIGNED_INT, &faces[fi]);
					useProg(cell_edge_prog);
					glDrawElements(GL_LINES, 1 * 3, GL_UNSIGNED_INT, &faces[fi]);
				}
				glBindVertexArray(0);

				glFlush();
				glfwSwapBuffers(window);
			}

		}
	};
}
