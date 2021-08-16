#include "fsl3d/utils.cpp"
#include "fsl3d/naive.cpp"
#include "ogview/simviewer.hpp"
#include "ogview/voronoi_viewable.hpp"

int main(int argc, char** argv)
{
	auto site_list = fsl3d::utils::read_site_list();
	auto voronoi = fsl3d::generate_naive_voronoi(site_list);
	ogview::VoronoiViewable vv(voronoi);
	ogview::SimulationViewer sv(vv);
	sv.render();
	fsl3d::utils::write_voronoi(voronoi);
	return 0;
}
