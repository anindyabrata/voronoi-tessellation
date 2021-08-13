#ifndef OGVIEW_VVIEWABLE
#define OGVIEW_VVIEWABLE


#include <vector>
#include "fsl3d/voronoi.hpp"

namespace ogview{
	class VoronoiViewable{
	public:
		VoronoiViewable();
		VoronoiViewable(const fsl3d::Voronoi&); // construct from actual data
		void setProgress(double prog = 0); // set as int if able to get away with static verts only
		void setBoundary(float bound = 0);
		std::vector<float> getVertices();
		std::vector<unsigned int> getSiteIndices();
		std::vector<unsigned int> getCompletedCellTris();
		std::vector<unsigned int> getBeachlineTris();
	private:
		fsl3d::Voronoi vor;
		float boundary = 0.5;
		double progress = 0;
		void triangulate(std::vector<unsigned int>&, std::vector<unsigned int>);
		void scale_floats(std::vector<float>&);
	};
}


#endif
