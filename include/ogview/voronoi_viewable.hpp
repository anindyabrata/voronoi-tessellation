#ifndef OGVIEW_VVIEWABLE
#define OGVIEW_VVIEWABLE


#include <cmath>
#include <vector>
#include <CGAL/Cartesian.h>
#include "fsl3d/voronoi.hpp"

namespace ogview{
	const size_t PSTEPS = 500;	// Steps to full progress
	const size_t GSTEPS = 30;	// Density of beachline vertices
	const float boundary = 0.5;	// Bounding box limits for viewer
	class VoronoiViewable{
	public:
		VoronoiViewable();
		VoronoiViewable(fsl3d::Voronoi&); // construct from actual data
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
		float cbeach[PSTEPS][1 + GSTEPS][1 + GSTEPS];
	};
}


#endif
