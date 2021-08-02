#ifndef OGVIEW_VIEWABLEDATA
#define OGVIEW_VIEWABLEDATA


#include <vector>

namespace og{
	class ViewableData{
		public:
		void setProgress(double progress);
		std::vector<float>& getStaticVertices(){
		}
		std::vector<int> getStaticPoints(){
		}
		std::vector<int>& getStaticTriangles(){
		}
		std::vector<float> getDynamicVertices(){
		}
		std::vector<int> getDynamicTriangles(){
		}
	};
}


#endif
