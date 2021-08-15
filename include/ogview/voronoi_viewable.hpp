#ifndef OGVIEW_VVIEWABLE
#define OGVIEW_VVIEWABLE


#include <cmath>
#include <vector>
#include "fsl3d/voronoi.hpp"

namespace ogview{
		const size_t PSTEPS = 30; // Steps to full progress
		const size_t GSTEPS = 20; // Density of beachline vertices
	class VoronoiViewable{
	public:
		VoronoiViewable();
		VoronoiViewable(fsl3d::Voronoi&); // construct from actual data
		void setBoundary(float bound = 0.5);
		const std::vector<float>& getVertices();
		const std::vector<unsigned int>& getSiteIndices();
		const std::vector<std::vector<unsigned int>>& getCells();
		const std::vector<std::vector<unsigned int>>& getBeachline();
		const std::vector<std::vector<unsigned int>>& getSweepline();
		void setProgress(size_t prog = PSTEPS); // set as int if able to get away with static verts only
		void setNoProgress();
		void setFullProgress();
		void increment();
	private:
		size_t site_count, progress = 0;
		std::vector<float> vverts, rverts, finy;
		std::vector<unsigned int> rsinds;
		std::vector<std::vector<unsigned int>> vfinds, vfaces, rcells, rsweep, rbeach;
		float cbeach[PSTEPS][GSTEPS][GSTEPS];
		float boundary = 0.5;
		void triangulate(std::vector<unsigned int>&, std::vector<unsigned int>);
	};
}


#endif
