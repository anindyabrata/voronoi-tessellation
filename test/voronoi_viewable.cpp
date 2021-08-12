#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	VoronoiViewable::VoronoiViewable(){}
	VoronoiViewable::VoronoiViewable(const fsl3d::Voronoi& voronoi){}
	void VoronoiViewable::setProgress(double progress){
	}
	std::vector<float> VoronoiViewable::getStaticVertices(){
		std::vector<float> ret = {
			0.5, 0.5, 0.5,
			-0.5, 0.5, 0.5,
			0.5, -0.5, 0.5,
			-0.5, -0.5, 0.5,
			0.5, 0.5, -0.5,
			-0.5, 0.5, -0.5,
			0.5, -0.5, -0.5,
			-0.5, -0.5, -0.5,
		};
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getSiteIndices(){
		std::vector<unsigned int> ret = {
			0, 4
		};
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getCompletedCellTris(){
		std::vector<unsigned int> ret = {
			0, 1, 3,
			0, 3, 2,
			//
			4, 5, 7,
			4, 7, 6,
		};
		return ret;
	}
	std::vector<float> VoronoiViewable::getDynamicVertices(){
		std::vector<float> ret;
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getBeachlineTris(){
		std::vector<unsigned int> ret;
		return ret;
	}
}
