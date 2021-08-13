#ifndef FSL3D_VORONOI
#define FSL3D_VORONOI

#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include "fsl3d/common.hpp"

namespace fsl3d{
	class Voronoi{
	public:
		Voronoi(){}
		Voronoi(const std::vector<vertex_type> &site_verts){
			site_count = site_verts.size();

			// calculate bounding box size
			bb_min = bb_max = (site_count > 0)? site_verts[0][0]: 0;

			for(auto v: site_verts){
				for(int i = 0; i < 3; ++i){
					if(v[i] < bb_min) bb_min = v[i];
					if(v[i] > bb_max) bb_max = v[i];
				}
			}

			// make bounding box slightly larger than minimum size necessary
			scalar_type offset = (bb_max - bb_min);
			offset = offset? offset * 10 / 100: 1;
			bb_min -= offset;
			bb_max += offset;

			// create bounding box corner vertices
			for(int mask = 8; mask--; ){
				vertex_type vert(
					(mask & 1)? bb_max: bb_min,
					(mask & 2)? bb_max: bb_min,
					(mask & 4)? bb_max: bb_min);
			    vertices.push_back(vert);
			}

			// copy site verts
			for(auto v: site_verts) vertices.push_back(v);
		}
		const std::vector<vertex_type> get_vertices() const{
			return vertices;
		}
		const std::vector<vertex_type> get_site_vertices() const{
			std::vector<vertex_type> ret;
			for(int i = 0; i < site_count; ++i) ret.push_back(vertices[6 + i]);
			return ret;
		}
		const std::vector<vertex_type> get_cell_vertices() const{
			std::vector<vertex_type> ret;
			for(int i = 6 + site_count; i < vertices.size(); ++i) ret.push_back(vertices[i]);
			return ret;
		}
		const std::vector<std::vector<int>> get_unique_faces() {
			std::vector<std::vector<int>> ret;
			for(int i = 0; i < site_count; ++i) for(auto j: cells[i]) if(j < i) ret.push_back(faces[to_fid(j, i)]);
			return ret;
		}
		const std::vector<std::vector<int>> get_cell_faces(int site_index) {
			std::vector<std::vector<int>> ret;
			for(auto i: cells[site_index]) ret.push_back(faces[to_fid(site_index, i)]);
			return ret;
		}
		int to_fid(int a, int b) const{
			if(a > b) std::swap(a,b);
			return a * site_count + b;
		}
		void sort_face(int fid){
			auto face = faces[fid];
			
			// Get plane of coplanar vertices
			K::Plane_3 pln(vertices[face[0]], vertices[face[1]], vertices[face[2]]);

			// Project 3D points as 2D points on that plane
			std::vector<K::Point_2> twod;
			for(int k = 0; k < face.size(); ++k) twod.push_back(pln.to_2d(vertices[face[k]]));

			// Find center of 2D points
			K::FT mdx = twod[0].x(), mdy = twod[0].y();
			// K::Point_2 md(twod[0]);
			for(int k = 1; k < face.size(); ++k) mdx += twod[k].x(), mdy += twod[k].y();
			mdx /= face.size(), mdy /= face.size();

			// Calculate slopes of 2D points around center to sort by
			std::vector<K::FT> slope;
			for(int k = 0; k < face.size(); ++k) slope.push_back((twod[k].y() - mdy) / (twod[k].x() - mdx));

			// Sort by slopes
			int sorder[face.size()];
			for(int k = 0; k < face.size(); ++k) sorder[k] = k;
			auto cmp = [slope](const int a, const int b) -> bool { return slope[a] < slope[b]; };
			std::sort(sorder, sorder + face.size(), cmp);

			// Replace face with sorted result
			std::vector<int> sorted_face;
			for(int k = 0; k < face.size(); ++k) sorted_face.push_back(face[sorder[k]]);
			faces[fid] = sorted_face;
		}
	// private: // commented out for testing
		size_t site_count = 0;
		scalar_type bb_min, bb_max;
		std::vector<vertex_type> vertices;
		std::map<int, std::vector<int>> faces;
		std::vector<std::vector<int>> cells;
	};
}

#endif
