#include "fsl3d/utils.cpp"
#include "fsl3d/fortunes.cpp"
#include "ogview/simviewer.hpp"
#include "ogview/voronoi_viewable.hpp"

int main(int argc, char** argv)
{
	// Read input and generate voronoi diagram
	auto site_list = fsl3d::utils::read_site_list();
	auto voronoi = fsl3d::generate_fortunes_voronoi(site_list);

	// Display simulation
	ogview::VoronoiViewable vv(voronoi);
	ogview::SimulationViewer sv(vv);
	sv.render();

	// Write cell descriptions to output
	fsl3d::utils::write_voronoi(voronoi);
	return 0;
}
