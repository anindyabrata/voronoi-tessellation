#ifndef OGVIEW_VVIEWABLE
#define OGVIEW_VVIEWABLE


#include <cmath>
#include <vector>
#include <CGAL/Cartesian.h>
#include "fsl3d/voronoi.hpp"

namespace ogview{
	const size_t PSTEPS = 200;	// Steps to full progress
	const size_t GSTEPS = 30;	// Density of beachline vertices
	const float boundary = 0.5;	// Bounding box limits for viewer

	// Makes voronoi diagram data easier to process in openGL
	// Generates data needed for fortune's algorithm simulation
	class VoronoiViewable{
	public:
		VoronoiViewable(fsl3d::Voronoi&); // Construct from voronoi data
		const std::vector<float>& getVertices(); // Get flattened euclidean vertex data
		const std::vector<unsigned int>& getSiteIndices(); // Get index to site vertices
		const std::vector<std::vector<unsigned int>>& getCells(); // Get indices for cell faces
		const std::vector<std::vector<unsigned int>>& getBeachline(); // Get indices for beachline faces
		const std::vector<std::vector<unsigned int>>& getSweepline(); // Get indices for sweepline faces
		void setProgress(size_t prog = PSTEPS); // Set progress of simulation
		void setNoProgress(); // Set progress to simulation beginning
		void setFullProgress(); // Set progress to simulation end
		void increment(); // Go to next progress step and wrap around
	private:
		size_t site_count, progress = 0;
		std::vector<float> vverts, rverts, finy;
		std::vector<unsigned int> rsinds;
		std::vector<std::vector<unsigned int>> vfinds, vfaces, rcells, rsweep, rbeach;
		float cbeach[PSTEPS][1 + GSTEPS][1 + GSTEPS];
	};
}


#endif
