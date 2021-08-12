#ifndef FSL3D_VORONOI
#define FSL3D_VORONOI

#include <vector>
#include <utility>
#include <CGAL/Vector_3.h>
#include "fsl3d/common.hpp"

namespace fsl3d{
	class Voronoi{
		// TODO: unique vertices/planes using site/boundary affiliation
	public:
		Voronoi(){}
		Voronoi(const std::vector<vertex_type> &site_verts){
			bb_min = bb_max = (site_verts.size() > 0)? site_verts[0][0]: 0;
			for(auto v: site_verts){
				site_vertices.push_back(v);
				for(int i = 0; i < 3; ++i){
					if(v[i] < bb_min) bb_min = v[i];
					if(v[i] > bb_max) bb_max = v[i];
				}
			}
			scalar_type offset = (bb_max - bb_min) * 10 / 100;
			bb_min -= offset;
			bb_max += offset;
		}
		const std::vector<vertex_type> get_site_vertices() const{
			return site_vertices;
		}
		const std::vector<vertex_type> get_cell_vertices() const{
			return cell_vertices;
		}
		const std::vector<std::vector<int>> get_unique_faces() const{
			return faces;
		}
		const std::vector<std::vector<int>> get_cell_faces(int site_index) const{
			std::vector<std::vector<int>> ret;
			auto v = cell_faces[site_index];
			for(auto face_index: v) ret.push_back(faces[face_index]);
			return ret;
		}
	// private: // commented out for testing
		scalar_type bb_min, bb_max;
		std::vector<vertex_type> site_vertices;
		std::vector<vertex_type> cell_vertices;
		std::vector<std::vector<int>> faces;
		std::vector<std::vector<int>> cell_faces;
		// std::vector<std::pair<int, int>> face_between;
	};
}

#endif
