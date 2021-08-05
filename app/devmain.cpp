// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <glm/vec3.hpp>
// 
// #include <vector>
// #include <iostream>

#include "ogview/simviewer.hpp"
#include "ogview/voronoi_viewable.hpp"
using namespace std;
using namespace ogview;


int main(int argc, char** argv)
{
	VoronoiViewable vv;
	SimulationViewer sv(vv);
	sv.render();
	return 0;
}
