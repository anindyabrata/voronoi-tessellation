#ifndef FSL3D_VORONOI
#define FSL3D_VORONOI

#include <vector>
#include <utility>
#include <CGAL/Vector_3.h>
#include "fsl3d/common.hpp"

namespace fsl3d{
	class Voronoi{
		// 
		// Get site vertices
		// Get vertices describing cells
		// Get all unique faces
		//		Faces are vectors of vertices in connected order (not tris)
		// Get face indices describing particular cells
	public:
		Voronoi(){}
		Voronoi(const std::vector<vertex_type> &site_verts){
			for(auto v: site_verts) site_vertices.push_back(v);
			bb_min = bb_max = site_verts[0];
			for(auto v: site_verts){
				K::FT x, y, z;
				x = v.x() < bb_min.x()? v.x(): bb_min.x();
				y = v.y() < bb_min.y()? v.y(): bb_min.y();
				z = v.z() < bb_min.z()? v.z(): bb_min.z();
				bb_min = vertex_type(x, y, z);
				x = v.x() > bb_max.x()? v.x(): bb_max.x();
				y = v.y() > bb_max.y()? v.y(): bb_max.y();
				z = v.z() > bb_max.z()? v.z(): bb_max.z();
				bb_max = vertex_type(x, y, z);
			}
			CGAL::Vector_3<K> offset(1, 1, 1);
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
		vertex_type bb_min, bb_max;
		std::vector<vertex_type> site_vertices;
		std::vector<vertex_type> cell_vertices;
		std::vector<std::vector<int>> faces;
		std::vector<std::vector<int>> cell_faces;
		// std::vector<std::pair<int, int>> face_between;
	};
}

#endif
