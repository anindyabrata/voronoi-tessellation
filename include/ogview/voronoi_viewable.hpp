#ifndef OGVIEW_VVIEWABLE
#define OGVIEW_VVIEWABLE


#include <vector>

namespace ogview{
	class VoronoiViewable{
		public:
		// VoronoiViewable(Voronoi); // construct from actual data
		void setProgress(double progress); // set as int if able to get away with static verts only
		std::vector<float> getStaticVertices();
		std::vector<unsigned int> getSiteIndices();
		std::vector<unsigned int> getCompletedCellTris();
		std::vector<float> getDynamicVertices();
		std::vector<unsigned int> getBeachlineTris();
	};
}


#endif
