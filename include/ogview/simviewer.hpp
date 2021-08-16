#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <vector>
#include <array>
#include <iostream>
#include "ogutils.cpp"
#include "linalg.h"
#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	enum class FaceGroup {cell, sweepline, beachline};
	class SimulationViewer{
		public:
		SimulationViewer(VoronoiViewable& _vv):vv(_vv){
			// vv.setProgress(15);
			// auto &dverts = vv.getVertices();
			// for(int i = 0; i < dverts.size(); i += 3) std::cout << dverts[i] << " " << dverts[i+1] << " " << dverts[i+2] << std::endl;
			// auto &dfaces = vv.getBeachline();
			// assert(0 < dfaces.size());
			// for(int i = 0; i < dfaces.size(); ++i) { for(int j = 0; j < dfaces[i].size(); ++j) std::cout << dfaces[i][j] << " "; std::cout << std::endl; }
			// auto &sface = vv.getSweepline()[0];
			// for(int i = 0; i < sface.size(); ++i) std::cout << sface[i] << " "; std::cout << std::endl;
			// std::cout << "#Verts = " << dverts.size() << std::endl;
			init();
		}
		~SimulationViewer(){
			glfwTerminate();
		}
		void render(){
			rend();
		}
		private:
		VoronoiViewable& vv;
		GLFWwindow* window = NULL;
		bool wireframe_mode = false, playing = false;
		GLuint site_prog, cell_prog, edge_prog, sweep_prog, beach_prog;
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
				 gl_PointSize = 5.0;
				 }
				);

			const char* site_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.8, 0.6, 0.6, 0.4);
				 }
				);

			const char* cell_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.6, 1.0, 1.0, 0.03);
				 }
				);
			const char* edge_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.2, 0.7, 0.9);
				 }
				);

			const char* sweep_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.2, 0.8, 0.1);
				 }
				);

			const char* beach_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.8, 0.4, 0.1);
				 }
				);

			site_prog   = LoadProgram(vert_shader, NULL, site_frag_shader);
			cell_prog   = LoadProgram(vert_shader, NULL, cell_frag_shader);
			edge_prog   = LoadProgram(vert_shader, NULL, edge_frag_shader);
			sweep_prog  = LoadProgram(vert_shader, NULL, sweep_frag_shader);
			beach_prog  = LoadProgram(vert_shader, NULL, beach_frag_shader);


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

			// Dynamic VAO setup
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
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

			window = glfwCreateWindow(800, 800, "Simulation window", NULL, NULL);
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
			// glEnable(GL_DEPTH_TEST);
			// glEnable(GL_MULTISAMPLE_ARB);
			// glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
			// glDepthFunc(GL_LEQUAL);

			// Used https://andersriggelsen.dk/glblendfunc.php to find a blending mode that works
			glEnable(GL_BLEND);
			// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendEquation(GL_FUNC_SUBTRACT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			glEnable(GL_PROGRAM_POINT_SIZE);

			initData();
		}
		float rtx = -0.7, rty = 0.9, rtz = -0.55;
		bool ppressed = false, _0pressed = false, _1pressed = false;
		bool wpressed = false;
		void handleKeyboardEvents(){
			// Press ESC to close window
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			// Handle progress
			if (!ppressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				playing = !playing;
			ppressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
			if (!_0pressed && glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
				playing = false, vv.setNoProgress();
			_0pressed = glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS;
			if (!_1pressed && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
				playing = false, vv.setFullProgress();
			_1pressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;

			// Handle wireframe mode
			if (!wpressed && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				wireframe_mode = !wireframe_mode;
			wpressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;

			// Handle rotation
			float rotation_speed = 0.002;
			if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				rtx -= rotation_speed;
			else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
				rtx += rotation_speed;
			if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				rty -= rotation_speed;
			else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				rty += rotation_speed;
			if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				rtz -= rotation_speed;
			else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
				rtz += rotation_speed;
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
				rtx = -0.7, rty = 0.9, rtz = -0.55;

			// Apply rotation
			transform = linalg::identity;
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({1, 0, 0}, rtx)), transform);
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({0, 1, 0}, rty)), transform);
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({0, 0, 1}, rtz)), transform);
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
		int tick = 0;
		void rend(){
			while(!glfwWindowShouldClose(window))
			{
				// glClearColor(0.1, 0.2, 0.12, 1.0);
				glClearColor(0, 0, 0, 0);
				glClearDepth(1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glfwPollEvents();
				handleKeyboardEvents();

				int playmask = 1 << 3 - 1;
				if(playing && !(++tick & playmask)) vv.increment();

				// Dynamic draw
				// Update dynamic vertices
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
				auto &vertices = vv.getVertices();
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
				// glBufferSubData?

				// load faces (indices, face groups)
				auto &siteverts  = vv.getSiteIndices();
				auto &cellfaces  = vv.getCells();
				auto &sweepfaces = vv.getSweepline();
				auto &beachfaces = vv.getBeachline();

				// Place into single container for z-sorting
				// std::vector<std::vector<unsigned int>> faces;
				// std::vector<FaceGroup> groups;
				// for(int i = 0; i <  cellfaces.size(); ++i) faces.push_back( cellfaces[i]), groups.push_back(FaceGroup::cell);
				// for(int i = 0; i < sweepfaces.size(); ++i) faces.push_back(sweepfaces[i]), groups.push_back(FaceGroup::sweepline);
				// for(int i = 0; i < beachfaces.size(); ++i) faces.push_back(beachfaces[i]), groups.push_back(FaceGroup::beachline);

				// sort faces
				// linalg::aliases::float4 cpos = getTransformedCameraPos();
				// std::vector<float> dist;
				// for(auto &face: faces){
				//	float d = 0;
				//	for(int i = 0; i < face.size(); ++i) {
				//		int vi = face[i];
				//		float vf, dd = 0;
				//		for(int j = 0; j < 3; ++j){
				//			vf = vertices[vi * 3 + j] - cpos[j];
				//			vf *= vf;
				//			dd += vf;
				//		}
				//		if(dd > d) d = dd;
				//	}
				//	dist.push_back(d);
				// }
				// std::vector<int> face_order; for(int i = 0; i < faces.size(); ++i) face_order.push_back(i);
				// auto cmp = [dist](const int &a, const int &b) -> bool { return dist[a] > dist[b]; };
				// std::sort(face_order.begin(), face_order.end(), cmp); // ASAN error

				// Render sites
				useProg(site_prog);
				glDrawElements(GL_POINTS, siteverts.size(), GL_UNSIGNED_INT, siteverts.data());

				for(auto &face: cellfaces){
					useProg(cell_prog);
					if(!wireframe_mode) glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
					useProg(edge_prog);
					glDrawElements(GL_LINE_LOOP, face.size(), GL_UNSIGNED_INT, face.data());
				}
				for(auto &face: sweepfaces){
					useProg(sweep_prog);
					glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
				}
				for(auto &face: beachfaces){
					useProg(beach_prog);
					glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
				}

				// Render faces and edges
				// for(int fi: face_order){
				//	if(groups[fi] == FaceGroup::cell){
				//		useProg(cell_prog);
				//		if(!wireframe_mode) glDrawElements(GL_TRIANGLE_STRIP, faces[fi].size(), GL_UNSIGNED_INT, faces[fi].data());
				//		useProg(edge_prog);
				//		glDrawElements(GL_LINE_LOOP, faces[fi].size(), GL_UNSIGNED_INT, faces[fi].data());
				//	}
				//	else if(groups[fi] == FaceGroup::sweepline){
				//		useProg(sweep_prog);
				//		glDrawElements(GL_TRIANGLE_STRIP, faces[fi].size(), GL_UNSIGNED_INT, faces[fi].data());
				//	}
				//	else{
				//		useProg(beach_prog);
				//		glDrawElements(GL_TRIANGLE_STRIP, faces[fi].size(), GL_UNSIGNED_INT, faces[fi].data());
				//	}
				// }
				glBindVertexArray(0);

				glFlush();
				glfwSwapBuffers(window);
			}

		}
	};
}
