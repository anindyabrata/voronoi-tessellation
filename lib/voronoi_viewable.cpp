#include "ogview/voronoi_viewable.hpp"

namespace ogview{
	VoronoiViewable::VoronoiViewable(){
	}
	VoronoiViewable::VoronoiViewable(const fsl3d::Voronoi &v): vor(v){
	}
	void VoronoiViewable::setProgress(double prog){
		progress = prog;
	}
	void VoronoiViewable::setBoundary(float bound){
		boundary = bound;
	}
	std::vector<float> VoronoiViewable::getStaticVertices(){
		auto original = vor.get_site_vertices();
		std::vector<float> ret;
		for(int i = 0; i < original.size(); ++i) for(int j = 0; j < 3; ++j) ret.push_back((float)original[i][j]);
		scale_floats(ret);
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getSiteIndices(){
		std::vector<unsigned int> ret;
		for(int i = 0; i < vor.get_site_vertices().size(); i += 3) ret.push_back(i);
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getCompletedCellTris(){
		std::vector<unsigned int> ret;
		auto faces = vor.get_unique_faces();
		for(auto face: faces){
			std::vector<unsigned int> f;
			for(int i = 0; i < face.size(); ++i) f.push_back(face[i] * 3);
			triangulate(ret, f);
		}
		return ret;
	}
	std::vector<float> VoronoiViewable::getDynamicVertices(){
		auto original = vor.get_cell_vertices();
		std::vector<float> ret;
		for(int i = 0; i < original.size(); ++i) for(int j = 0; j < 3; ++j) ret.push_back((float)original[i][j]);
		scale_floats(ret);
		return ret;
	}
	std::vector<unsigned int> VoronoiViewable::getBeachlineTris(){
		std::vector<unsigned int> ret;
		return ret;
	}
	void VoronoiViewable::triangulate(std::vector<unsigned int> &ret, std::vector<unsigned int> face){
		for(int i = 2; i < face.size(); ++i){
			ret.push_back(face[0]);
			ret.push_back(face[i - 1]);
			ret.push_back(face[i]);
		}
	}
	void VoronoiViewable::scale_floats(std::vector<float> &vec){
		for(int i = 0; i < vec.size(); ++i) vec[i] = ((vec[i] - vor.bb_min) / (vor.bb_max - vor.bb_min)) * (2 * boundary) - boundary;
	}
}
