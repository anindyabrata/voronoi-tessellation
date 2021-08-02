#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <vector>
#include <iostream>

#include "ogview/simviewer.hpp"
using namespace std;


int main( int argc, char** argv )
{
	SimulationViewer v;
	v.render();
	return 0;
}
