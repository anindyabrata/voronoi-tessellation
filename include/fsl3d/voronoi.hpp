#ifndef FSL3D_VORONOI
#define FSL3D_VORONOI

#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include "fsl3d/common.hpp"

namespace fsl3d{

	// Class used to represent Voronoi diagram
	class Voronoi{
	public:

		// Empty constructor for convenience
		Voronoi(){}

		// Takes site list as parameter
		// Determines a bounding box
		// Saves vertices for sites and bounding box
		Voronoi(const std::vector<vertex_type> &site_verts){
			site_count = site_verts.size();

			// Calculate bounding box size
			bb_min = bb_max = (site_count > 0)? site_verts[0][0]: 0;

			for(auto v: site_verts){
				for(int i = 0; i < 3; ++i){
					if(v[i] < bb_min) bb_min = v[i];
					if(v[i] > bb_max) bb_max = v[i];
				}
			}

			// Make bounding box slightly larger than minimum size necessary
			scalar_type offset = (bb_max - bb_min);
			offset = offset? offset * 10 / 100: 1;
			bb_min -= offset;
			bb_max += offset;

			// Create bounding box corner vertices
			for(int mask = 8; mask--; ){
				vertex_type vert(
					(mask & 1)? bb_max: bb_min,
					(mask & 2)? bb_max: bb_min,
					(mask & 4)? bb_max: bb_min);
			    vertices.push_back(vert);
			}

			// Copy site vertices
			for(auto v: site_verts) vertices.push_back(v);
		}

		// Get all vertices used in representing this diagram
		const std::vector<vertex_type> get_vertices() const{
			return vertices;
		}

		// Get vertices representing the sites used to construct it
		const std::vector<vertex_type> get_site_vertices() const{
			std::vector<vertex_type> ret;
			for(int i = 0; i < site_count; ++i) ret.push_back(vertices[8 + i]);
			return ret;
		}

		// Get vertices used in cell representation
		// This includes vertices used by bounding box
		const std::vector<vertex_type> get_cell_vertices() const{
			std::vector<vertex_type> ret;
			for(int i = 0; i < 8; ++i) ret.push_back(vertices[i]);
			for(int i = 8 + site_count; i < vertices.size(); ++i) ret.push_back(vertices[i]);
			return ret;
		}

		// Get all faces used for representing this voronoi diagram
		const std::vector<std::vector<unsigned int>> get_unique_faces() {
			std::vector<std::vector<unsigned int>> ret;
			for(int i = 0; i < site_count; ++i) for(auto j: cells[i]) if(j < i) ret.push_back(faces[to_fid(j, i)]);
			return ret;
		}

		// Get indices for all faces used for representing this voronoi diagram
		// in the same order as the output of get_unique_faces()
		const std::vector<std::vector<unsigned int>> get_unique_face_indices() {
			int k = 0;
			std::vector<std::vector<unsigned int>> ret;
			for(int i = 0; i < site_count; ++i) ret.emplace_back();
			for(int i = 0; i < site_count; ++i) for(auto j: cells[i]) if(j < i){
				ret[i].push_back(k);
				if(j >= 0) ret[j].push_back(k);
				++k;
			}
			return ret;
		}

		// Get vertex indices representing cell faces
		const std::vector<std::vector<unsigned int>> get_cell_faces(int site_index) {
			std::vector<std::vector<unsigned int>> ret;
			for(auto i: cells[site_index]) ret.push_back(faces[to_fid(site_index, i)]);
			return ret;
		}

		// Convert pair of site indices to face id
		int to_fid(int a, int b) const{
			if(a > b) std::swap(a,b);
			return a * site_count + b;
		}

		// Convert face id to pair of site indices
		// -ve site indices indicate bounding box
		void from_fid(int fid, int &a, int &b) const{
			a = fid / (int)site_count;
			b = fid % (int)site_count;
			if(b < 0){
				--a;
				b += (int)site_count;
			}
		}

		// Sort vertices in face of id fid in order of edges between them
		void sort_face(int fid){
			auto face = faces[fid];
			
			// Get plane of coplanar vertices
			K::Plane_3 pln(vertices[face[0]], vertices[face[1]], vertices[face[2]]);

			// Project 3D points as 2D points on that plane
			std::vector<K::Point_2> twod;
			for(int k = 0; k < face.size(); ++k) twod.push_back(pln.to_2d(vertices[face[k]]));

			// Find center of 2D points
			K::FT mdx = twod[0].x(), mdy = twod[0].y();
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
			std::vector<unsigned int> sorted_face;
			for(int k = 0; k < face.size(); ++k) sorted_face.push_back(face[sorder[k]]);
			faces[fid] = sorted_face;
		}

		// Reduce the face of id fid by bisector plane between sites of indices i and j
		// face contains reduced face; the part closer to site i
		// returned vector contains new vertices formed due to the bisection if any
		std::vector<unsigned int> bisect_face(int fid, int i, int j, std::vector<unsigned int> &face){
			std::vector<unsigned int> intersect;
			auto old_face = faces[fid];
			std::vector<bool> closer;
			for(auto verti: old_face) closer.push_back(CGAL::has_smaller_distance_to_point(vertices[verti], vertices[8 + i], vertices[8 + j]));
			for(int k = 0; k < old_face.size(); ++k){
				int kr = (1 + k) % old_face.size();
				if(closer[k] && closer[kr]){
					if(!face.size() || face.back() != old_face[k]) face.push_back(old_face[k]);
					face.push_back(old_face[kr]);
				}
				else if(closer[k] != closer[kr]){
					if(closer[k] && (!face.size() || face.back() != old_face[k])) face.push_back(old_face[k]);

					// Calculate intersecting vertex (between current edge and bisector plane)
					auto bplane = CGAL::bisector(vertices[8 + i], vertices[8 + j]);
					K::Line_3 edge(vertices[old_face[k]], vertices[old_face[kr]]);
					auto calcres = CGAL::intersection(edge, bplane);
					vertex_type calcvert = *boost::get<vertex_type>(&*calcres);

					// Add to vertices list and get index
					int vi = vertices.size();
					vertices.push_back(calcvert);

					// Add vertex to face (perform bisection)
					face.push_back(vi);
					intersect.push_back(vi);

					if(closer[kr]) face.push_back(old_face[kr]);
				}
			}
			// Remove possible duplication of end vertices
			if(face.size() && face.front() == face.back()) face.pop_back();

			// Return list of intersecting vertices
			return intersect;
		}
		size_t site_count = 0;
		scalar_type bb_min, bb_max;
		std::vector<vertex_type> vertices;
		std::map<int, std::vector<unsigned int>> faces;
		std::vector<std::vector<int>> cells;
	};
}

#endif
