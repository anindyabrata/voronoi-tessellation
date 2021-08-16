#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <vector>
#include "ogutils.cpp"
#include "linalg.h"
#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	enum class FaceGroup {cell, sweepline, beachline};

	// A class that takes viewable voronoi data and displays a simulation
	// using an openGL window. Makes sure openGL resources are properly
	// deallocated using glfwTerminate function
	class SimulationViewer{
	public:

		SimulationViewer(VoronoiViewable& _vv):vv(_vv){
			init();
		}

		~SimulationViewer(){
			glfwTerminate();
		}

		// This function blocks current thread and runs openGL main loop
		// The function will return once the simulation window is closed
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

		// Initialize shaders and buffers required for openGL rendering
		void initData()
		{
			// Vertex shader
			// Used by all rendered elements
			// mulmat is the transformation matrix used for rotation
			// gl_PointSize is the size of points representing sites
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

			// Fragment shader for site vertices
			const char* site_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.8, 0.6, 0.6, 0.4);
				 }
				);

			// Fragment shader for cell faces
			const char* cell_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.6, 1.0, 1.0, 0.03);
				 }
				);

			// Fragment shader for cell edges
			const char* edge_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.2, 0.7, 0.9);
				 }
				);

			// Fragment shader for sweep line faces
			const char* sweep_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.2, 0.8, 0.1);
				 }
				);

			// Fragment shader for beachline faces
			const char* beach_frag_shader = GLSL
				(
				 450 core,
				 out vec4 FragColor;
				 void main()
				 {
				 FragColor = vec4(0.2, 0.8, 0.4, 0.1);
				 }
				);

			// Install all shader programs
			// Each type of object is given a different shader
			// This is done to save memory
			// The alternative is to provide color data for each point
			site_prog   = LoadProgram(vert_shader, NULL, site_frag_shader);
			cell_prog   = LoadProgram(vert_shader, NULL, cell_frag_shader);
			edge_prog   = LoadProgram(vert_shader, NULL, edge_frag_shader);
			sweep_prog  = LoadProgram(vert_shader, NULL, sweep_frag_shader);
			beach_prog  = LoadProgram(vert_shader, NULL, beach_frag_shader);

			// Sample vertex data used for initializing buffer
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

			// VAO setup
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindVertexArray(0);
		}

		// Initialiaze all openGL entities required for rendering
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

			// Alpha blending is required for rendering transparent/semi-transparent faces
			// Used https://andersriggelsen.dk/glblendfunc.php to find a blending mode that works
			glEnable(GL_BLEND);
			// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			// glBlendEquation(GL_FUNC_SUBTRACT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			// Enable changing size of point rendering in shader
			glEnable(GL_PROGRAM_POINT_SIZE);

			initData();
		}

		// Keyboard input and rotation parameters
		float rtx = -0.7, rty = 0.9, rtz = -0.55;
		bool ppressed = false, _0pressed = false, _1pressed = false;
		bool wpressed = false;

		// Keyboard event polling function
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
			// Uses external linalg library for matrix operations
			transform = linalg::identity;
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({1, 0, 0}, rtx)), transform);
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({0, 1, 0}, rty)), transform);
			transform = linalg::mul(linalg::rotation_matrix<float>(linalg::rotation_quat({0, 0, 1}, rtz)), transform);
		}

		// Everytime shader program is changed, transformation matrix must be provided
		void useProg(GLuint prog){
				glUseProgram(prog);

				// translate + rotate + scale matrix
				unsigned int mulmatid = glGetUniformLocation(prog, "mulmat");
				float tr[4*4]; for(int r = 4; r--;) for(int c = 4; c--; ) tr[r * 4 + c] = transform[r][c];
				glUniformMatrix4fv(mulmatid, 1, GL_FALSE, tr);
		}

		// Used to limit frame rate
		int tick = 0;

		// Contains main loop of Simulation
		// Function returns when simulation window is closed
		void rend(){
			while(!glfwWindowShouldClose(window))
			{
				// Background color is pure black
				glClearColor(0, 0, 0, 0);
				glClearDepth(1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glfwPollEvents();
				handleKeyboardEvents();

				// Used to limit the speed at which simulation is progressed
				// (without using modulus) since changing progress is computationally expensive
				int playmask = 1 << 3 - 1;
				if(playing && !(++tick & playmask)) vv.increment();

				// Update dynamic vertices
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
				auto &vertices = vv.getVertices();
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);

				// Load faces, indices from viewable object
				auto &siteverts  = vv.getSiteIndices();
				auto &cellfaces  = vv.getCells();
				auto &sweepfaces = vv.getSweepline();
				auto &beachfaces = vv.getBeachline();

				// Render site vertices
				useProg(site_prog);
				glDrawElements(GL_POINTS, siteverts.size(), GL_UNSIGNED_INT, siteverts.data());

				// Render cell faces
				for(auto &face: cellfaces){
					useProg(cell_prog);
					if(!wireframe_mode) glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
					useProg(edge_prog);
					glDrawElements(GL_LINE_LOOP, face.size(), GL_UNSIGNED_INT, face.data());
				}

				// Render sweepline(or sweep plane in this case) faces
				for(auto &face: sweepfaces){
					useProg(sweep_prog);
					glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
				}

				// Render beachline(plane in this case) faces
				for(auto &face: beachfaces){
					useProg(beach_prog);
					glDrawElements(GL_TRIANGLE_FAN, face.size(), GL_UNSIGNED_INT, face.data());
				}

				// Unbind from VAO after use
				glBindVertexArray(0);

				// Indicate to openGL that the frame is ready to be rendered
				glFlush();
				glfwSwapBuffers(window);
			}

		}
	};
}
